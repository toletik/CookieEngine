#include <D3D11Helper.hpp>
#include "Render/RendererRemote.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/Map.hpp"
#include "Primitives.hpp"
#include "Render/Drawers/MapDrawer.hpp"

using namespace Cookie::Render;
using namespace Cookie::Core::Math;

struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Vec4 tileNb;
};

struct PS_CONSTANT_BUFFER
{
    Vec4 limitColor;
    Vec4 tileLimits;
};

/*============================= CONSTRUCTORS/DESTRUCTORS =============================*/

MapDrawer::MapDrawer():
    mapMesh{ Core::Primitives::CreateCube() }
{
	InitShader();
}

MapDrawer::~MapDrawer()
{
    if (VShader)
    {
        VShader->Release();
    }
    if (PShader)
    {
        PShader->Release();
    }
    if (VCBuffer)
    {
        VCBuffer->Release();
    }
    if (PCBuffer)
    {
        PCBuffer->Release();
    }
    if (ILayout)
    {
        ILayout->Release();
    }
}

/*============================= INIT METHODS =============================*/

void MapDrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 27
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv       : UV; 
        float3 normal   : NORMAL;
        float3 fragPos  : FRAGPOS;
        float4 view     : VIEW;
    };
    
    cbuffer MODEL_CONSTANT : register(b0)
    {
        float4x4  model;
        float2    tileSize;
    };

    cbuffer CAM_CONSTANT : register(b1)
    {
        float4x4  proj;
        float4x4  view;
    };

    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
    {
        VOut output;
    
        output.fragPos  = mul(float4(position,1.0),model).xyz;
        output.view     = mul(float4(output.fragPos,1.0),view);
        output.position = mul(output.view, proj);
        output.uv       = uv * tileSize;
        output.normal   = normalize(mul(normal,(float3x3)model));
    
        return output;
    }
    )";

    Render::CompileVertex(source, &blob, &VShader);
    source = (const char*)R"(#line 93

    Texture2D	albedoTex           : register(t0);
    Texture2D	normalTex           : register(t1); 
    Texture2D	metallicRoughness   : register(t2); 

    cbuffer MODEL_CONSTANT : register(b0)
    {
        float3 limitColor;
        float2 tileLimits;
    };

    SamplerState WrapSampler : register(s0);

    float3x3 cotangent_frame(float3 normal, float3 p, float2 uv)
    {
        // get edge vectors of the pixel triangle
        float3 dp1 = ddx( p );
        float3 dp2 = ddy( p );
        float2 duv1 = ddx( uv );
        float2 duv2 = ddy( uv );
     
        // solve the linear system
        float3 dp2perp = cross( dp2, normal );
        float3 dp1perp = cross( normal, dp1 );
        float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
        float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
     
        // construct a scale-invariant frame 
        float invmax = rsqrt( max( dot(T,T), dot(B,B) ) );
        return float3x3( T * invmax, B * invmax, normal );
    }

    float3 perturb_normal(float3 normal, float3 viewEye, float2 uv, float3 texNormal)
    {
        // assume N, the interpolated vertex normal and 
        // V, the view vector (vertex to eye)
        texNormal = (texNormal * 2.0 - 1.0);
        float3x3 TBN = cotangent_frame(normal, viewEye, uv);
        return normalize(mul(texNormal,TBN));
    }

    struct POut
    {
        float4 position : SV_TARGET0;
        float4 normal : SV_TARGET1; 
        float4 albedo : SV_TARGET2;
    };

    POut main(float4 position : SV_POSITION, float2 uv : UV, float3 normal : NORMAL, float3 fragPos : FRAGPOS, float4 view : VIEW) 
    {
        POut pOutput;

        float3 texNormal        = normalTex.Sample(WrapSampler,uv).xyz;
        float3 viewEye          = normalize(view.xyz);
        float3 perturbNormal    = perturb_normal(normal,viewEye,uv,texNormal);

        pOutput.position    = float4(fragPos,metallicRoughness.Sample(WrapSampler,uv).b);
        pOutput.normal      = lerp(float4(normal,metallicRoughness.Sample(WrapSampler,uv).g),float4(perturbNormal,metallicRoughness.Sample(WrapSampler,uv).g),step(0.1,dot(texNormal,texNormal)));
        float2  mapUv       = frac(uv);

        if (mapUv.x <= tileLimits.x || mapUv.x >= tileLimits.y || mapUv.y <= tileLimits.x || mapUv.y >= tileLimits.y)
        {
            pOutput.albedo = float4(limitColor,metallicRoughness.Sample(WrapSampler,uv).r);
        }
        else
        {
            pOutput.albedo = float4(albedoTex.Sample(WrapSampler,uv).rgb,metallicRoughness.Sample(WrapSampler,uv).r);
        }
        return pOutput;
    })";

    struct Vertex
    {
        Core::Math::Vec3 position;
        Core::Math::Vec2 uv;
        Core::Math::Vec3 normal;
    };

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex,position),  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV",       0, DXGI_FORMAT_R32G32_FLOAT,    0,     offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    Render::CreateLayout(&blob, ied, 3, &ILayout);

    Render::CompilePixel(source, &PShader);

    VS_CONSTANT_BUFFER vbuffer = {};

    Render::CreateBuffer(&vbuffer, sizeof(VS_CONSTANT_BUFFER), &VCBuffer);

    PS_CONSTANT_BUFFER pbuffer = {};

    Render::CreateBuffer(&pbuffer, sizeof(PS_CONSTANT_BUFFER), &PCBuffer);

    blob->Release();
}

/*============================= REALTIME METHODS =============================*/

void MapDrawer::Set(const Resources::Map& map)
{
    albedoTex = map.model.albedo;
    normalTex = map.model.normal;
    matTex = map.model.metallicRoughness;

    model = map.trs.TRS;
    tileNb = map.tilesNb;
}

void MapDrawer::Draw()
{
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);
    Render::RendererRemote::context->IASetInputLayout(ILayout);

    VS_CONSTANT_BUFFER vbuffer   = {};
    vbuffer.model                = model;
    vbuffer.tileNb             = { tileNb.x,tileNb.y,0.0f,0.0f };

    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &VCBuffer);
    Render::WriteBuffer(&vbuffer, sizeof(vbuffer), 0, &VCBuffer);

    PS_CONSTANT_BUFFER pbuffer = {};
    pbuffer.limitColor = { limitColor.r,limitColor.g ,limitColor.b, 1.0f };
    pbuffer.tileLimits = { tileLimits.x,tileLimits.y,0.0f,0.0f };
    Render::RendererRemote::context->PSSetConstantBuffers(0, 1, &PCBuffer);
    Render::WriteBuffer(&pbuffer, sizeof(pbuffer), 0, &PCBuffer);

    if (albedoTex)
        albedoTex->Set(0);
    if (normalTex)
        normalTex->Set(1);
    if (matTex)
        matTex->Set(2);

    mapMesh->Set();
    mapMesh->Draw();
}
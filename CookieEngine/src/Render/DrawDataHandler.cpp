#include "D3D11Helper.hpp"
#include "Camera.hpp"
#include "ECS/Coordinator.hpp"
#include "ECS/ComponentHandler.hpp"
#include "Game.hpp"
#include "Scene.hpp"
#include "Resources/Mesh.hpp"
#include "DrawDataHandler.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Render;

constexpr float cullEpsilon = -3.0f;

struct VS_CONSTANT_BUFFER
{
	Mat4 model = Cookie::Core::Math::Mat4::Identity();
};

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

DrawDataHandler::DrawDataHandler()
{
	InitCBuffer();
}

DrawDataHandler::~DrawDataHandler()
{
	if (CBuffer)
		CBuffer->Release();
}

/*========================= INIT METHODS =========================*/

void DrawDataHandler::InitCBuffer()
{
	VS_CONSTANT_BUFFER buffer = {};

	Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &CBuffer);
}

void DrawDataHandler::Init(const Game& game)
{
	coordinator			= &game.coordinator;
	player				= &game.playerData;

	depthStencilView	= game.renderer.geomPass.depthBuffer;
	CamCBuffer			= game.renderer.geomPass.CBuffer;
}

/*========================= REALTIME METHODS =========================*/

void Frustrum::MakeFrustrum(const Camera& cam)
{
	Mat4 viewProj = cam.GetViewProj();

	//left plane
	planes[0].x = viewProj.c[3].e[0] + viewProj.c[0].e[0];
	planes[0].y = viewProj.c[3].e[1] + viewProj.c[0].e[1];
	planes[0].z = viewProj.c[3].e[2] + viewProj.c[0].e[2];
	planes[0].w = viewProj.c[3].e[3] + viewProj.c[0].e[3];

	//right plane
	planes[1].x = viewProj.c[3].e[0] - viewProj.c[0].e[0];
	planes[1].y = viewProj.c[3].e[1] - viewProj.c[0].e[1];
	planes[1].z = viewProj.c[3].e[2] - viewProj.c[0].e[2];
	planes[1].w = viewProj.c[3].e[3] - viewProj.c[0].e[3];

	//top plane
	planes[2].x = viewProj.c[3].e[0] - viewProj.c[1].e[0];
	planes[2].y = viewProj.c[3].e[1] - viewProj.c[1].e[1];
	planes[2].z = viewProj.c[3].e[2] - viewProj.c[1].e[2];
	planes[2].w = viewProj.c[3].e[3] - viewProj.c[1].e[3];

	//bottom plane
	planes[3].x = viewProj.c[3].e[0] + viewProj.c[1].e[0];
	planes[3].y = viewProj.c[3].e[1] + viewProj.c[1].e[1];
	planes[3].z = viewProj.c[3].e[2] + viewProj.c[1].e[2];
	planes[3].w = viewProj.c[3].e[3] + viewProj.c[1].e[3];

	//near plane
	planes[4].x = viewProj.c[2].e[0];
	planes[4].y = viewProj.c[2].e[1];
	planes[4].z = viewProj.c[2].e[2];
	planes[4].w = viewProj.c[2].e[3];

	//far plane
	planes[5].x = viewProj.c[3].e[0] - viewProj.c[2].e[0];
	planes[5].y = viewProj.c[3].e[1] - viewProj.c[2].e[1];
	planes[5].z = viewProj.c[3].e[2] - viewProj.c[2].e[2];
	planes[5].w = viewProj.c[3].e[3] - viewProj.c[2].e[3];

	for (int i = 0; i < planes.size(); i++)
	{
		planes[i] = planes[i].Normalize();
	}

	//the vectors of the referential of cam
	Core::Math::Vec3 camFwd		= { viewProj.c[2].x,viewProj.c[2].y,viewProj.c[2].z };
	Core::Math::Vec3 camRight	= { viewProj.c[0].x,viewProj.c[0].y,viewProj.c[0].z };
	Core::Math::Vec3 camUp		= { viewProj.c[1].x,viewProj.c[1].y,viewProj.c[1].z };
	camFwd		= camFwd.Normalize();
	camRight	= camRight.Normalize();
	camUp		= camUp.Normalize();

	//height and width of far plane
	float heightFar = 2.0f * tanf(Core::Math::ToRadians(cam.fov) / 2.0f)*cam.camFar;
	float widthFar = heightFar * cam.aspectRatio;

	//height and width of near plane
	float heightNear = 2.0f * tanf(Core::Math::ToRadians(cam.fov) / 2.0f) * cam.camNear;
	float widthNear = heightNear * cam.aspectRatio;

	//Center of each plane
	Core::Math::Vec3 nearCenter = cam.pos + camFwd * cam.camNear;
	Core::Math::Vec3 farCenter	= cam.pos + camFwd * cam.camFar;
	centroid = cam.pos + camFwd * ((cam.camNear + cam.camFar) / 2.0f);

	//the corners of the frustrum
	corners[0] = nearCenter - (camUp * (heightNear / 2.0f)) - (camRight * (widthNear / 2.0f));
	corners[1] = nearCenter - (camUp * (heightNear / 2.0f)) + (camRight * (widthNear / 2.0f));
	corners[2] = nearCenter + (camUp * (heightNear / 2.0f)) - (camRight * (widthNear / 2.0f));
	corners[3] = nearCenter + (camUp * (heightNear / 2.0f)) + (camRight * (widthNear / 2.0f));
	corners[4] = farCenter - (camUp * (heightFar / 2.0f)) - (camRight * (widthFar / 2.0f));
	corners[5] = farCenter - (camUp * (heightFar / 2.0f)) + (camRight * (widthFar / 2.0f));
	corners[6] = farCenter + (camUp * (heightFar / 2.0f)) - (camRight * (widthFar / 2.0f));
	corners[7] = farCenter + (camUp * (heightFar / 2.0f)) + (camRight * (widthFar / 2.0f));
}

void DrawDataHandler::SetDrawData(const Camera* cam, const Game& game)
{
	currentCam			= cam;
	frustrum.MakeFrustrum(*cam);

	const Coordinator&			coord			= *coordinator;
	const ECS::EntityHandler&	entityHandler	= *coord.entityHandler;
	ECS::ComponentHandler&		components		= *coord.componentHandler;

	bool cull = false;

	mapDrawer.Set(game.scene->map);

	for (int i = 0; i < entityHandler.livingEntities; ++i)
	{
		const Entity& iEntity = entityHandler.entities[i];
		if ((iEntity.signature & (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL)) == (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
		{
			ECS::ComponentModel& model = components.GetComponentModel(iEntity.id);

			if (model.mesh == nullptr)
			{
				continue;
			}

			Core::Math::Mat4& trs = components.GetComponentTransform(iEntity.id).TRS;

			Vec4 modelMin = trs * Core::Math::Vec4(model.mesh->AABBMin, 1.0f);
			Vec4 modelMax = trs * Core::Math::Vec4(model.mesh->AABBMax, 1.0f);

			for (int j = 0; j < frustrum.planes.size(); j++)
			{

				if ((frustrum.planes[j].Dot(modelMin) + frustrum.planes[j].w) < cullEpsilon && (frustrum.planes[j].Dot(modelMax) + frustrum.planes[j].w) < cullEpsilon)
				{
					cull = true;
					break;
				}

			}

			if (cull)
			{
				cull = false;
				continue;
			}

			AABB[0].x = std::min(modelMin.x, AABB[0].x);
			AABB[0].y = std::min(modelMin.y, AABB[0].y);
			AABB[0].z = std::min(modelMin.z, AABB[0].z);

			AABB[1].x = std::max(modelMax.x, AABB[1].x);
			AABB[1].y = std::max(modelMax.y, AABB[1].y);
			AABB[1].z = std::max(modelMax.z, AABB[1].z);

			ECS::ComponentGameplay& iGameplay = components.GetComponentGameplay(iEntity.id);

			//if (iGameplay.signatureGameplay & (CGP_SIGNATURE::PRODUCER | CGP_SIGNATURE::WORKER))
			//{
			//
			//}
			//else
			{
				models.push_back(model);
				matrices.push_back(trs);
			}
		}
	}

	for (int i = 0; i < coord.selectedEntities.size(); ++i)
	{
		Entity& iEntity = *coord.selectedEntities[i];
		if ((iEntity.signature & (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL)) == (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
		{
			ECS::ComponentModel& model = components.GetComponentModel(iEntity.id);

			if (model.mesh == nullptr)
			{
				continue;
			}

			selectedModels.push_back(model);
			selectedMatrices.push_back(components.GetComponentTransform(iEntity.id).TRS);
			selectedGameplays.push_back(components.GetComponentGameplay(iEntity.id));
		}
	}
}

void DrawDataHandler::Draw(int _i)
{
	ID3D11ShaderResourceView* fbos[3] = { nullptr, nullptr, nullptr };

	VS_CONSTANT_BUFFER buffer;

	size_t bufferSize = sizeof(buffer);

	for (int i = _i; i < models.size(); i++)
	{
		buffer.model = matrices[i];
		Render::WriteCBuffer(&buffer, bufferSize, 0, &CBuffer);

		const ECS::ComponentModel& iModel = models[i];

		if (iModel.albedo)
			iModel.albedo->Set(0);
		if (iModel.normal)
			iModel.normal->Set(1);
		if (iModel.metallicRoughness)
			iModel.metallicRoughness->Set(2);
		if (iModel.mesh)
		{
			iModel.mesh->Set();
			iModel.mesh->Draw();
		}

		Render::RendererRemote::context->PSSetShaderResources(0, 3, fbos);
	}
}

void DrawDataHandler::Clear()
{
	models.clear();
	matrices.clear();
	selectedModels.clear();
	selectedMatrices.clear();
	selectedGameplays.clear();
	AABB[0] = { std::numeric_limits<float>().max(),std::numeric_limits<float>().max() ,std::numeric_limits<float>().max() };
	AABB[1] = { -std::numeric_limits<float>().max(), -std::numeric_limits<float>().max() , -std::numeric_limits<float>().max() };
}
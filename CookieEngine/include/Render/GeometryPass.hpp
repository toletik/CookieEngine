#ifndef __GEOMETRY_PASS_HPP__
#define __GEOMETRY_PASS_HPP__

#include "FrameBuffer.hpp"

namespace Cookie
{
	namespace Core::Math
	{
		union Vec4;
		union Mat4;
	}

	namespace ECS
	{
		class Coordinator;
	}

	namespace Render
	{
		class GeometryPass
		{
			private:
				ID3D11InputLayout*	ILayout{ nullptr };
				ID3D11VertexShader* VShader{ nullptr };
				ID3D11PixelShader*	PShader{ nullptr };
				ID3D11SamplerState*	PSampler{ nullptr };

				ID3D11DepthStencilState*	depthStencilState	= nullptr;
				ID3D11RasterizerState*		rasterizerState		= nullptr;
				ID3D11BlendState*			blendState			= nullptr;

			public:
				ID3D11DepthStencilView* depthBuffer = nullptr;
				ID3D11Buffer* CBuffer{ nullptr };

				FrameBuffer	posFBO;
				FrameBuffer	normalFBO;
				FrameBuffer	albedoFBO;

			private:
				void InitShader();
				void InitState();

			public:
				void CreateDepth(int width, int height);

				GeometryPass(int width, int height);
				~GeometryPass();

				void Set();
				void Draw(const Core::Math::Mat4& viewProj, const ECS::Coordinator& coordinator);
				void Clear();
		};
	}
}

#endif /*__GEOMETRY_PASS_HPP__*/
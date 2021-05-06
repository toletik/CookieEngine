#ifndef __PARTICLESPASS_HPP__
#define __PARTICLESPASS_HPP__

#include <reactphysics3d/reactphysics3d.h>
#include "D3D11Helper.hpp"
#include "Mat4.hpp"
#include "Vec4.hpp"


namespace Cookie
{
	namespace Core::Math
	{
		//union Mat4;
		//union Vec4;
	}

	namespace ECS
	{
		class Coordinator;
	}

	namespace Resources
	{
		class Mesh;
	}

	namespace Render
	{
		struct InstancedData
		{
			Cookie::Core::Math::Mat4 World;
			Cookie::Core::Math::Vec4 Color;
		};

		class ParticlesPass
		{
		public:
			ID3D11InputLayout* ILayout{ nullptr };
			ID3D11VertexShader* VShader{ nullptr };
			ID3D11PixelShader* PShader{ nullptr };
			ID3D11Buffer* CBuffer{ nullptr };
			ID3D11Buffer* InstanceBuffer{ nullptr };

			std::vector<InstancedData> mInstancedData;

			void InitShader();
			void Draw(const Core::Math::Mat4& viewProj, Resources::Mesh* mesh, std::vector<InstancedData> data);


			ParticlesPass();
			~ParticlesPass();
		};
	}
}

#endif
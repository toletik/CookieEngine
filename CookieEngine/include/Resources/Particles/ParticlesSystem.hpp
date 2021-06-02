#ifndef __PARTICLESSYSTEM_HPP__
#define __PARTICLESSYSTEM_HPP__


#include "ParticlesEmitter.hpp"
#include "ParticlesData.hpp"
#include "ComponentTransform.hpp"
#include "ParticlesPass.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Camera.hpp"
#include "DrawDataHandler.hpp"
#include "Resources/ResourcesManager.hpp"

namespace Cookie
{
	namespace Resources
	{
		namespace Particles
		{
			struct emit
			{
				std::string name;
				Cookie::Core::Math::Vec3 data[2];
				std::string nameData;
			};

			class ParticlesPrefab
			{
			public:
				std::string name;
				struct data
				{
					Mesh* mesh;
					Texture* texture;
					int size;
					int countFrame;
					int countAlive;
					bool isBillboard;
				};
				
				std::vector<data> data;
				std::vector<ParticlesEmitter> emitter;
				std::vector<std::vector<emit>> emit;
			};

			class ParticlesSystem
			{
			public :
				std::vector<ParticlesData> data;
				std::vector<ParticlesEmitter> particlesEmiter;

				Core::Math::Mat4 trs = Core::Math::Mat4::Identity();
				bool needToBeRemoved = false;

				std::string name;

				ParticlesSystem() 
				{}

				ParticlesSystem(const ParticlesSystem& other): data(other.data), particlesEmiter(other.particlesEmiter), 
					trs(other.trs), needToBeRemoved(other.needToBeRemoved), name(other.name)
				{}

				ParticlesSystem(Cookie::Render::ParticlesPass* shader)
				{}

				ParticlesSystem(int size, int sizeFrame)
				{
					data.push_back(ParticlesData());
					particlesEmiter.push_back(ParticlesEmitter());
					data[0].generate(size, sizeFrame);
				}
				~ParticlesSystem() 
				{
					data.clear();
					particlesEmiter.clear();
				}

				void generate();
				void Update();
				void Draw(const Render::Camera& cam, Render::Frustrum& frustrum);
			};
		}
	}
}

#endif // !__PARTICLESSYSTEM_HPP__

#ifndef __PARTICLESSYSTEM_HPP__
#define __PARTICLESSYSTEM_HPP__

#include "ParticlesEmitter.hpp"
#include "ParticlesData.hpp"
#include "ComponentTransform.hpp"
#include "ParticlesPass.hpp"
#include "Resources/Mesh.hpp"

namespace Cookie
{
	namespace Resources
	{
		namespace Particles
		{
			class ParticlesSystem
			{
			public :
				std::vector<ParticlesData> data;
				std::vector<ParticlesEmitter> particlesEmiter;

				ECS::ComponentTransform trs;

				Cookie::Render::ParticlesPass shader;

				ParticlesSystem() {}

				ParticlesSystem(int size, int sizeFrame)
				{
					shader.InitShader();
					data.push_back(ParticlesData());
					particlesEmiter.push_back(ParticlesEmitter());
					data[0].generate(size, sizeFrame);
				}
				~ParticlesSystem() {}

				void Update()
				{
					for (int j = 0; j < data.size(); j++)
					{
						if (data[j].countAlive < data[j].countFrame)
						{
							float countAlive = data[j].countAlive;
							float countFrame = data[j].countFrame;

							data[j].wake(countAlive, countFrame);

							for (int i = 0; i < particlesEmiter[j].generators.size(); i++)
								particlesEmiter[j].generators[i]->generate(&data[j], countAlive, countFrame);
						}

						for (int i = 0; i < data[j].countAlive; i++)
						{
							for (int k = 0; k < particlesEmiter[j].updates.size(); k++)
								particlesEmiter[j].updates[k]->Update(&data[j], i);
							if (data[j].time[i] < 0)
								data[j].kill(i);
						}
					}
				}

				void Draw(const Core::Math::Mat4& viewProj, Cookie::Resources::ResourcesManager& resources)
				{
					std::vector<Cookie::Render::InstancedData> newData;

					for (int j = 0; j < data.size(); j++)
					{
						for (int i = 0; i < data[j].countAlive; i++)
						{
							Cookie::Render::InstancedData temp;
							temp.World = (trs.TRS * Cookie::Core::Math::Mat4::Translate(data[j].pos[i]));
							temp.Color = Cookie::Core::Math::Vec4(1, 0, 0, 1);
							newData.push_back(temp);
						}

						shader.Draw(viewProj, resources.meshes["Quad"].get(), newData);
					}
				}
			};
		}
	}
}

#endif // !__PARTICLESSYSTEM_HPP__

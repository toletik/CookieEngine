#ifndef __PARTICLESDATA_HPP__
#define __PARTICLESDATA_HPP__

#include <vector>
#include "Vec4.hpp"
#include "ParticlesEmitter.hpp"
#include "ParticlesGenerator.hpp"

namespace Cookie
{
	namespace Core::Math
	{
		union Vec3;
	}
	namespace Resources
	{
		class Mesh;
		class Shader;
		class Texture;

		namespace Particles
		{  

			class ParticlesData
			{
			public:
				std::vector<Cookie::Core::Math::Vec3>	pos;
				std::vector<Cookie::Core::Math::Vec3>	vel;
				std::vector<Cookie::Core::Math::Vec4>	col;
				std::vector<Cookie::Core::Math::Vec4>	acc;
				std::vector<float>						time;
				std::vector<float>						mass;
				std::vector<bool>						alive;

				int										count = 0;
				int										countFrame = 0;
				int										countAlive = 0;
				int										signature = 0b0000;

				ParticlesData() {}
				~ParticlesData() {}

				void generate(int size, int sizeFrame) 
				{
					count = size;
					countFrame = sizeFrame;
					countAlive = 0;

					pos.resize(size);
					vel.resize(size);
					col.resize(size);
					acc.resize(size);
					time.resize(size);
					alive.resize(size);
					mass.resize(size);
				}

				void kill(int index) 
				{
					alive[index] = false;
					swapData(index, countAlive - 1);
					countAlive -= 1;
				}

				void wake(int indexBegin, int indexEnd)
				{
					for (int i = indexBegin; i <= indexEnd; i++)
					{
						alive[i] = true;
						countAlive += 1;
					}
				}

				void swapData(int indexA, int indexB) 
				{
					std::iter_swap(pos.begin() + indexA, pos.begin() + indexB);
					std::iter_swap(vel.begin() + indexA, vel.begin() + indexB);
					std::iter_swap(col.begin() + indexA, col.begin() + indexB);
					std::iter_swap(acc.begin() + indexA, acc.begin() + indexB);
					std::iter_swap(time.begin() + indexA, time.begin() + indexB);
					std::iter_swap(alive.begin() + indexA, alive.begin() + indexB);
					std::iter_swap(mass.begin() + indexA, mass.begin() + indexB);
				}
			};
			
		}
	}
}


#endif // !__PARTICLESDATA_HPP__

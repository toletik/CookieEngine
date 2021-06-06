#ifndef __PARTICLESEMITER_HPP__
#define __PARTICLESEMITER_HPP__

#include <vector>
#include "ParticlesGenerator.hpp"
#include "ParticlesUpdate.hpp"

namespace Cookie
{
	namespace Resources
	{
		namespace Particles
		{
			class ParticlesGenerator;

			enum COMPONENTADD
			{
				PPGEN		= 0b0000000000000000000000001,
				BPGEN		= 0b0000000000000000000000010,
				CPGEN		= 0b0000000000000000000000100,
				SCGEN		= 0b0000000000000000000001000,
				SRGEN		= 0b0000000000000000000010000,
				RRGEN		= 0b0000000000000000000100000,
				VCGEN		= 0b0000000000000000001000000,
				VRGEN		= 0b0000000000000000010000000,
				MCGEN		= 0b0000000000000000100000000,
				TCGEN		= 0b0000000000000001000000000,

				TRGEN		= 0b0000000000000010000000000,
				CCGEN		= 0b0000000000000100000000000,
				CRGEN		= 0b0000000000001000000000000,
				IVWP		= 0b0000000000010000000000000,
				UPVEL		= 0b0000000000100000000000000,
				UPSCALE		= 0b0000000001000000000000000,
				UPALPHA		= 0b0000000010000000000000000,

				COL			= 0b0000000100000000000000000,
				EG			= 0b0000001000000000000000000,
				UPT			= 0b0000010000000000000000000,
				LP			= 0b0000100000000000000000000,
				CWP			= 0b0001000000000000000000000,
				CP			= 0b0010000000000000000000000,
				SH			= 0b0100000000000000000000000,
				SP			= 0b1000000000000000000000000
			};


			class ParticlesEmitter
			{
			public:
				std::vector<std::shared_ptr<ParticlesGenerator>> generators;
				std::vector<std::shared_ptr<ParticlesUpdate>>  updates;

				int componentAdd = 0;

				ParticlesEmitter() {}
				~ParticlesEmitter() {}
			};
		}
	}
}

#endif // !__PARTICLESEMITER_HPP__

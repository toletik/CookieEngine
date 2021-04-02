#ifndef __PHYSICS_HANDLE_HPP__
#define __PHYSICS_HANDLE_HPP__

#include <reactphysics3d/reactphysics3d.h>

namespace Cookie
{
	namespace Physics
	{
		class PhysicsHandle
		{
			public:
				inline static const std::unique_ptr<rp3d::PhysicsCommon>	physCom{std::make_unique<rp3d::PhysicsCommon>()};
				inline static rp3d::PhysicsWorld*							physSim{nullptr};
				inline static rp3d::PhysicsWorld*							editWorld{Physics::PhysicsHandle::physCom->createPhysicsWorld()};

		};
	}
}

#endif // !__PHYSICS_HANDLE_HPP__
#ifndef __PHYSICS_HANDLE_HPP__
#define __PHYSICS_HANDLE_HPP__

#include "Physics/PhysicsSimulator.hpp"

namespace Cookie
{
	namespace Physics
	{
		class PhysicsHandle
		{
			public:
				inline static std::unique_ptr<rp3d::PhysicsCommon>			physCom{nullptr};
				inline static PhysicsSimulator								physSim;
				inline static rp3d::PhysicsWorld*							editWorld{nullptr};

				static void Init()
				{
					physCom = std::make_unique<rp3d::PhysicsCommon>();
					physSim.worldSim = PhysicsHandle::physCom->createPhysicsWorld();
				}

				static void Terminate()
				{
					int max = physSim.worldSim->getNbRigidBodies();
					for (int i = 0; i < max; i++)
					{
						physSim.worldSim->destroyRigidBody(physSim.worldSim->getRigidBody(0));
					}

					PhysicsHandle::physCom->destroyPhysicsWorld(physSim.worldSim);

					if (editWorld)
					{
						int max = Physics::PhysicsHandle::editWorld->getNbRigidBodies();
						for (int i = 0; i < max; i++)
						{
							Physics::PhysicsHandle::editWorld->destroyRigidBody(Physics::PhysicsHandle::editWorld->getRigidBody(0));
						}

						Physics::PhysicsHandle::physCom->destroyPhysicsWorld(Physics::PhysicsHandle::editWorld);
					}
				}

		};
	}
}

#endif // !__PHYSICS_HANDLE_HPP__

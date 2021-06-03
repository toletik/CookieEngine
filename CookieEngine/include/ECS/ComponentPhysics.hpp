#ifndef __COMPONENT_PHYSICS_HPP__
#define __COMPONENT_PHYSICS_HPP__

#include "Physics/PhysicsHandle.hpp"

namespace reactphysics3d
{
	class RigidBody;
	class Collider;
	//class Transform;
}

namespace Cookie
{
	namespace Core::Math
	{
		union Vec2;
		union Vec3;
	}

    namespace ECS
    {

		class ComponentTransform;

		class ComponentPhysics
		{
			public:
				::reactphysics3d::RigidBody*				physBody = nullptr;
				std::vector<::reactphysics3d::Collider*>	physColliders;
				::reactphysics3d::Transform					physTransform;
				::reactphysics3d::Transform					oldTransform;

			public:
				ComponentPhysics();
				~ComponentPhysics();

			void AddSphereCollider(float radius, const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void AddCubeCollider(const Core::Math::Vec3& halfExtent, const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void AddCapsuleCollider(const Core::Math::Vec2& capsuleInfo, const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void RemoveCollider(::reactphysics3d::Collider* collider);

			void Update(float factor)noexcept;
			void Set(const ComponentTransform& trs);

			inline void ToDefault()
			{
				for (int i = 0; i < physColliders.size(); i++)
				{
					RemoveCollider(physColliders[i]);
				}

				physBody->setIsActive(false);

				std::vector<reactphysics3d::Collider*>().swap(physColliders);
			}
		};

    }
}

#endif // !__COMPONENT_PHYSICS_HPP__

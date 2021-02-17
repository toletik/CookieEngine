#ifndef __COORDINATOR_HPP__
#define __COORDINATOR_HPP__

#include "EntityHandler.hpp"
#include "ComponentHandler.hpp"
#include "SystemHandler.hpp"
#include <assert.h>

namespace Cookie
{
	namespace ECS
	{

		class Coordinator
		{
		public:
			EntityHandler entityHandler;
			ComponentHandler componentHandler;

			Coordinator() {}
			~Coordinator() {}

			void AddEntity(const int signature, std::string name = std::string("No Name") )
			{
				assert(entityHandler.livingEntities < MAX_ENTITIES && "Too many entities in existence.");

				entityHandler.entities[entityHandler.livingEntities].name = name;
				unsigned int id = entityHandler.entities[entityHandler.livingEntities].id;
				entityHandler.livingEntities++;

				if (CheckSignature(signature, SIGNATURE_TRANSFORM))
					componentHandler.AddComponentTransform(entityHandler.entities[id]);
				if (CheckSignature(signature, SIGNATURE_RIGIDBODY))
					componentHandler.AddComponentRigidBody(entityHandler.entities[id]);
				if (CheckSignature(signature, SIGNATURE_MODEL))
					componentHandler.AddComponentModel(entityHandler.entities[id]);
			}
			void RemoveEntity(Entity& entity)
			{
				assert(entityHandler.livingEntities > 0 && "No Entity to remove");

				//Reset Components
				if (CheckSignature(entity.signature, SIGNATURE_TRANSFORM))
					componentHandler.GetComponentTransform(entity.id).ToDefault();
				if (CheckSignature(entity.signature, SIGNATURE_RIGIDBODY))
					componentHandler.GetComponentRigidBody(entity.id).ToDefault();
				if (CheckSignature(entity.signature, SIGNATURE_MODEL))
					componentHandler.GetComponentModel(entity.id).ToDefault();

				//Reset Entity
				entity.signature = 0;
				entity.name = "No Name";
				entity.needToBeRemoved = false;

				for (unsigned int i = 0; i < entity.children.size(); ++i)
					componentHandler.GetComponentTransform(entity.children[i]).globalTRS = Core::Math::Mat4::Identity();

				//Switch the removed one with the last alive
				entityHandler.livingEntities--;
				entity.Swap(entityHandler.entities[entityHandler.livingEntities]);
			}


			bool CheckSignature(const int entitySignature, const int signature) const { return (entitySignature & signature) == signature;	}
			void ApplySystemDisplayId()
			{
				for (int i = 0; i < entityHandler.livingEntities; ++i)
					std::cout << entityHandler.entities[i].id << std::endl;
			}
			void ApplySystemGravity()
			{
				for (int i = 0; i < entityHandler.livingEntities; ++i)
					if (CheckSignature(entityHandler.entities[i].signature, SIGNATURE_RIGIDBODY))
						System::SystemGravity(componentHandler.GetComponentRigidBody(entityHandler.entities[i].id));
			}
			void ApplySystemVelocity() 
			{
				for (int i = 0; i < entityHandler.livingEntities; ++i)
					if (CheckSignature(entityHandler.entities[i].signature, SIGNATURE_TRANSFORM + SIGNATURE_RIGIDBODY))
						System::SystemVelocity(componentHandler.GetComponentTransform(entityHandler.entities[i].id),
											   componentHandler.GetComponentRigidBody(entityHandler.entities[i].id));
			}


		};

	}
}

#endif
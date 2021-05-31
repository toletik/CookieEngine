#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <json.hpp>

using json = nlohmann::json;

namespace Cookie
{
	class Game;

	namespace ECS
	{
		class Coordinator;
		class ComponentPhysics;
		class ComponentHandler;
		class ComponentGameplay;
		class EntityHandler;
	}

	namespace reactphysics3d
	{
		class PhysicsCommon;
	}

	namespace Core::Math
	{
		union Vec4;
	}

	namespace Resources
	{
		class ResourcesManager;
		class Scene;
		class Prefab;
		class Texture;
		class Sound;
		namespace Particles
		{
			class ParticlesSystem;
		}
	}

	namespace Resources
	{
		namespace Serialization
		{
			namespace Save
			{
				void ToJson(json& js, const Cookie::ECS::EntityHandler& entity);
				void ToJson(json& js, const Cookie::ECS::EntityHandler& entity, Cookie::ECS::ComponentHandler& component, 
					Cookie::Resources::ResourcesManager& resourcesManager);

				void SaveScene(Cookie::Resources::Scene& actScene, Cookie::Resources::ResourcesManager& resourcesManager);
				void SavePrefab(const Prefab* const & prefab);
				void SavePhysic(json& js, Cookie::ECS::ComponentPhysics& physics);

				void SaveAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager);
				void SaveTexture(std::string& name, Cookie::Core::Math::Vec4& color);
				void SaveVolumAndModeMusic(std::string key);
				void SaveVolumAndModeMusic(Sound* const & sound);
				void SaveParticles(Cookie::Resources::Particles::ParticlesSystem& particles);
			}

			namespace Load
			{
				void FromJson(json& js, Cookie::ECS::EntityHandler& entity);
				void FromJson(json& js, const Cookie::ECS::EntityHandler& entity, 
					Cookie::ECS::ComponentHandler& component, Cookie::Resources::ResourcesManager& resourcesManager);

				std::shared_ptr<Scene> LoadScene(const char* filepath, Game& game);
				void LoadAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadAllTextures(Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadPhysic(json& physic, Cookie::ECS::ComponentPhysics& physics);
				void LoadGameplay(json& game, Cookie::ECS::ComponentGameplay& gameplay, Cookie::Resources::ResourcesManager& resourcesManager);
				void LoadVolumAndModeMusic(std::string path, std::string key);
				void LoadParticles(Cookie::Resources::ResourcesManager& resourcesManager);
			}
		}
	}
}


#endif // !__SERIALIZATION_HPP__

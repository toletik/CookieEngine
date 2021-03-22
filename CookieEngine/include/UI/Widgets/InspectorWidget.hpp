#ifndef	__INSPECTOR_W_HPP__
#define __INSPECTOR_W_HPP__

#include "UIwidgetBases.hpp"

namespace Cookie::Resources  { class ResourcesManager; }
namespace Cookie::ECS		 { class Coordinator; class Entity; }
namespace Cookie::Editor	 { class Scene; }
namespace Cookie::Core::Math { union Vec2; }


namespace Cookie::UIwidget
{
	class Inspector final : public WItemBase
	{
		Cookie::Resources::ResourcesManager& resources;
		Cookie::ECS::Coordinator& coordinator;


		Cookie::Editor::Scene*	 selectedScene	= nullptr;
		Cookie::Core::Math::Vec2 sceneTiles;

	public:
		Cookie::ECS::Entity*	selectedEntity	= nullptr;
	
	private:
		void EntityInspection();

		void TransformInterface();
		void RigidBodyInterface();
		void ModelInterface();
		void ScriptInterface();
		void MapInterface();

		void SceneInspection();

	public:
		inline Inspector(Cookie::Resources::ResourcesManager& _resources, Cookie::ECS::Coordinator& _coordinator)
			: WItemBase			("Inspector", false),
			  resources			(_resources),
			  coordinator		(_coordinator)
		{}

		void WindowDisplay() override;


		inline void SelectEntity(Cookie::ECS::Entity* newSelection)
		{ selectedEntity = newSelection; selectedScene = nullptr; }

		void SelectScene(Cookie::Editor::Scene* newSelection);
	};
}

#endif

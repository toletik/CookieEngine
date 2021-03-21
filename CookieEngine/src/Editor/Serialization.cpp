#include <iostream>
#include <fstream>
#include <iomanip>
#include "Scene.hpp"
#include "Serialization.hpp"
#include <bitset>
#include <filesystem>

using namespace Cookie::Editor;
using namespace Cookie::Editor::Serialization;

 void Cookie::Editor::Serialization::Save::ToJson(json& js, const Cookie::ECS::EntityHandler& entity)
{
	for (int i = 0; i < entity.livingEntities; i++)
		js["EntityHandler"] += json{ { "entity", { { "id", entity.entities[i].id }, { "signature", entity.entities[i].signature }, { "name", entity.entities[i].name } } } };
}

 void Cookie::Editor::Serialization::Save::ToJson(json& js, const Cookie::ECS::EntityHandler& entity, Cookie::ECS::ComponentHandler component)
 {
	 for (int i = 0; i < entity.livingEntities; i++)
	 {
		 if (entity.entities[i].signature & SIGNATURE_TRANSFORM)
		 {
			 Cookie::ECS::ComponentTransform transform = component.GetComponentTransform(entity.entities[i].id);
			 js["ComponentHandler"]["Transform"] += json{ { "localTRS", { { "translate", transform.localTRS.pos.e }, { "rotation", transform.localTRS.rot.e }, { "scale", transform.localTRS.scale.e } } } };
		 }
		 if (entity.entities[i].signature & SIGNATURE_MODEL)
		 {
			 Cookie::ECS::ComponentModel model = component.GetComponentModel(entity.entities[i].id);
			 js["ComponentHandler"]["Model"] += json{ { "model", model.mesh.get()->name }, { "shader", "default" }, { "texture", model.texture.get()->name } };
		 }
	 }
 }


 void Cookie::Editor::Serialization::Save::SaveScene(const Cookie::Editor::Scene& actScene)
{
	std::ofstream file(actScene.filepath);

	json js;

	//Name Type
	{
		js["Name"] = actScene.name;
		js["Type"] = "map";
	}

	//Size tiles
	{
		js["Tiles"] = json{ { "widthTile", actScene.tiles.widthTile }, { "depthTile", actScene.tiles.depthTile },
							{ "widthTileProp", actScene.tiles.widthTileProp }, { "depthTileProp", actScene.tiles.depthTileProp } };
	}

	//Entities
	{
		Cookie::Editor::Serialization::Save::ToJson(js, actScene.entityHandler);
	}

	//Components
	{
		Cookie::Editor::Serialization::Save::ToJson(js, actScene.entityHandler, actScene.componentHandler);
	}

	file << std::setw(4) << js << std::endl;
}

 //------------------------------------------------------------------------------------------------------------------

 void Cookie::Editor::Serialization::Load::FromJson(json& js, Cookie::ECS::EntityHandler& entity)
 {
	 entity.livingEntities = js["EntityHandler"].size();
	 
	 for (int i = 0; i < entity.livingEntities; i++)
		 entity.entities[i] = (Cookie::ECS::Entity(js["EntityHandler"][i].at("entity").at("id").get<int>(), js["EntityHandler"][i].at("entity").at("signature").get<int>(), js["EntityHandler"][i].at("entity").at("name").get<std::string>()));
 }

 void Cookie::Editor::Serialization::Load::FromJson(json& js, const Cookie::ECS::EntityHandler& entity, Cookie::ECS::ComponentHandler& component, Cookie::Resources::ResourcesManager resourcesManager)
 {
	 for (int i = 0; i < entity.livingEntities; i++)
	 {
		 if (entity.entities[i].signature & SIGNATURE_TRANSFORM)
		 {
			 Cookie::ECS::ComponentTransform transform;
			 js["ComponentHandler"]["Transform"][i].at("localTRS").at("translate").get_to(transform.localTRS.pos.e);
			 js["ComponentHandler"]["Transform"][i].at("localTRS").at("rotation").get_to(transform.localTRS.rot.e);
			 js["ComponentHandler"]["Transform"][i].at("localTRS").at("scale").get_to(transform.localTRS.scale.e);
			 component.componentTransforms[entity.entities[i].id].localTRS = transform.localTRS;
		 }
		 if (entity.entities[i].signature & SIGNATURE_MODEL)
		 {
			 std::string test;
			 component.componentModels[entity.entities[i].id].mesh = resourcesManager.GetMesh(js["ComponentHandler"]["Model"][i].at("model").get<std::string>());
			 component.componentModels[entity.entities[i].id].texture = resourcesManager.GetTexture(js["ComponentHandler"]["Model"][i].at("texture").get<std::string>());
			 component.componentModels[entity.entities[i].id].shader = resourcesManager.GetDefaultShader();//resourcesManager.GetMesh(js["ComponentHandler"]["Model"][i].at("shader").get<std::string>());
		 }
	 }
 }

 void Cookie::Editor::Serialization::Load::LoadScene(const char* filepath, Cookie::Editor::Scene& newScene, Cookie::Resources::ResourcesManager resourcesManager)
 {
	 std::ifstream file(filepath);

	 if (!file.is_open())
	 {
		 std::cout << "DON'T FIND THE FILE\n";
		 return;
	 }

	 json js;
	 file >> js;
	 
	 //name
	 {
		  js["Name"].get_to(newScene.name);
	 }

	 //tiles
	 {
		 //----------------------CHECK--------------------------------------
		 js["Tiles"].at("widthTile").get_to(newScene.tiles.widthTile);
		 js["Tiles"].at("depthTile").get_to(newScene.tiles.depthTile);
		 js["Tiles"].at("widthTileProp").get_to(newScene.tiles.widthTileProp);
		 js["Tiles"].at("depthTileProp").get_to(newScene.tiles.depthTileProp);
		 newScene.ChangeNumberOfTiles(newScene.tiles.widthTile, newScene.tiles.depthTile);
		 newScene.tiles.ReInitAllTiles();
	 }
	 
	 //entities
	 {
		 Cookie::Editor::Serialization::Load::FromJson(js, newScene.entityHandler);
	 }

	 //component
	 {
		 Cookie::Editor::Serialization::Load::FromJson(js, newScene.entityHandler, newScene.componentHandler, resourcesManager);
	 }

	 newScene.filepath = filepath;
 }
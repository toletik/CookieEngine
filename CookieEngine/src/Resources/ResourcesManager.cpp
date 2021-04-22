#include <d3d11.h>
#include <algorithm>

#include "Resources/Mesh.hpp"
#include "Render/Renderer.hpp"
#include "Resources/Loader.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Core/Primitives.hpp"
#include "Resources/Prefab.hpp"

#include "Resources/Shader/TextureShader.hpp"
#include "Resources/Shader/SkyBoxShader.hpp"
#include "Resources/Shader/BlinnPhongShader.hpp"

#include <memory>

using namespace Cookie::Resources;

ResourcesManager::ResourcesManager()
{
	scripts["test.lua"] = std::make_shared<Script>("Assets\\scripts\\test.lua");
	scripts["test2.lua"] = std::make_shared<Script>("Assets\\scripts\\test2.lua");

	InitPrimitives();
	InitShaders();
}

ResourcesManager::~ResourcesManager()
{

}


void ResourcesManager::SearchForAssets(const fs::path& path, std::vector<std::string>& gltfFiles)
{

	if (fs::exists(path) && fs::is_directory(path))
	{
		for (const fs::directory_entry& entry : fs::directory_iterator(path))
		{
			const fs::path& filename = entry.path().c_str();
			if (fs::is_directory(entry.status()))
			{
				SearchForAssets(filename,gltfFiles);
			}
		}

		for (const fs::directory_entry& entry : fs::directory_iterator(path))
		{
			const fs::path& filename = entry.path().c_str();
			if (fs::is_regular_file(entry.status()))
			{
				if (filename.string().find(".gltf") != std::string::npos)
					gltfFiles.push_back(filename.string());
				else if (filename.string().find(".png") != std::string::npos)
					gltfFiles.push_back(filename.string());
				else if (filename.string().find(".dds") != std::string::npos)
					gltfFiles.push_back(filename.string());
				else if (filename.string().find(".jpg") != std::string::npos)
					gltfFiles.push_back(filename.string());
				else if (filename.string().find(".ico") != std::string::npos)
					gltfFiles.push_back(filename.string());
			}

		}
	}
}

void ResourcesManager::InitShaders()
{
	shaders["Texture_Shader"] = std::make_shared<TextureShader>("Texture_Shader");
	shaders["SkyBox_Shader"] = std::make_shared<SkyBoxShader>("SkyBox_Shader");
	shaders["BlinnPhong_Shader"] = std::make_shared<BlinnPhongShader>("BlinnPhong_Shader");
}

void ResourcesManager::InitPrimitives()
{
	meshes["Quad"] = Cookie::Core::Primitives::CreateQuad();
	meshes["Triangle"] = Cookie::Core::Primitives::CreateTriangle();
	meshes["Cube"] = Cookie::Core::Primitives::CreateCube();
}

void ResourcesManager::Load(Render::Renderer& _renderer)
{
	std::vector<std::string> assetsFiles;
	SearchForAssets(std::string("Assets/"), assetsFiles);

	Loader loader;

	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string& iFile = assetsFiles.at(i);
		std::replace(iFile.begin(),iFile.end(),'\\','/');
		//printf("%s\n", gltfFiles.at(i).c_str());
	}
	
	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string iFile = assetsFiles.at(i);
		//printf("%s\n", iFile.c_str());
		if (iFile.find(".gltf") != std::string::npos)
		{
			loader.Load(iFile.c_str(), *this, _renderer);
			assetsFiles.erase(assetsFiles.begin() + i);
			i--;
		}
		//printf("%s\n", gltfFiles.at(i).c_str());
	}
	for (unsigned int i = 0; i < assetsFiles.size(); i++)
	{
		std::string iFile = assetsFiles.at(i);
		if (textures.find(iFile) == textures.end())
		{
			textures[iFile] = std::make_shared<Texture>(iFile);
		}
		//printf("%s\n", gltfFiles.at(i).c_str());
	}
}

void ResourcesManager::CreateNewPrefabs(ECS::Entity& entity, ECS::ComponentHandler& component)
{
	Prefab newPrefab;

	newPrefab.name = entity.name;
	newPrefab.nameMesh = component.componentModels[entity.id].mesh->name;
	newPrefab.nameShader = component.componentModels[entity.id].shader->name;
	newPrefab.nameTexture = component.componentModels[entity.id].texture->name;
	newPrefab.signature = entity.id;
	newPrefab.rotation = component.componentTransforms[entity.id].rot;
	newPrefab.scale = component.componentTransforms[entity.id].scale;
	newPrefab.filepath = "Assets/Prefabs/" + entity.name + ".PAsset";
	newPrefab.rigidBody = component.componentPhysics[entity.id].physBody;
	newPrefab.colliders = component.componentPhysics[entity.id].physColliders;

	entity.namePrefab = entity.name;

	prefabs[newPrefab.name] = std::make_shared<Prefab>(newPrefab);
}


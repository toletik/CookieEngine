#include "Editor.hpp" 
#include "UIcore.hpp"
#include "UIeditor_AllIn.hpp"
#include "Serialization.hpp"
#include "Primitives.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Resources/Scene.hpp"
#include "CGPMove.hpp"
#include "CGPProducer.hpp"
#include "SoundManager.hpp"

using namespace Cookie;
using namespace Cookie::Core;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Gameplay;
using namespace Cookie::Resources;
using namespace rp3d;

Editor::Editor()
    : editorFBO{game.renderer.window.width,game.renderer.window.height}
{
    game.resources.Load(game.renderer);
    cam.SetProj(60.f, game.renderer.viewport.Width, game.renderer.viewport.Height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    cam.pos = { 0.f , 20.0f,30.0f };
    cam.rot = { Core::Math::ToRadians(30.0f) ,0.0f,0.0f };
    cam.ResetPreviousMousePos();
    cam.Update();
    cam.Deactivate();
    game.scene->InitCoordinator(game.coordinator);

    //Load all Textures we have create in texture editor
    Resources::Serialization::Load::LoadAllTextures(game.resources);

    //Load all prefabs in folder Prefabs
    Resources::Serialization::Load::LoadAllPrefabs(game.resources);

    Resources::SoundManager::InitSystem();
    Resources::SoundManager::LoadAllMusic(game.resources);
    Serialization::Load::LoadAllParticles(game.resources);
    game.particlesHandler.particlesPrefab = &game.resources.particles;

    //Load default Scene
    Resources::Serialization::Load::LoadScene("Assets/Save/Default.CAsset", game);
    game.SetScene();

    editorUI.AddItem(new UIwidget::SaveButton(game.scene, game.resources), 0);
    editorUI.AddWItem(new UIwidget::ExitPannel(game.renderer.window.window), 0);
    
    editorUI.AddWItem(new UIwidget::TextureEditor(game.resources), 1);
    editorUI.AddWItem(new UIwidget::AIBehaviorEditor(game.resources), 1);
    editorUI.AddWItem(new UIwidget::GameUIeditor(game), 1);
    editorUI.AddWItem(new UIwidget::SoundOrchestrator(game.resources), 1);
    
    editorUI.AddWItem(new UIwidget::Inspector(selectedEntity, game.resources, game.coordinator), 2);
    editorUI.AddWItem(new UIwidget::Hierarchy(game.resources, game.scene, game.coordinator, selectedEntity), 2);
    editorUI.AddWItem(new UIwidget::WorldSettingsWidget(game.scene, game.scene.get()->lights, game.renderer.skyBox, game.resources), 2);
    editorUI.AddWItem(new UIwidget::Console(CDebug, game.resources), 2);
    editorUI.AddWItem(new UIwidget::FileExplorer(game), 2);

    editorUI.AddWItem(new UIwidget::DemoWindow, 3);


    UIwidget::Toolbar* toolbar = new UIwidget::Toolbar(game.resources, isPlaying);
    editorUI.AddWindow(new UIwidget::Viewport(toolbar, game.renderer.window.window, editorFBO, &cam, game.coordinator, selectedEntity));
    editorUI.AddWindow(new UIwidget::GamePort(isPlaying, game));

    InitEditComp();

    Physics::PhysicsHandle::editWorld->setIsDebugRenderingEnabled(false);
}

Editor::~Editor()
{
    //Save all prefabs in folder Prefabs
   // Resources::Serialization::Save::SaveAllPrefabs(game.resources);
    Resources::SoundManager::Release();
}


void Editor::InitEditComp()
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        int j = game.coordinator.entityHandler->entities[i].id;
        editingComponent[j].InitComponent(game.coordinator.componentHandler->GetComponentTransform(j));
    }
}

void Editor::ModifyEditComp()
{
    int max = livingEntitiesNb > game.coordinator.entityHandler->livingEntities ? livingEntitiesNb : game.coordinator.entityHandler->livingEntities;
    for (int i = 0; i < max; i++)
    {
        int j = game.coordinator.entityHandler->entities[i].id;
        ComponentEditor& jComponent = editingComponent[j];
        jComponent.editTrs = &game.coordinator.componentHandler->GetComponentTransform(j);
        bool hasMesh = (game.coordinator.entityHandler->entities[i].signature & C_SIGNATURE::MODEL) && game.coordinator.componentHandler->GetComponentModel(j).mesh != nullptr;

        /* if it has mesh and alive, change collider */
        if (hasMesh)
        {
            jComponent.AABBMin = game.coordinator.componentHandler->GetComponentModel(j).mesh->AABBMin;
            jComponent.AABBMax = game.coordinator.componentHandler->GetComponentModel(j).mesh->AABBMax;
            jComponent.MakeCollider();
        }
        
        /* if entity deleted, remove collider */
        if (!hasMesh && jComponent.collider)
        {
            if (jComponent.body)
            {
                if (jComponent.collider)
                {
                    jComponent.body->removeCollider(editingComponent[j].collider);
                    jComponent.collider = nullptr;
                }
            }
        }

        jComponent.Update();
    }
}


void Editor::Loop()
{
    //Cookie::Resources::SoundManager::PlayMusic("Music.mp3");
    Physics::PhysicsHandle physHandle;

    {
       // game.scene->map.model.albedo = game.resources.textures2D["Assets/Floor_DefaultMaterial_BaseColor.png"].get();
    }

    //for (int i = 0; i < MAX_ENTITIES; i++)
    //{
    //    game.coordinator.AddEntity(MODEL | TRANSFORM, std::to_string(i));
    //    game.coordinator.componentHandler->GetComponentModel(game.coordinator.entityHandler->entities[i].id).mesh = game.resources.meshes["Sphere"].get();
    //    game.coordinator.componentHandler->GetComponentModel(game.coordinator.entityHandler->entities[i].id).albedo = game.resources.textures["Green"].get();
    //    game.coordinator.componentHandler->GetComponentModel(game.coordinator.entityHandler->entities[i].id).metallicRoughness = game.resources.textures["Blue"].get();
    //    game.coordinator.componentHandler->GetComponentTransform(game.coordinator.entityHandler->entities[i].id).pos = { -50.0f + ((float)std::rand() / (float)RAND_MAX) * 100.0f, (float)std::rand() / (float)RAND_MAX * 10.0f + 1.0f, -50.0f + ((float)std::rand() / (float)RAND_MAX) * 100.0f };
    //    game.coordinator.componentHandler->GetComponentTransform(game.coordinator.entityHandler->entities[i].id).trsHasChanged = true;
    //}

    while (!glfwWindowShouldClose(game.renderer.window.window))
    {
        // Present frame
        if (!ImGui::GetIO().KeysDownDuration[GLFW_KEY_L])
            Cookie::Resources::Particles::ParticlesHandler::CreateParticlesWithPrefab(Vec3(-5, 15, 5), game.resources.particles["Bomb"].get(), Vec3(10, 0, 25));
        
        if (isPlaying)
        {
            game.Update();
        }
        else
        {
            glfwPollEvents();
            TryResizeWindow();
            game.coordinator.ApplyComputeTrs();

            cam.Update();
           
            if (currentScene != game.scene.get() || game.coordinator.entityHandler->livingEntities != livingEntitiesNb)
            {
                selectedEntity = {};
                selectedEntity.componentHandler = game.coordinator.componentHandler;
                ModifyEditComp();
                currentScene = game.scene.get();
                livingEntitiesNb = game.coordinator.entityHandler->livingEntities;
            }

            if (!ImGui::GetIO().MouseDownDuration[0])
            {
                Core::Math::Vec3 fwdRay = cam.pos + cam.MouseToWorldDirClamp() * cam.camFar;
                rp3d::Ray ray({ cam.pos.x,cam.pos.y,cam.pos.z }, { fwdRay.x,fwdRay.y,fwdRay.z });
                physHandle.editWorld->raycast(ray, this);
            }

            if (selectedEntity.toChangeEntityIndex >= 0)
            {
                PopulateFocusedEntity();
            }
            if (selectedEntity.focusedEntity && (selectedEntity.focusedEntity->signature & C_SIGNATURE::PHYSICS))
            {
                selectedEntity.componentHandler->GetComponentPhysics(selectedEntity.focusedEntity->id).Set(selectedEntity.componentHandler->GetComponentTransform(selectedEntity.focusedEntity->id));
            }

            //Update for 3D Music
            Cookie::Resources::SoundManager::UpdateFMODFor3DMusic(cam);
        }

           
        //game.scene->physSim.Update();
        //game.coordinator.ApplySystemPhysics(game.scene->physSim.factor);
        


        //game.coordinator.armyHandler->Debug();
        //game.coordinator.entityHandler->Debug();

        //Draw
        game.renderer.Clear();
        game.renderer.ClearFrameBuffer(editorFBO);
        game.renderer.Draw(&cam,editorFBO);
		game.particlesHandler.Draw(cam);

        dbgRenderer.Draw(cam.GetViewProj());

        game.renderer.SetBackBuffer();
        UIcore::BeginFrame();
        editorUI.UpdateUI();
        UIcore::EndFrame();
        game.renderer.Render();
    }

    if (game.scene)
        game.scene->skyBox = game.renderer.skyBox.texture;

    Particles::ParticlesHandler::shader.Destroy();
}

void Editor::TryResizeWindow()
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(game.renderer.window.window, &width, &height);

    if (width <= 0 || height <= 0)
        return;

    if (game.renderer.window.width != width || game.renderer.window.height != height)
    {
        //Core::DebugMessageHandler::Summon().Log((std::to_string(width) + ' ' + std::to_string(height)).c_str());
        //printf("%d, %d\n", width, height);
        game.renderer.window.width = width;
        game.renderer.window.height = height;

        game.renderer.ResizeBuffer(width, height);
        game.frameBuffer.Resize(width, height);
        editorFBO.Resize(width, height);

        //scene->camera->SetProj(Core::Math::ToRadians(60.f), width, height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    }
}

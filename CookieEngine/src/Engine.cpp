#include <reactphysics3d/reactphysics3d.h>
#include "Engine.hpp"
#include "Serialization.hpp"
#include "Time.hpp"
#include "Debug.hpp"
#include "Core/Math/Calc.hpp"
#include "ImGui/imgui.h"
#include <vector>


//Temp
#include "GLFW/glfw3.h"

using namespace Cookie;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Resources;

// Class WorldRaycastCallback
class MyCallbackClass : public reactphysics3d::RaycastCallback {
public:
    reactphysics3d::CollisionBody* body = nullptr;
    reactphysics3d::Vector3 pt = { 0.0f,0.0f,0.0f };

    virtual float notifyRaycastHit(const reactphysics3d::RaycastInfo& info)
    {
        // Display the world hit point coordinates
        std::cout << "Hit point : " <<
            info.worldPoint.x <<
            info.worldPoint.y <<
            info.worldPoint.z <<
            std::endl;

        body = info.body;
        pt = info.worldPoint;
        // Return a fraction of 1.0 to gather all hits
        return float(0.0);
    }
};


Engine::Engine() :
    window{}, renderer{ window }, ui{ window.window, renderer },physCom{std::make_shared<reactphysics3d::PhysicsCommon>()},physSim{ physCom }
{
    resources.Load(renderer);
    renderer.AddFrameBuffer(resources);
    camera = std::make_shared<Render::GameCam>();
    camera->SetProj(Core::Math::ToRadians(60.f), renderer.state.viewport.Width,renderer.state.viewport.Height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    camera->pos = { 0.0f,20.0f,30.0f };
    camera->rot = { Core::Math::ToRadians(30.f) ,0.0f,0.0f};
    camera->ResetPreviousMousePos();
    camera->Update();
    camera->Deactivate();
    scene = Editor::Scene(resources, coordinator);
    scene.InitCoordinator(coordinator);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_W);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_S);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_A);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_D);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_SPACE);
    ImGui::GetIO().AddInputCharacter(GLFW_KEY_LEFT_CONTROL);
    resources.AddTexture(std::make_shared<Resources::Texture>(renderer, "Pink", Core::Math::Vec4(1.0f,0.5f,0.5f,1.0f)));
    resources.AddTexture(std::make_shared<Resources::Texture>(renderer, "Assets/Floor_DefaultMaterial_BaseColor.png"));
    //coordinator.componentHandler->componentModels[0].texture = resources.GetTexture("Assets/Floor_DefaultMaterial_BaseColor.png");
}

Engine::~Engine()
{
}

/*
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << key << std::endl;

    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        std::cout << "Space Key Pressed" << std::endl;
    }
}*/

void Engine::Run()
{
    ////Map in future Classes
    //std::unordered_map<int, std::function<void()> > UnitInputs;
    //UnitInputs['A'] = [] { std::cout << "Unit Shortcut 1\n"; };
    //UnitInputs['Z'] = [] { std::cout << "Unit Shortcut 2\n"; };
    //UnitInputs['E'] = [] { std::cout << "Unit Shortcut 3\n"; };
    //std::unordered_map<int, std::function<void()> > BuildingInputs;
    //BuildingInputs['A'] = [] { std::cout << "Building Shortcut 1\n"; };
    //BuildingInputs['Z'] = [] { std::cout << "Building Shortcut 2\n"; };
    //BuildingInputs['E'] = [] { std::cout << "Building Shortcut 3\n"; };

    //input.Set(UnitInputs);


    ui.AddWItem(new UIwidget::ExitPannel(window.window), 0);

    ui.AddWItem(new UIwidget::TextureEditor(renderer, resources), 1);
    
    
    ui.AddWItem(new UIwidget::FileExplorer, 2);
    
    UIwidget::Inspector* insp = new UIwidget::Inspector(resources, coordinator);
    ui.AddWItem(insp, 2);
    ui.AddWItem(new UIwidget::Hierarchy(resources, &scene, coordinator, insp), 2);

    ui.AddWItem(new UIwidget::Console(CDebug, renderer), 2);

    ui.AddWItem(new UIwidget::DemoWindow, 3);

    UIwidget::Toolbar* toolbar = new UIwidget::Toolbar(renderer);
    ui.AddWindow(new UIwidget::Viewport(toolbar, window.window, renderer.GetLastFrameBuffer(), &camera, coordinator, insp->selectedEntity));

    //Load default Scene
    Editor::Serialization::Load::LoadScene("Assets/Save/DefaultDuck.CAsset", scene, resources);

    static bool camClicked = false;

    while (!glfwWindowShouldClose(window.window))
    {
        // Present frame
        CDebug.UpdateTime();

        // Present frame
        glfwPollEvents();
        TryResizeWindow();

        //physSim.Update();
        //coordinator.ApplySystemPhysics(physSim.factor);

        renderer.Clear();

        if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_MIDDLE) && !camClicked)
        {
            Core::Math::Vec3 pos = camera->pos;
            Core::Math::Vec2 window = { { camera->width,camera->height } };
            if (isFlyCam)
            {
                camera = std::make_shared<Render::GameCam>();
                camera->rot = { Core::Math::ToRadians(30.f) ,0.0f,0.0f };
            }
            else
            {
                Core::Math::Vec3 rot = camera->rot;
                camera = std::make_shared<Render::FreeFlyCam>();
                camera->rot = rot;
            }

            camera->pos = pos;
            isFlyCam = !isFlyCam;
            camera->ResetPreviousMousePos();
            camera->SetProj(Core::Math::ToRadians(60.f),window.x,window.y, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
            camera->Update();
            camera->Deactivate();
            camClicked = true;
        }
        else if (!glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_MIDDLE) && camClicked)
        {
            camClicked = false;
        }

        camera->Update();

       //select unit or move selected
       if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_LEFT))
       {       
           Core::Math::Vec3 end = camera->pos + camera->MouseToWorldDir() * camera->camFar;
           reactphysics3d::Ray ray{ {camera->pos.x,camera->pos.y,camera->pos.z},{end.x,end.y,end.z} };
           MyCallbackClass hit;
           physSim.worldSim->raycast(ray, &hit);

           if (hit.body != nullptr)
           {
               for (int i = 0; i < coordinator.componentHandler->componentRigidBodies.size(); i++)
               {
                   ComponentRigidBody iRigidBody = coordinator.componentHandler->GetComponentRigidBody(i);
                   if (iRigidBody.physBody == hit.body)
                   {
                       if (glfwGetKey(window.window, GLFW_KEY_LEFT_CONTROL) && coordinator.selectedEntity)
                       {
                           Core::Math::Vec3 force = camera->MouseToWorldDir() * 10.0;
                           iRigidBody.physBody->applyForceAtWorldPosition({ force.x,force.y,force.z },hit.pt);
                       }
                       else
                       {
                           coordinator.selectedEntity = &coordinator.entityHandler->entities[i];
                           insp->SelectEntity(coordinator.selectedEntity);
                       }
                   }
               }
           }
         
       }

       renderer.Draw(camera->GetViewProj(), coordinator);
       ///TEMP

       if (glfwGetKey(window.window, GLFW_KEY_H) == GLFW_PRESS)
           Editor::Serialization::Save::SaveScene(scene);
       
       if (glfwGetKey(window.window, GLFW_KEY_P) == GLFW_PRESS)
           Editor::Serialization::Load::LoadScene("Assets/Save/Map2.CAsset", scene, resources);           

       if (glfwGetKey(window.window, GLFW_KEY_L) == GLFW_PRESS)
           Editor::Serialization::Load::LoadScene("Assets/Save/Map1.CAsset", scene, resources);
           

       ///

        coordinator.ApplyScriptUpdate();
        coordinator.ApplyDraw(renderer.remote, camera->GetViewProj());
        renderer.SetBackBuffer();
        //frameBuffer.Draw(renderer.remote);
        
        ui.UpdateUI();

        renderer.Render();
    }

    //for (int i = 0; i < coordinator.componentHandler->componentRigidBodies.size(); i++)
    //{
    //    ComponentRigidBody iRigidBody = coordinator.componentHandler->GetComponentRigidBody(i);
    //    if (iRigidBody.physBody != nullptr)
    //    {
    //        physSim.worldSim->destroyRigidBody(iRigidBody.physBody);
    //        iRigidBody.physBody = nullptr;
    //    }
    //}
    //
}


void Engine::TryResizeWindow()
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(window.window, &width, &height);

    if (width <= 0 || height <= 0)
        return;

    if (window.width != width || window.height != height)
    {
        Core::DebugMessageHandler::Summon().Log((std::to_string(width) + ' '+ std::to_string(height)).c_str());
        printf("%d, %d\n", width, height);
        window.width = width;
        window.height = height;

        renderer.ResizeBuffer(width,height);
        camera->SetProj(Core::Math::ToRadians(60.f),width,height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    }
}

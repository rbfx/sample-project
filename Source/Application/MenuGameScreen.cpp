//
// Copyright (c) 2022-2023 the rbfx project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "MenuGameScreen.h"

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Plugins/PluginManager.h>
#include <Urho3D/RmlUI/RmlUI.h>

#include "SampleGameScreen.h"
#include "SampleProject.h"

MainMenuWindow::MainMenuWindow(Context* context)
    : RmlUIComponent(context)
{
    SetResource("UI/MainMenu.rml");
}

void MainMenuWindow::RegisterObject(Context* context)
{
    context->RegisterFactory<MainMenuWindow>();
}

void MainMenuWindow::SetGame(SampleGameScreen* game)
{
    isGamePlayed_ = GetApp()->IsGamePlayed();
    DirtyAllVariables();
}

void MainMenuWindow::OnDataModelInitialized()
{
    Rml::DataModelConstructor* constructor = GetDataModelConstructor();
    URHO3D_ASSERT(constructor);

    constructor->RegisterArray<StringVector>();

    constructor->Bind("is_game_played", &isGamePlayed_);

    constructor->BindEventCallback("on_continue", WrapCallback(&MainMenuWindow::OnContinue));
    constructor->BindEventCallback("on_new_game", WrapCallback(&MainMenuWindow::OnNewGame));
    constructor->BindEventCallback("on_settings", WrapCallback(&MainMenuWindow::OnSettings));
    constructor->BindEventCallback("on_exit", WrapCallback(&MainMenuWindow::OnExit));
}

MenuGameScreen* MainMenuWindow::GetApplicationState() const
{
    auto stateManager = GetSubsystem<StateManager>();
    return dynamic_cast<MenuGameScreen*>(stateManager->GetState());
}

void MainMenuWindow::Update(float timeStep)
{
    BaseClassName::Update(timeStep);
}

void MainMenuWindow::OnContinue()
{
    SampleProject* app = GetApp();
    app->ContinueGame();
}

void MainMenuWindow::OnNewGame()
{
    SampleProject* app = GetApp();
    app->NewGame();
}

SampleProject* MainMenuWindow::GetApp() const
{
	const auto* pluginManager = context_->GetSubsystem<PluginManager>();
    return dynamic_cast<SampleProject*>(pluginManager->GetMainPlugin());
}

void MainMenuWindow::OnSettings()
{
}

void MainMenuWindow::OnExit()
{
    auto* engine = context_->GetSubsystem<Engine>();
    engine->Exit();
}

MenuGameScreen::MenuGameScreen(Context* context)
    : ApplicationState(context)
{
    auto* ui = context_->GetSubsystem<RmlUI>();

    // Setup mouse.
    SetMouseMode(MM_RELATIVE);
    SetMouseVisible(false);

    auto* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    auto* zone = scene_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogColor(Color::GRAY);
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    // Create skybox.
    Node* skyboxNode = scene_->CreateChild("Sky");
    auto skybox = skyboxNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/DefaultSkybox.xml"));

    // Create a camera.
    cameraNode_ = scene_->CreateChild("Camera");
    auto camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFov(90.0f);

    // Set an initial position for the camera scene node.
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -2.0f));

    // Create a window rendered into backbuffer.
    window_ = scene_->CreateComponent<MainMenuWindow>();
    window_->SetEnabled(false);

    // Set up a viewport so 3D scene can be visible.
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera));
    SetViewport(0, viewport);

    // Disable updates until state is activated.
    scene_->SetUpdateEnabled(false);
}

MenuGameScreen::~MenuGameScreen()
{
    // Release scene.
    scene_ = nullptr;
}

void MenuGameScreen::Activate(StringVariantMap& bundle)
{
    BaseClassName::Activate(bundle);
    window_->SetEnabled(true);
    window_->SetGame(dynamic_cast<SampleGameScreen*>(bundle["Game"].GetPtr()));
    SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(MenuGameScreen, HandleKeyDown));

    // Enable scene updates.
    scene_->SetUpdateEnabled(true);
}

void MenuGameScreen::Update(float timeStep)
{
}

void MenuGameScreen::Deactivate()
{
    BaseClassName::Deactivate();

    // Disable updates until state is activated.
    scene_->SetUpdateEnabled(false);

    UnsubscribeFromEvent(Urho3D::E_KEYDOWN);
    window_->SetEnabled(false);
}

void MenuGameScreen::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    const auto key = static_cast<Key>(eventData[KeyDown::P_KEY].GetUInt());
    if (key == KEY_ESCAPE)
    {
        if (window_->HasGame())
            window_->OnContinue();
        else
            window_->OnExit();
    }
}

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

#include "SampleGameScreen.h"

#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/VirtualFileSystem.h>
#include <Urho3D/Input/FreeFlyController.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/SplashScreen.h>
#include <Urho3D/Plugins/PluginManager.h>

#include "MenuGameScreen.h"
#include "SampleProject.h"

PlayerController::PlayerController(Context* context)
    : LogicComponent(context)
{
}

void PlayerController::RegisterObject(Context* context)
{
    context->RegisterFactory<ClassName>(Category_SampleProject);
}

void PlayerController::FixedUpdate(float timeStep)
{
    const float moveSpeed = 5.0f;

    auto input = GetSubsystem<Input>();
    auto camera = node_->FindComponent<Camera>();
    auto kinematicController = node_->GetComponent<KinematicCharacterController>();

    if (!camera || !kinematicController)
        return;

    // Get local move direction.
    Vector3 localDirection;
    if (input->GetKeyDown(KEY_W))
        localDirection += Vector3::FORWARD;
    if (input->GetKeyDown(KEY_S))
        localDirection += Vector3::BACK;
    if (input->GetKeyDown(KEY_A))
        localDirection += Vector3::LEFT;
    if (input->GetKeyDown(KEY_D))
        localDirection += Vector3::RIGHT;
    localDirection.Normalize();

    // Get world move direction.
    const float yawAngle = camera->GetNode()->GetWorldRotation().YawAngle();
    const Vector3 worldDirection = Quaternion{ yawAngle, Vector3::UP } *localDirection;
    kinematicController->SetWalkIncrement(worldDirection * moveSpeed * timeStep);

    // Jump without checking for ground.
    jumpCooldown_ -= timeStep;
    if (input->GetKeyDown(KEY_SPACE))
    {
        if (jumpCooldown_ <= 0.0f)
        {
            kinematicController->Jump();
            jumpCooldown_ = jumpInterval_;
        }
    }
}

void GameSaveData::SerializeInBlock(Archive& archive)
{
    SerializeValue(archive, "actorPosition", actorPosition_);
    SerializeValue(archive, "actorRotation", actorRotation_);
}

SampleGameScreen::SampleGameScreen(Context* context)
    : ApplicationState(context)
{
    // Setup mouse.
    SetMouseMode(MM_RELATIVE);
    SetMouseVisible(false);
}

SampleGameScreen::~SampleGameScreen()
{
    // Release scene.
    scene_ = nullptr;
    actorNode_ = nullptr;
    cameraNode_ = nullptr;
}

void SampleGameScreen::RegisterObject(Context* context)
{
    context->RegisterFactory<SampleGameScreen>();
}

void SampleGameScreen::InitScene()
{
    if (scene_)
        return;

    // Load scene.
    scene_ = MakeShared<Scene>(context_);
    scene_->LoadFile("Scenes/Scene.xml");

    // Find camera.
    actorNode_ = scene_->FindChild("Actor");
    Camera* camera = actorNode_ ? actorNode_->FindComponent<Camera>() : nullptr;
    if (!camera)
        return;
    cameraNode_ = camera->GetNode();

    // Create player component now to prevent it from moving in Editor.
    auto player = actorNode_->CreateComponent<PlayerController>();

    // Create free-fly controller with zero speed to control motion with physics.
    const auto controller = cameraNode_->CreateComponent<FreeFlyController>();
    controller->SetSpeed(0.0f);
    controller->SetAcceleratedSpeed(0.0f);

    // Create viewport.
    const auto viewport = MakeShared<Viewport>(context_, scene_, camera);
    SetViewport(0, viewport);

    LoadGame();
}


void SampleGameScreen::Activate(StringVariantMap& bundle)
{
    BaseClassName::Activate(bundle);

    InitScene();

    SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(SampleGameScreen, HandleKeyDown));

    // Warp camera to current position in Editor, if applicable.
    if (const auto position = bundle[Param_ScenePosition]; !position.IsEmpty())
        actorNode_->SetWorldPosition(position.GetVector3() - cameraNode_->GetPosition());

    if (const auto rotation = bundle[Param_SceneRotation]; !rotation.IsEmpty())
        cameraNode_->SetWorldRotation(rotation.GetQuaternion());

}

void SampleGameScreen::Update(float timeStep)
{
    const float autoSaveInterval = 5.0f;
    autosaveTimer_ += timeStep;
    if (autosaveTimer_ >= autoSaveInterval)
    {
        autosaveTimer_ = 0.0f;
        SaveGame();
    }
}

void SampleGameScreen::Deactivate()
{
    BaseClassName::Deactivate();

    UnsubscribeFromEvent(Urho3D::E_KEYDOWN);
}

SampleProject* SampleGameScreen::GetApp() const
{
    const auto* pluginManager = context_->GetSubsystem<PluginManager>();
    return dynamic_cast<SampleProject*>(pluginManager->GetMainPlugin());
}


void SampleGameScreen::SaveGame()
{
    if (GetPlatform() != PlatformId::Web || !scene_)
        return;

    GameSaveData saveGame;
    saveGame.actorPosition_ = actorNode_->GetWorldPosition();
    saveGame.actorRotation_ = cameraNode_->GetWorldRotation();

    JSONFile jsonFile(context_);
    jsonFile.SaveObject("saveGameData", saveGame);

    // TODO: Get rid of VirtualFileSystem.
    auto vfs = GetSubsystem<VirtualFileSystem>();
    auto file = vfs->OpenFile(FileIdentifier("conf://Saves/actor.json"), FILE_WRITE);
    if (file)
        jsonFile.Save(*file);
}

void SampleGameScreen::LoadGame()
{
    if (GetPlatform() != PlatformId::Web || !scene_)
        return;

    // TODO: Get rid of VirtualFileSystem.
    auto vfs = GetSubsystem<VirtualFileSystem>();
    auto file = vfs->OpenFile(FileIdentifier("conf://Saves/actor.json"), FILE_READ);
    if (!file)
        return;

    JSONFile jsonFile(context_);
    if (!jsonFile.Load(*file))
        return;

    GameSaveData saveGame;
    jsonFile.LoadObject("saveGameData", saveGame);

    actorNode_->SetWorldPosition(saveGame.actorPosition_);
    cameraNode_->SetWorldRotation(saveGame.actorRotation_);
}

void SampleGameScreen::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    const auto key = static_cast<Key>(eventData[KeyDown::P_KEY].GetUInt());
    if (key == KEY_ESCAPE)
    {
        GetApp()->OpenMenu();
    }
}

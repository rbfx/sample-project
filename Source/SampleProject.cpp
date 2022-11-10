//
// Copyright (c) 2017-2020 the rbfx project.
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

#include "SampleProject.h"

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/FreeFlyController.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

URHO3D_DEFINE_PLUGIN_MAIN(SampleProject);

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
    auto camera = node_->GetComponent<Camera>(true);
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
    const Vector3 worldDirection = Quaternion{yawAngle, Vector3::UP} * localDirection;
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

SampleGameScreen::SampleGameScreen(Context* context)
    : ApplicationState(context)
{
}

void SampleGameScreen::Activate(StringVariantMap& bundle)
{
    BaseClassName::Activate(bundle);

    // Setup mouse.
    SetMouseMode(MM_RELATIVE);
    SetMouseVisible(false);

    // Load scene.
    scene_ = MakeShared<Scene>(context_);
    scene_->LoadFile("Scenes/Scene.xml");

    // Find camera.
    Node* actorNode = scene_->FindChild("Actor");
    Camera* camera = actorNode ? actorNode->GetComponent<Camera>(true) : nullptr;
    if (!camera)
        return;
    Node* cameraNode = camera->GetNode();

    // Create player component now to prevent it from moving in Editor.
    auto player = actorNode->CreateComponent<PlayerController>();

    // Create free-fly controller with zero speed to control motion with physics.
    auto controller = cameraNode->CreateComponent<FreeFlyController>();
    controller->SetSpeed(0.0f);
    controller->SetAcceleratedSpeed(0.0f);

    // Create viewport.
    auto viewport = MakeShared<Viewport>(context_, scene_, camera);
    SetViewport(0, viewport);

    // Warp camera to current position in Editor, if applicable.
    if (const auto position = bundle[Param_ScenePosition]; !position.IsEmpty())
        actorNode->SetWorldPosition(position.GetVector3() - cameraNode->GetPosition());

    if (const auto rotation = bundle[Param_SceneRotation]; !rotation.IsEmpty())
        cameraNode->SetWorldRotation(rotation.GetQuaternion());
}

void SampleGameScreen::Deactivate()
{
    BaseClassName::Deactivate();

    // Release scene.
    scene_ = nullptr;
}

SampleProject::SampleProject(Context* context)
    : MainPluginApplication(context)
{
    RegisterObject<PlayerController>();
}

void SampleProject::Load()
{
}

void SampleProject::Unload()
{
}

void SampleProject::Start(bool isMain)
{
    auto engine = GetSubsystem<Engine>();
    auto stateManager = GetSubsystem<StateManager>();

    // TODO: Use this to check whether to show menu or not.
    const bool fromEditor = !engine->GetParameter(Param_SceneName).IsEmpty();

    // Allocate game screen.
    gameScreen_ = MakeShared<SampleGameScreen>(context_);

    // Activate game screen.
    StringVariantMap bundle;
    bundle[Param_ScenePosition] = engine->GetParameter(Param_ScenePosition);
    bundle[Param_SceneRotation] = engine->GetParameter(Param_SceneRotation);
    stateManager->EnqueueState(gameScreen_, bundle);
    stateManager->Update(0.001f);
}

void SampleProject::Stop()
{
}

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
    auto cache = GetSubsystem<ResourceCache>();
    auto engine = GetSubsystem<Engine>();
    auto renderer = GetSubsystem<Renderer>();

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
    viewport_ = MakeShared<Viewport>(context_, scene_, camera);
    renderer->SetNumViewports(1);
    renderer->SetViewport(0, viewport_);

    // Warp camera to current position in Editor, if applicable.
    if (const auto position = engine->GetParameter(Param_ScenePosition); !position.IsEmpty())
        actorNode->SetWorldPosition(position.GetVector3() - cameraNode->GetPosition());

    if (const auto rotation = engine->GetParameter(Param_SceneRotation); !rotation.IsEmpty())
        cameraNode->SetWorldRotation(rotation.GetQuaternion());
}

void SampleProject::Stop()
{
    // Unset viewport.
    auto renderer = GetSubsystem<Renderer>();
    renderer->SetNumViewports(0);

    // Release viewport and scene.
    viewport_ = nullptr;
    scene_ = nullptr;
}

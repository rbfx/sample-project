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

#include "SampleProject.h"

#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/VirtualFileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/SplashScreen.h>

URHO3D_DEFINE_PLUGIN_MAIN(SampleProject);

SampleProject::SampleProject(Context* context)
    : MainPluginApplication(context)
{
}

void SampleProject::Load()
{
    RegisterObject<PlayerController>();
    RegisterObject<MainMenuWindow>();
    RegisterObject<SampleGameScreen>();
}

void SampleProject::Unload()
{
    loadingScreen_ = nullptr;
    gameScreen_ = nullptr;
    menuScreen_ = nullptr;
}

void SampleProject::OpenMenu()
{
	const auto stateManager = GetSubsystem<StateManager>();
    stateManager->EnqueueState(menuScreen_);
}

void SampleProject::NewGame()
{
    const auto stateManager = GetSubsystem<StateManager>();
    gameScreen_ = MakeShared<SampleGameScreen>(context_);
    stateManager->EnqueueState(gameScreen_);
}

void SampleProject::ContinueGame()
{
    if (!gameScreen_)
        return;
    const auto stateManager = GetSubsystem<StateManager>();
    stateManager->EnqueueState(gameScreen_);
}

void SampleProject::Start(bool isMain)
{
    auto engine = GetSubsystem<Engine>();
    auto stateManager = GetSubsystem<StateManager>();

    // TODO: Use this to check whether to show menu or not.
    const bool fromEditor = !engine->GetParameter(Param_SceneName).IsEmpty();

    // Allocate main menu screen.
    menuScreen_ = MakeShared<MenuGameScreen>(context_);

    // TODO: Web cursor is broken, disable menu on Web for now.
    if (fromEditor || GetPlatform() == PlatformId::Web)
    {
        // Activate game screen.
        StringVariantMap bundle;
        bundle[Param_ScenePosition] = engine->GetParameter(Param_ScenePosition);
        bundle[Param_SceneRotation] = engine->GetParameter(Param_SceneRotation);
        gameScreen_ = MakeShared<SampleGameScreen>(context_);
        stateManager->EnqueueState(gameScreen_, bundle);
    }
    else
    {
        stateManager->SetFadeInDuration(0.2f);
        stateManager->SetFadeOutDuration(0.2f);
        loadingScreen_ = MakeShared<SplashScreen>(context_);
        loadingScreen_->SetMouseMode(MM_RELATIVE);
        loadingScreen_->SetMouseVisible(false);
        loadingScreen_->QueueSceneResourcesAsync("Scenes/Scene.xml");
        loadingScreen_->SetProgressColor(Color::WHITE);
        loadingScreen_->SetDefaultFogColor(Color::GRAY);
        loadingScreen_->SetDuration(1.0f);
        loadingScreen_->SetSkippable(true);
        loadingScreen_->SetBackgroundImage(GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Images/SplashScreen.jpg"));
        stateManager->EnqueueState(loadingScreen_);
        OpenMenu();
    }
}

void SampleProject::Stop()
{
}

#pragma once

#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Plugins/PluginApplication.h>
#include <Urho3D/Scene/Scene.h>
#include "SampleGameScreen.h"
#include "MenuGameScreen.h"

using namespace Urho3D;

/// Main class that hosts the application.
class SampleProject : public MainPluginApplication
{
    URHO3D_OBJECT(SampleProject, MainPluginApplication);

public:
    explicit SampleProject(Context* context);

    /// Initialize plugin.
    void Load() override;
    /// Start game.
    void Start(bool isMain) override;
    /// Stop game.
    void Stop() override;
    /// Deinitialize plugin.
    void Unload() override;

    /// Open main menu.
    void OpenMenu();
    /// Starting new game.
    void NewGame();
    /// Continue current game.
    void ContinueGame();
    /// Is game played.
    bool IsGamePlayed() const { return gameScreen_ != nullptr; }

private:
    /// Loading screen.
    SharedPtr<SplashScreen> loadingScreen_;
    /// Screen with actual game.
    SharedPtr<SampleGameScreen> gameScreen_;
    /// Screen with actual game.
    SharedPtr<MenuGameScreen> menuScreen_;
};

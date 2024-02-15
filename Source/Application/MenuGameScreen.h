#pragma once

#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/RmlUI/RmlUIComponent.h>

using namespace Urho3D;

class MenuGameScreen;
class SampleGameScreen;
class SampleProject;

/// A 2D UI window, managed by main UI instance returned by GetSubsystem<RmlUI>().
class MainMenuWindow : public RmlUIComponent
{
    URHO3D_OBJECT(MainMenuWindow, RmlUIComponent);

public:
    explicit MainMenuWindow(Context* context);

    static void RegisterObject(Context* context);

    void SetGame(SampleGameScreen* game);
    bool HasGame() const { return isGamePlayed_; }

	void OnContinue();
    void OnExit();
private:
    MenuGameScreen* GetApplicationState() const;

    /// Implement RmlUIComponent
    /// @{
    void Update(float timeStep) override;
    void OnDataModelInitialized() override;
    /// @}

    void OnNewGame();
    void OnSettings();

    /// Get current application instance.
    SampleProject* GetApp() const;

    bool isGamePlayed_{};
};

/// Screen with actual game.
class MenuGameScreen : public ApplicationState
{
    URHO3D_OBJECT(MenuGameScreen, ApplicationState)

public:
    explicit MenuGameScreen(Context* context);
    ~MenuGameScreen() override;

    /// Implement ApplicationState.
    /// @{
    void Activate(StringVariantMap& bundle) override;
    void Update(float timeStep) override;
    void Deactivate() override;
    /// @}

private:
    /// Handle key down
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

    /// Main scene of the game.
    SharedPtr<Scene> scene_;
    SharedPtr<Node> cameraNode_;
    SharedPtr<MainMenuWindow> window_;
};

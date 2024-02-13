#pragma once

#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/SplashScreen.h>

using namespace Urho3D;

class SampleProject;

URHO3D_GLOBAL_CONSTANT(ConstString Category_SampleProject{ "Component/SampleProject" });

/// Player controller class.
class PlayerController : public LogicComponent
{
    URHO3D_OBJECT(PlayerController, LogicComponent);

public:
    explicit PlayerController(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    void FixedUpdate(float timeStep) override;

private:
    float jumpInterval_{ 0.3f };
    float jumpCooldown_{};
};

/// Save file.
struct GameSaveData
{
    Vector3 actorPosition_;
    Quaternion actorRotation_;

    void SerializeInBlock(Archive& archive);
};

/// Screen with actual game.
class SampleGameScreen : public ApplicationState
{
    URHO3D_OBJECT(SampleGameScreen, ApplicationState);

public:
    explicit SampleGameScreen(Context* context);
    ~SampleGameScreen() override;

    static void RegisterObject(Context* context);

    /// Implement ApplicationState.
    /// @{
    void Activate(StringVariantMap& bundle) override;
    void Update(float timeStep) override;
    void Deactivate() override;
    /// @}

private:
    /// Save game.
    void SaveGame();
    /// Load game.
    void LoadGame();
    /// Handle key down
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Init scene.
    void InitScene();
    /// Get current application instance.
    SampleProject* GetApp() const;

    /// Main scene of the game.
    SharedPtr<Scene> scene_;

    /// Actor and camera nodes.
    SharedPtr<Node> actorNode_;
    SharedPtr<Node> cameraNode_;

    /// Autosave timer.
    float autosaveTimer_{};
};

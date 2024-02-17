#pragma once

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <Editor/Foundation/SceneViewTab.h>

namespace Urho3D
{

/// This is an example of editor plugin.
/// This plugin adds a menu item to Node context menu in Hierarchy window.
class CreateRandomBox : public SceneViewAddon
{
    URHO3D_OBJECT(CreateRandomBox, SceneViewAddon);

public:
    explicit CreateRandomBox(SceneViewTab* owner)
        : SceneViewAddon(owner)
    {
        owner->OnSelectionEditMenu.Subscribe(this, &CreateRandomBox::RenderMenu);
    }

    /// Implement SceneViewAddon.
    /// @{
    ea::string GetUniqueName() const override { return "CreateRandomBox"; }
    /// @}

private:
    void RenderMenu(SceneViewPage& page, Scene* scene, SceneSelection& selection)
    {
        const bool hasNodesSelected = !selection.GetNodes().empty();
        if (ui::MenuItem("Create Random Box", nullptr, false, hasNodesSelected))
            CreateObjects(selection);
    }

    void CreateObjects(const SceneSelection& selection)
    {
        const StringVector materialNames{
            "Materials/CubeMetallic.xml",
            "Materials/CubeLimestone.xml",
            "Materials/CubePlastic.xml",
        };

        auto cache = GetSubsystem<ResourceCache>();
        auto& random = RandomEngine::GetDefaultEngine();

        for (Node* node : selection.GetNodes())
        {
            const CreateNodeActionBuilder builder{node->GetScene(), AttributeScopeHint::Node};

            const unsigned materialIndex = random.GetUInt(0, materialNames.size() - 1);
            const Vector3 position = random.GetVector3({-5.0f, 2.0f, -5.0f}, {5.0f, 5.0f, 5.0f});
            const Quaternion rotation = random.GetQuaternion();

            Node* childNode = node->CreateChild("Box");
            childNode->SetWorldPosition(position);
            childNode->SetWorldRotation(rotation);

            auto staticModel = childNode->CreateComponent<StaticModel>();
            staticModel->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            staticModel->SetMaterial(cache->GetResource<Material>(materialNames[materialIndex]));

            auto rigidBody = childNode->CreateComponent<RigidBody>();
            rigidBody->SetMass(10.0f);

            auto collisionShape = childNode->CreateComponent<CollisionShape>();
            collisionShape->SetBox(Vector3::ONE);

            owner_->PushAction(builder.Build(childNode));
        }
    }
};

inline void Plugin_CreateRandomBox(Context* context, SceneViewTab* sceneViewTab)
{
    sceneViewTab->RegisterAddon<CreateRandomBox>();
}

} // namespace Urho3D

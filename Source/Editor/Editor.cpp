#include "CreateRandomBox.h"

#include <Editor/EditorApplication.h>

using namespace Urho3D;

class CustomEditorApplication : public EditorApplication
{
public:
    using EditorApplication::EditorApplication;

    /// Implement EditorApplication.
    /// @{
    void Setup() override
    {
        editorPluginManager_->AddPlugin("SampleProject.CreateRandomBox", &Plugin_CreateRandomBox);

        EditorApplication::Setup();
        LinkedPlugins::RegisterStaticPlugins();
    }
    /// @}
};

URHO3D_DEFINE_APPLICATION_MAIN(CustomEditorApplication);

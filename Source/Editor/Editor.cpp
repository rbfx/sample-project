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
        EditorApplication::Setup();
        LinkedPlugins::RegisterStaticPlugins();
    }
    /// @}
};

URHO3D_DEFINE_APPLICATION_MAIN(CustomEditorApplication);

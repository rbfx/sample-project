#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>

#include <Player/PlayerApplication.h>

using namespace Urho3D;

class LauncherApplication : public PlayerApplication
{
public:
    using PlayerApplication::PlayerApplication;

    void Setup() override
    {
        PlayerApplication::Setup();

#if MOBILE
        engineParameters_[EP_ORIENTATIONS] = "Portrait";
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";..;../..";
#else
        auto fs = context_->GetSubsystem<FileSystem>();
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = fs->GetProgramDir() + ";" + fs->GetCurrentDir() + "../";
#endif

        engineParameters_[EP_PLUGINS] = ea::string::joined(LinkedPlugins::GetLinkedPlugins(), ";");
        LinkedPlugins::RegisterStaticPlugins();
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(LauncherApplication);

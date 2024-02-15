#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>

#include <Player/PlayerApplication.h>

namespace Urho3D
{

class LauncherApplication : public PlayerApplication
{
public:
    using PlayerApplication::PlayerApplication;

    void Setup() override
    {
        PlayerApplication::Setup();

        auto fs = context_->GetSubsystem<FileSystem>();
        engineParameters_[EP_RESOURCE_PATHS] = "CoreData;Data";

#if MOBILE
        engineParameters_[EP_ORIENTATIONS] = "Portrait";
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";..;../..";
#else
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = fs->GetProgramDir() + ";" + fs->GetCurrentDir() + "../";
#endif
    }
};

} // namespace Urho3D

URHO3D_DEFINE_APPLICATION_MAIN(Urho3D::LauncherApplication);

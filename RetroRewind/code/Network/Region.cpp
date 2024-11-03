#include <kamek.hpp>
#include <core/rvl/DWC/DWC.hpp>
#include <core/System/SystemManager.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Info.hpp>
#include <PulsarSystem.hpp>
#include <Network/WiiLink.hpp>

namespace Pulsar {
namespace Network {
//Region Patch (Leseratte)
static void PatchLoginRegion() {
    const Pulsar::Info& infoInstance = Pulsar::System::sInstance->GetInfo();
    u32 region = infoInstance.GetWiimmfiRegion();
    WWFC_CUSTOM_REGION = region;
    char path[0x9];
    snprintf(path, 0x9, "%08d", region + 100000);
    for(int i = 0; i < 8; ++i) {
        DWC::loginRegion[i] = path[i];
    }
}
BootHook LoginRegion(PatchLoginRegion, 2);

int PatchRegion(char* path, u32 len, const char* fmt, const char* mode) {
    const Info& info = System::sInstance->GetInfo();
    return snprintf(path, len, fmt, mode, info.GetWiimmfiRegion());
}

kmCall(0x8065921c, PatchRegion);
kmCall(0x80659270, PatchRegion);
kmCall(0x80659734, PatchRegion);
kmCall(0x80659788, PatchRegion);


//kmWrite32(0x8065a038, 0x7C050378);
//kmWrite32(0x8065a084, 0x7C050378);
static int GetFriendsSearchType(int curType, u32 regionId) {
    register u8 friendRegionId;
    asm(mr friendRegionId, r0;);
    const Pulsar::Info& infoInstance = Pulsar::System::sInstance->GetInfo();
    u8 region = infoInstance.GetWiimmfiRegion();  // Call GetWiimmfiRegion on the retrieved Info reference
    if (region != friendRegionId) return curType;
    else if (curType == 7) return 6;
    else return 9;
}
kmBranch(0x8065a03c, GetFriendsSearchType);
kmBranch(0x8065a088, GetFriendsSearchType);



static u32 PatchRKNetControllerRegion() {
    const Pulsar::Info& infoInstance = Pulsar::System::sInstance->GetInfo();
    return infoInstance.GetWiimmfiRegion();  // Call GetWiimmfiRegion on the retrieved Info reference
}
kmCall(0x80653640, PatchRKNetControllerRegion);
kmWrite32(0x80653644, 0x7c651b78);
kmCall(0x806536ac, PatchRKNetControllerRegion); //for battle
kmWrite32(0x806536b0, 0x7c661b78);

//kmCall(0x80653700, PatchRKNetControllerRegion); this is for battle, right now it'll store 2 (if pal)/FF



//kmWrite32(0x8065A034, 0x3880008E);
//kmWrite32(0x8065A080, 0x3880008E);

}//namespace Network
}//namespace Pulsar
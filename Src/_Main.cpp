#include "ModUtils.hpp"
#include "HooksUtils.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

HWND hWnd;

struct markChildrensWithIndex : public CCNode {
    void doit(float) {
        ModUtils::markChildrensWithIndex(this);
    }
};

inline CCLayer* (__cdecl* CCLayer_create)();
CCLayer* CCLayer_create_H() {
    CCLayer* pCCLayer = CCLayer_create();
    pCCLayer->schedule(schedule_selector(markChildrensWithIndex::doit), 0.1f);
    return pCCLayer;
}

inline CCScene* (__cdecl* CCScene_create)();
CCScene* CCScene_create_H() {
    CCScene* pCCScene = CCScene_create();
    pCCScene->schedule(schedule_selector(markChildrensWithIndex::doit), 0.1f);
    return pCCScene;
}

DWORD WINAPI ModThread(void* hModule) {
    //game ver check
    if (ModUtils::GetGameVersion() != "2.200")
        if (MessageBoxA(hWnd, "This mod developed for Geometry Dash 2.200!\nStill countinue?", "Wrong game version", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
            return 1;
    //othher stuff
    MH_Initialize();
    CC_HOOK("?create@CCLayer@cocos2d@@SAPAV12@XZ", CCLayer_create);
    CC_HOOK("?create@CCScene@cocos2d@@SAPAV12@XZ", CCScene_create);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return TRUE;
    hWnd = GetForegroundWindow();//сatch the game window right on launch (maybe on launch) hehe
    CreateThread(0, 0, ModThread, hModule, 0, 0);
    return TRUE;
}

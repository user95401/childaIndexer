#include "ModUtils.hpp"
#include "HooksUtils.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

HWND hWnd;

bool bEnabled = true;

DWORD WINAPI EnableThread(void* hModule);
DWORD WINAPI DisableThread(void* hModule);
FLAlertLayer* lastAlert;

DWORD WINAPI EnableThread(void* hModule) {
    while (true) {
        if (GetKeyState(VK_F1) & 0x8000) {
            bEnabled = true;
            CreateThread(0, 0, DisableThread, hModule, 0, 0);
            return 0;
        }
    }
    return 0;
}

DWORD WINAPI DisableThread(void* hModule) {
    while (true) {
        if (GetKeyState(VK_F1) & 0x8000) {
            bEnabled = false;
            CreateThread(0, 0, EnableThread, hModule, 0, 0);
            return 0;
        }
    }
    return 0;
}


struct markChildrensWithIndex : public CCNode {
    void doit(float) {

        if (bEnabled) {
            ModUtils::markChildrensWithIndex(this);
            //was disabled label
            this->removeChildByTag(9540);
            //info
            this->removeChildByTag(5410);
            this->addChild(CCLabelTTF::create(
                "u can toggle childaIndexer with F1",
                "Comic Sans MS", 5.f,
                { CCDirector::sharedDirector()->getWinSize().width * 1.99f, 20 }, CCTextAlignment::kCCTextAlignmentRight
            ), 999, 5410);
            this->getChildByTag(5410)->runAction(CCEaseExponentialIn::create(CCFadeOut::create(5.f)));
        }

        else if (this->getChildByTag(5920) && !this->getChildByTag(9540)) {
            this->addChild(CCLabelTTF::create(
                "childaIndexer was disabled!\n(reopen that scene to take effect)",
                "Comic Sans MS", 12.f,
                { CCDirector::sharedDirector()->getWinSize().width * 1.9f, 100 }, CCTextAlignment::kCCTextAlignmentRight
            ), 999, 9540);
            this->getChildByTag(9540)->runAction(CCEaseExponentialIn::create(CCFadeOut::create(8.f)));
        }

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
    CreateThread(0, 0, DisableThread, hModule, 0, 0);
    return TRUE;
}

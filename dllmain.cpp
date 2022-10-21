// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <iostream>
#include "mem.h"
#include "proc.h"

// Created with ReClass.NET 1.2 by KN4CK3R

struct Vector3 { float x, y, z; };

class playerClass
{
public:
    class N00000165* vTable; //0x0000
    Vector3 playerHead; //0x0004
    Vector3 playerVelocity; //0x0010
    char pad_001C[32]; //0x001C
    Vector3 playerBody; //0x003C
    Vector3 headAngles; //0x0048
    char pad_0054[164]; //0x0054
    int32_t health; //0x00F8
    int32_t ammo; //0x00FC
    char pad_0100[60]; //0x0100
    int32_t pistolAmmoClip; //0x013C
    char pad_0140[16]; //0x0140
    int32_t arAmmoClip; //0x0150
    char pad_0154[4]; //0x0154
    int32_t grenadeCount; //0x0158
    char pad_015C[4]; //0x015C
    int32_t knifeWait; //0x0160
    int32_t pistolWait; //0x0164
    char pad_0168[16]; //0x0168
    int32_t arWait; //0x0178
    char pad_017C[504]; //0x017C
    class weapon* currentWeapon; //0x0374
}; //Size: 0x0378
static_assert(sizeof(playerClass) == 0x378);

class N00000165
{
public:
    char pad_0000[68]; //0x0000
}; //Size: 0x0044
static_assert(sizeof(N00000165) == 0x44);

class N00000185
{
public:
    char pad_0000[4]; //0x0000
}; //Size: 0x0004
static_assert(sizeof(N00000185) == 0x4);

class N000001E3
{
public:
    char pad_0000[68]; //0x0000
}; //Size: 0x0044
static_assert(sizeof(N000001E3) == 0x44);

class weapon
{
public:
    char pad_0000[20]; //0x0000
    class ammoPtr1* ammoPtr; //0x0014
}; //Size: 0x0018
static_assert(sizeof(weapon) == 0x18);

class ammoPtr1
{
public:
    int32_t ammo; //0x0000
}; //Size: 0x0004
static_assert(sizeof(ammoPtr1) == 0x4);

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "OG for a fee, stay sippin' fam\n";

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    //calling it with NULL also gives you the address of the .exe module
    moduleBase = (uintptr_t)GetModuleHandle(NULL);

    bool bHealth = false, bAmmo = false, bRecoil = false;

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
            bHealth = !bHealth;

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;
        }

        //no recoil NOP
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }

            else
            {
                //50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }

        //need to use uintptr_t for pointer arithmetic later
        //uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

        playerClass* localPlayerPtr = *(playerClass**)(moduleBase + 0x10F4F4);

        //continuous writes / freeze

        if (localPlayerPtr)
        {
            if (bHealth)
            {

                //*localPlayerPtr = derference the pointer, to get the localPlayerAddr
                // add 0xF8 to get health address
                //cast to an int pointer, this pointer now points to the health address
                //derference it and assign the value 1337 to the health variable it points to
                
                //*(int*)(*localPlayerPtr + 0xF8) = 1337;
                localPlayerPtr->health = 1337;

            }

            if (bAmmo)
            {
                //We aren't external now, we can now efficiently calculate all pointers dynamically
                //before we only resolved pointers when needed for efficiency reasons
                //we are executing internally, we can calculate everything when needed
                /*uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
                int* ammo = (int*)ammoAddr;
                *ammo = 1337;*/

                //or just
                //*(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
                localPlayerPtr->currentWeapon->ammoPtr->ammo = 1337;
            }

        }
        Sleep(5);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // More info @ http://www.youtube.com/watch?v=hlioPJ_uB7M&t=17m50s
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




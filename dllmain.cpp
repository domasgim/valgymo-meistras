// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <iostream>
#include "mem.h"
#include "proc.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

// Created with ReClass.NET 1.2 by KN4CK3R

struct Vector3 { float x, y, z; };

class weapon
{
public:
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(int*, ammoPtr, 0x14);
    };
};

class playerClass
{
public:
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(Vector3, playerHead, 0x4);
        DEFINE_MEMBER_N(Vector3, playerVelocity, 0x10);
        DEFINE_MEMBER_N(Vector3, playerBody, 0x4);
        DEFINE_MEMBER_N(Vector3, headAngles, 0x48);
        DEFINE_MEMBER_N(int32_t, health, 0xF8);
        DEFINE_MEMBER_N(int32_t, ammo, 0xFC);
        DEFINE_MEMBER_N(int32_t, grenadeCount, 0x158);
        DEFINE_MEMBER_N(int32_t, knifeWait, 0x160);
        DEFINE_MEMBER_N(int32_t, pistolWait, 0x164);
        DEFINE_MEMBER_N(int32_t, arWait, 0x178);
        DEFINE_MEMBER_N(weapon*, currentWeapon, 0x374);
    };
};

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
                *localPlayerPtr->currentWeapon->ammoPtr = 1337;
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




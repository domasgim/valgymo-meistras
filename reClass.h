#pragma once

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
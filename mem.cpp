#include "pch.h"
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	// Old permission storage
	DWORD oldprotect;

	// Need to modify program permissions to let it r/w, standart security on windows
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}


void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

// DMA - dynamic memory allocation
uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	// Old permission storage
	DWORD oldprotect;

	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect); //
}


void mem::Nop(BYTE* dst, unsigned int size)
{
	// Old permission storage
	DWORD oldprotect;

	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

// DMA - dynamic memory allocation
uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {
		// Cast it to a pointer and derefrence it (get the value of casted pointer)
		addr = *(uintptr_t*)addr;

		addr += offsets[i];
	}
	return addr;
}

bool mem::detour32(BYTE* src, BYTE* dst, const uintptr_t len) {
	if (len < 5) return false;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	uintptr_t relativeAddress = dst - src - 5;

	*src = 0xE9;

	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

BYTE* mem::trampHook32(BYTE* src, BYTE* dst, const uintptr_t len) {
	if (len < 5) return 0;

	// Create gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Write stolen bytes to the gateway
	memcpy_s(gateway, len, src, len);

	// Get the gateway to destination address
	uintptr_t gatewayRelativeAddr = src - gateway - 5;

	// Add jmp opcode to the end of the gateway
	*(gateway + len) = 0xE9;

	// Write the address of the gateway to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	// Perform a detour
	detour32(src, dst, len);

	return gateway;
}
#ifndef APatch_H
#define APatch_H

#include <Windows.h>

namespace APatch {
	void memory_fill(void* addr, const int value, const int size) {
		DWORD newProtect;
		VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &newProtect);
		memset(addr, value, size);
		VirtualProtect(addr, size, newProtect, &newProtect);
	}

	template<typename T>
	void write_memory(void* addr, T value) {
		DWORD newProtect;
		VirtualProtect(addr, sizeof(T), PAGE_EXECUTE_READWRITE, &newProtect);
		*reinterpret_cast<T*>(addr) = value;
		VirtualProtect(addr, sizeof(T), newProtect, &newProtect);
	}

	template<typename T>
	T read_memory(void* addr) {
		DWORD newProtect;
		VirtualProtect(addr, sizeof(T), PAGE_EXECUTE_READWRITE, &newProtect);
		T value = *reinterpret_cast<T*>(addr);
		VirtualProtect(addr, sizeof(T), newProtect, &newProtect);
		return value;
	}

	void hex2bin(void* addr, const char* value, int size) {
		memcpy(addr, value, size);
	}

	int memory_compare(void* addr1, void* addr2, int bytes) {
		DWORD newProtect;
		DWORD newProtect1;
		VirtualProtect(addr1, bytes, PAGE_EXECUTE_READWRITE, &newProtect);
		VirtualProtect(addr2, bytes, PAGE_EXECUTE_READWRITE, &newProtect1);
		int result = memcmp(addr1, addr2, bytes);
		VirtualProtect(addr2, bytes, newProtect1, &newProtect1);
		VirtualProtect(addr1, bytes, newProtect, &newProtect);
		return result;
	}

	DWORD protect(void* addr, int size, DWORD newProtect) {
		VirtualProtect(addr, size, newProtect, &newProtect);
	}

	DWORD unprotect(void* addr, int size) {
		DWORD newProtect;
		VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &newProtect);
		return newProtect;
	}
}

#endif // !APatch_H
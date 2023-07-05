#ifndef _SAMP_LIBRARY_HPP_
#define _SAMP_LIBRARY_HPP_

#include <Windows.h>

namespace SAMP
{
	enum class sampVersion : int
	{
		unknown = 0,
		notLoaded = -1,
		R1 = 1,
		R3 = 2,
		R5 = 3,
	}; // enum class sampVersion
	unsigned long GetSAMPHandle() {
		static unsigned long samp{ 0 };
		if (!samp) {
			samp = reinterpret_cast<unsigned long>(GetModuleHandleA("samp"));
			if (!samp || samp == -1 || reinterpret_cast<HANDLE>(samp) == INVALID_HANDLE_VALUE) {
				samp = 0;
			}
		}
		return samp;
	}
	enum sampVersion GetSAMPVersion() {
		static sampVersion sampVersion = sampVersion::unknown;
		if (sampVersion != sampVersion::unknown) {
			return sampVersion;
		}
		unsigned long samp = GetSAMPHandle();
		if (!samp) {
			sampVersion = sampVersion::notLoaded;
		}
		else {
			unsigned long EntryPoint = reinterpret_cast<IMAGE_NT_HEADERS*>(samp + reinterpret_cast<IMAGE_DOS_HEADER*>(samp)->e_lfanew)->OptionalHeader.AddressOfEntryPoint;
			switch (EntryPoint) {
			case (0x31DF13): {
				sampVersion = sampVersion::R1;
				break;
			}
			case (0xCC4D0): {
				sampVersion = sampVersion::R3;
				break;
			}
			case (0xCBC90): {
				sampVersion = sampVersion::R5;
				break;
			}
			default: {
				sampVersion = sampVersion::unknown;
				break;
			}
			}
		}
		return sampVersion;
	}

	bool IsSAMPInitialized() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0F8) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8DC) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB94) != nullptr;
		return false;
	}

	int GetFontSize() {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xB3C60)();
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5B20)();
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5290)();
	}

	int GetNameTagsAddr() {
		if (GetSAMPVersion() == sampVersion::R1)
			return GetSAMPHandle() + 0x686C0;
		else if (GetSAMPVersion() == sampVersion::R3)
			return GetSAMPHandle() + 0x6C630;
		else if (GetSAMPVersion() == sampVersion::R5)
			return GetSAMPHandle() + 0x6CDA0;
		else
			return 0;
	}

	int GetHealthBarAddr() {
		if (GetSAMPVersion() == sampVersion::R1)
			return GetSAMPHandle() + 0x689C0;
		else if (GetSAMPVersion() == sampVersion::R3)
			return GetSAMPHandle() + 0x6C930;
		else if (GetSAMPVersion() == sampVersion::R5)
			return GetSAMPHandle() + 0x6D0A0;
		else
			return 0;
	}

	int GetPlayerTagsLost() {
		if (GetSAMPVersion() == sampVersion::R1)
			return GetSAMPHandle() + 0x68F70;
		else if (GetSAMPVersion() == sampVersion::R3)
			return GetSAMPHandle() + 0x6CEE0;
		else if (GetSAMPVersion() == sampVersion::R5)
			return GetSAMPHandle() + 0x6D650;
		else
			return 0;
	}

	float getDistanceToCamera(CVector* position) {
		void* CGame{ nullptr };
		void* CCamera{ nullptr };
		if (GetSAMPVersion() == sampVersion::R1) {
			CGame = *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A10C);
			CCamera = reinterpret_cast<void* (__thiscall*)(void*)>(GetSAMPHandle() + 0x2D20)(CGame);
			return reinterpret_cast<float(__thiscall*)(void*, CVector*)>(GetSAMPHandle() + 0x990A0)(CCamera, position);
		}
		else if (GetSAMPVersion() == sampVersion::R3) {
			CGame = *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8F4);
			CCamera = reinterpret_cast<void* (__thiscall*)(void*)>(GetSAMPHandle() + 0x2D10)(CGame);
			return reinterpret_cast<float(__thiscall*)(void*, CVector*)>(GetSAMPHandle() + 0x9CFF0)(CCamera, position);
		}
		else if (GetSAMPVersion() == sampVersion::R5) {
			CGame = *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EBAC);
			CCamera = reinterpret_cast<void* (__thiscall*)(void*)>(GetSAMPHandle() + 0x2D30)(CGame);
			return reinterpret_cast<float(__thiscall*)(void*, CVector*)>(GetSAMPHandle() + 0x9D700)(CCamera, position);
		}
		else
			return 0.f;
	}

	void nopZalupa() {
		if (GetSAMPVersion() == sampVersion::R1) {
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x6FC81), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x6FE7D), 0x90, 5);

			APatch::memory_fill((void*)(GetSAMPHandle() + 0x70D71), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x70FAE), 0x90, 5);
		}
		else if (GetSAMPVersion() == sampVersion::R3) {
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x73B71), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x73D71), 0x90, 5);

			APatch::memory_fill((void*)(GetSAMPHandle() + 0x74C61), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x74EA2), 0x90, 5);
		}
		else if (GetSAMPVersion() == sampVersion::R5) {
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x74261), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x7446E), 0x90, 5);

			APatch::memory_fill((void*)(GetSAMPHandle() + 0x75361), 0x90, 5);
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x755AF), 0x90, 5);
		}
	}

}; // namespace SAMP

#endif // !_SAMP_LIBRARY_HPP_
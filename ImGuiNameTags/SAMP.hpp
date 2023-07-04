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

	const char* GetNameFormat() {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<const char*>(GetSAMPHandle() + 0xD835C);
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<const char*>(GetSAMPHandle() + 0xEA738);
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<const char*>(GetSAMPHandle() + 0xEA790);
	}

	void NopNameTags() {
		if (GetSAMPVersion() == sampVersion::R1)
			memwrapper::fill_memory(SAMP::GetSAMPHandle() + 0x71190, 0x90, 10);
		else if (GetSAMPVersion() == sampVersion::R3)
			memwrapper::fill_memory(SAMP::GetSAMPHandle() + 0x75080, 0x90, 10);
		else if (GetSAMPVersion() == sampVersion::R5)
			memwrapper::fill_memory(SAMP::GetSAMPHandle() + 0x7578F, 0x90, 10);
	}
}; // namespace SAMP

#endif // !_SAMP_LIBRARY_HPP_
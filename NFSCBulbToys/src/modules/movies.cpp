#include "../../core/bulbtoys.h"

namespace movies
{
	void Init()
	{
		// "NTSC\0" is 5, but we add an extra character to invalidate eg. "NTSCx"
		Settings::String<"Movies", "Force", "off", 6> force;

		// Only psychopaths use mixed case here
		if (!strcmp(force.Get(), "pal") || !strcmp(force.Get(), "PAL"))
		{
			Patch<uint16_t>(0x583BEF, 0x9090);
		}
		if (!strcmp(force.Get(), "ntsc") || !strcmp(force.Get(), "NTSC"))
		{
			Patch<uint8_t>(0x583BEF, 0xEB);
		}
	}

	void End()
	{
		// Since Force might not be PAL/NTSC (ie. disabled), we pass false here
		Unpatch(0x583BEF, false);
	}
}

MODULE_NO_PANEL(movies);
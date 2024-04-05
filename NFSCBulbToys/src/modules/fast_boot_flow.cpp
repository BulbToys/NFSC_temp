#include "../../ext/base/bulbtoys.h"

namespace fast_boot_flow
{
	bool enabled = true;

	void Init()
	{
		Settings::Bool<"FastBootFlow", "Enabled", true> setting;
		fast_boot_flow::enabled = setting.Get();
		if (!fast_boot_flow::enabled)
		{
			return;
		}

		// Prevent pushing splash screen: "DEMO_SPLASH.fng" -> ""
		Patch<uint8_t>(0x9CB4E4, 0);

		// Patch FEStateManager::Push's next_state: STATE_DO_SPLASH -> STATE_BOOTCHECK
		Patch<uint8_t>(0x5BD528, 3);

		// Patch out FEBFSM::ShowBackdrop() (STATE_BACKDROP)
		PatchNOP(0x5891C3, 8);

		// Patch out everything but STATE_SPLASH and STATE_AUTOLOAD in FEBFSM::ShowEverythingElse() (STATE_EA_LOGO, STATE_PSA and STATE_ATTRACT)
		PatchNOP(0x716583, 34);
		PatchNOP(0x7165B1, 12);
	}

	void End()
	{
		if (!fast_boot_flow::enabled)
		{
			return;
		}

		Unpatch(0x7165B1);
		Unpatch(0x716583);

		Unpatch(0x5891C3);

		Unpatch(0x9CB4E4);

		Unpatch(0x5BD528);
	}
}

MODULE_NO_PANEL(fast_boot_flow);
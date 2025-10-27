#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace qr_autosave
{
	//HOOK(0x644FF0, void, __fastcall, GRaceDatabase_UpdateRaceScore, uintptr_t race_db, void* edx, char flag);

	void Init()
	{
		PatchNOP(0x5BDF40, 6);
		//CREATE_HOOK(GRaceDatabase_UpdateRaceScore);
	}

	void End()
	{
		Unpatch(0x5BDF40);
		//Hooks::Destroy(0x644FF0);
	}

	/*
	void __fastcall GRaceDatabase_UpdateRaceScore_(uintptr_t race_db, void* edx, char flag)
	{
		GRaceDatabase_UpdateRaceScore(race_db, edx, flag);

		//Error("Saved!");
	}
	*/
}

MODULE_NO_PANEL(qr_autosave);
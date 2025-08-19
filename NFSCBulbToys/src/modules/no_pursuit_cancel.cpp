#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace no_pursuit_cancel
{
	HOOK(0x6721F0, uintptr_t, __fastcall, ESelectEngagableTrigger_Construct, uintptr_t event);
	HOOK(0x695760, void, __fastcall, ESelectEngagableTrigger_Destruct, uintptr_t event);

	HOOK(VIRTUAL, void, __fastcall, FEPostPursuitStateManager_HandlePadAccept, uintptr_t fesm);
	HOOK(VIRTUAL, void, __fastcall, FEPostPursuitStateManager_HandleFlowDone, uintptr_t fesm);

	int career_fe_location = -1;

	uintptr_t __fastcall ESelectEngagableTrigger_Construct_(uintptr_t event)
	{
		uintptr_t vehicle = 0;
		uintptr_t simable = 0;
		if (!NFSC::BulbToys_GetMyVehicle(&vehicle, &simable))
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		auto player = NFSC::PhysicsObject_GetPlayer(simable);
		if (!player)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		// LocalPlayer::GetHUD
		auto hud = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(0x76E720)(player);
		if (!hud)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		// UTL::COM::Object::_IList::Find(hud, IMenuZoneTrigger::IHandle)
		auto imzt = NFSC::BulbToys_FindInterface<0x40B190>(hud);
		if (!imzt)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		// MenuZoneTrigger::IsPlayerInsideSafehouseTrigger
		auto is_safehouse = reinterpret_cast<bool(__thiscall*)(uintptr_t)>(0x5D47C0)(imzt);
		if (!is_safehouse)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		auto ai_vehicle = NFSC::PVehicle_GetAIVehiclePtr(vehicle);
		if (!ai_vehicle)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		auto pursuit = NFSC::AIVehicle_GetPursuit(ai_vehicle);
		if (!pursuit)
		{
			return ESelectEngagableTrigger_Construct(event);
		}

		// pursuit->mEvadeLevel
		Write<float>(pursuit + 0x158, 2.0);

		// force patch GManager::NotifyPursuitEnded's 2nd argument (do_roaming_check) to 0 in AICopManager::PursuitIsEvaded
		Write<uint8_t>(0x423E11, 0);

		// remember where we entered (MenuZoneTrigger::GetCareerFELocation)
		career_fe_location = reinterpret_cast<int(__thiscall*)(uintptr_t)>(0x5E6B70)(imzt);

		return ESelectEngagableTrigger_Construct(event);
	}

	void __fastcall ESelectEngagableTrigger_Destruct_(uintptr_t event)
	{
		// if we're evading the pursuit by entering the safe house, do nothing
		if (career_fe_location == -1)
		{
			ESelectEngagableTrigger_Destruct(event);
		}
	}

	void __fastcall FEPostPursuitStateManager_HandlePadAccept_(uintptr_t fesm)
	{
		if (career_fe_location == -1)
		{
			FEPostPursuitStateManager_HandlePadAccept(fesm);
			return;
		}

		// if we're evading the pursuit by entering the safe house, do the following
		auto fe_state_manager = reinterpret_cast<NFSC::FEStateManager*>(fesm);
		if (fe_state_manager->current_state != 0)
		{
			FEPostPursuitStateManager_HandlePadAccept(fesm);
			return;
		}

		struct
		{
			char data[0x14]{ 0 };
		} MEnterSafeHouse;

		// MEnterSafeHouse::MEnterSafeHouse
		auto message = reinterpret_cast<uintptr_t(__thiscall*)(void*, const char*)>(0x57A950)(&MEnterSafeHouse, "safehouse");

		// Hermes::Message::Enqueue
		reinterpret_cast<void(__thiscall*)(uintptr_t, unsigned int, int)>(0x566070)(message, 0x20D60DBF, 0);

		fe_state_manager->exit_point = 0;

		FEPostPursuitStateManager_HandlePadAccept(fesm);
	}

	void __fastcall FEPostPursuitStateManager_HandleFlowDone_(uintptr_t fesm)
	{
		if (career_fe_location == -1)
		{
			FEPostPursuitStateManager_HandleFlowDone(fesm);
			return;
		}

		// if we're evading the pursuit by entering the safe house, do the following
		auto fe_state_manager = reinterpret_cast<NFSC::FEStateManager*>(fesm);
		if (fe_state_manager->current_state != 0)
		{
			FEPostPursuitStateManager_HandleFlowDone(fesm);
			return;
		}

		// EventSys::Event::Allocate
		auto event = reinterpret_cast<uintptr_t(__cdecl*)(int, int)>(0x561590)(16, 1);
		if (event)
		{
			// EQuitToFE::EQuitToFE
			reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, int, int)>(0x6715D0)(event, 0, career_fe_location);
		}

		// reset our career fe location, and reset/unpatch GManager::NotifyPursuitEnded's 2nd argument (do_roaming_check) back to 1 in AICopManager::PursuitIsEvaded
		career_fe_location = -1;
		Write<uint8_t>(0x423E11, 1);
	}

	void Init()
	{
		CREATE_HOOK(ESelectEngagableTrigger_Construct);
		CREATE_HOOK(ESelectEngagableTrigger_Destruct);

		CREATE_VTABLE_PATCH(0x9D27A8 + 19 * 4, FEPostPursuitStateManager_HandleFlowDone);
		CREATE_VTABLE_PATCH(0x9D27A8 + 30 * 4, FEPostPursuitStateManager_HandlePadAccept);
	}

	void End()
	{
		Unpatch(0x9D27A8 + 30 * 4);
		Unpatch(0x9D27A8 + 19 * 4);

		Hooks::Destroy(0x695760);
		Hooks::Destroy(0x6721F0);
	}
}

MODULE_NO_PANEL(no_pursuit_cancel);
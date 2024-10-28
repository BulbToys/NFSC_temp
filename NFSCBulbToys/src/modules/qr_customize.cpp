#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace qr_customize
{
	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_Start, uintptr_t state_manager);
	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandlePadAccept, uintptr_t state_manager);
	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandleChildFlowDone, uintptr_t state_manager, uintptr_t edx, int unk);

	void Init()
	{
		CREATE_VTABLE_PATCH(0x9D2FB4, FEPhotoModeStateManager_Start);
		CREATE_VTABLE_PATCH(0x9D3010, FEPhotoModeStateManager_HandlePadAccept);
		CREATE_VTABLE_PATCH(0x9D2FCC, FEPhotoModeStateManager_HandleChildFlowDone);
	}

	void End()
	{
		Unpatch(0x9D2FCC);
		Unpatch(0x9D3010);
		Unpatch(0x9D2FB4);
	}

	void __fastcall FEPhotoModeStateManager_Start_(uintptr_t state_manager)
	{
		auto fesm = reinterpret_cast<NFSC::FEStateManager*>(state_manager);

		// FEQuickRaceStateManager
		if (fesm->parent && fesm->parent->parent && fesm->parent->parent->vtable == reinterpret_cast<void**>(0x9F8428))
		{
			auto handle = Read<uint32_t>(Read<uintptr_t>(reinterpret_cast<uintptr_t>(fesm->parent) + 0xD0) + 0x8);

			// Only show the Customize button for My Cars vehicles
			uintptr_t user_profile = NFSC::FEManager_GetUserProfile(NFSC::BulbToys_GetFEManager(), 0);
			uintptr_t car_record = NFSC::FEPlayerCarDB_GetCarRecordByHandle(user_profile + 0x234, handle);
			if ((Read<uint32_t>(car_record + 0xC) & 0x4) == 0x4)
			{
				// FeCrewCar.fng
				NFSC::FEStateManager_Push(state_manager, reinterpret_cast<char*>(0x9D0078), 0);
				return;
			}
		}

		// FePhotoMode.fng
		NFSC::FEStateManager_Push(state_manager, reinterpret_cast<char*>(0x9CFCA8), 0);
	}

	void __fastcall FEPhotoModeStateManager_HandlePadAccept_(uintptr_t state_manager)
	{
		auto fesm = reinterpret_cast<NFSC::FEStateManager*>(state_manager);
		auto handle = Read<uint32_t>(Read<uintptr_t>(reinterpret_cast<uintptr_t>(fesm->parent) + 0xD0) + 0x8);

		uintptr_t user_profile = NFSC::FEManager_GetUserProfile(NFSC::BulbToys_GetFEManager(), 0);
		uintptr_t car_record = NFSC::FEPlayerCarDB_GetCarRecordByHandle(user_profile + 0x234, handle);

		// FECustomizeStateManager::FECustomizeStateManager
		uintptr_t fe_customize_sm = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uintptr_t, int, uintptr_t)>(0x8631D0)(NFSC::malloc(0xD4), state_manager, 0, car_record);
		NFSC::FEStateManager_SwitchChildManager(state_manager, fe_customize_sm, NFSC::FESM::PhotoMode::GIMME_MY_QR_FNG, 1);
	}

	void __fastcall FEPhotoModeStateManager_HandleChildFlowDone_(uintptr_t state_manager, uintptr_t edx, int unk)
	{
		if (Read<int>(state_manager + 0x4) == NFSC::FESM::PhotoMode::GIMME_MY_QR_FNG)
		{
			// FeCrewCar.fng
			NFSC::FEStateManager_Push(state_manager, reinterpret_cast<char*>(0x9D0078), 0);
		}
	}
}

MODULE_NO_PANEL(qr_customize);
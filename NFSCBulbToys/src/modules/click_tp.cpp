#include "../../core/bulbtoys.h"
#include "../../core/bulbtoys/io.h"
#include "../nfsc.h"

namespace click_tp
{
	bool enabled = true;
	constexpr uint32_t gps_color = 0xFFFFFFFF;
	NFSC::Vector3 location = { 0, 0, 0 };

	HOOK(VIRTUAL, void, __fastcall, FEWorldMapStateManager_HandlePadAccept, uintptr_t state_manager);
	HOOK(VIRTUAL, void, __fastcall, FEWorldMapStateManager_HandleShowDialog, uintptr_t state_manager);
	HOOK(VIRTUAL, void, __fastcall, FEWorldMapStateManager_HandleButtonPressed, uintptr_t state_manager, uintptr_t edx, uint32_t unk);
	HOOK(VIRTUAL, void, __fastcall, FEWorldMapStateManager_HandleStateChange, uintptr_t state_manager);
	HOOK(VIRTUAL, void, __fastcall, FEWorldMapStateManager_HandleScreenTick, uintptr_t state_manager);

	void Init()
	{
		Settings::Bool<"ClickTP", "Enabled", true> setting;
		click_tp::enabled = setting.Get();
		if (!click_tp::enabled)
		{
			return;
		}

		CREATE_VTABLE_PATCH(0x9D2E88, FEWorldMapStateManager_HandlePadAccept);
		CREATE_VTABLE_PATCH(0x9D2F1C, FEWorldMapStateManager_HandleShowDialog);
		CREATE_VTABLE_PATCH(0x9D2E38, FEWorldMapStateManager_HandleButtonPressed);
		CREATE_VTABLE_PATCH(0x9D2F24, FEWorldMapStateManager_HandleStateChange);
		CREATE_VTABLE_PATCH(0x9D2F14, FEWorldMapStateManager_HandleScreenTick);
	}

	void End()
	{
		if (!click_tp::enabled)
		{
			return;
		}

		Unpatch(0x9D2F14);
		Unpatch(0x9D2F24);
		Unpatch(0x9D2E38);
		Unpatch(0x9D2F1C);
		Unpatch(0x9D2E88);
	}

	void __fastcall FEWorldMapStateManager_HandlePadAccept_(uintptr_t fe_state_manager)
	{
		// mCurrentState
		auto state = Read<int>(fe_state_manager + 4);

		if (IO::Get()->KeyHeld(VK_SHIFT) && state == NFSC::FESM::WorldMap::NORMAL)
		{
			// sub_00582D90 (seems legit??? some script shit lol)
			reinterpret_cast<void(__thiscall*)(uintptr_t, bool)>(0x582D90)(NFSC::BulbToys_GetWorldMap(), true);
			NFSC::FEStateManager_ShowDialog(fe_state_manager, NFSC::FESM::WorldMap::CLICK_TP);
			return;
		}

		FEWorldMapStateManager_HandlePadAccept(fe_state_manager);
	}

	void __fastcall FEWorldMapStateManager_HandleButtonPressed_(uintptr_t fe_state_manager, uintptr_t edx, uint32_t unk)
	{
		// mCurrentState
		auto state = Read<int>(fe_state_manager + 4);

		uintptr_t dialog_screen = Read<uintptr_t>(0xA97B14);
		if (!dialog_screen)
		{
			FEWorldMapStateManager_HandleButtonPressed(fe_state_manager, edx, unk);
			return;
		}

		uint32_t* button_hashes = Read<uint32_t*>(dialog_screen + 0x2C);

		auto gfs = NFSC::BulbToys_GetGameFlowState();

		if (state == NFSC::FESM::WorldMap::CLICK_TP)
		{
			// We are not in FE and we clicked on a valid road
			if (gfs == NFSC::GFS::RACING && !isnan(click_tp::location.y))
			{
				// First button - Jump to Location
				if (unk == button_hashes[0])
				{
					NFSC::FEStateManager_ChangeState(fe_state_manager, NFSC::FESM::WorldMap::CLICK_TP_JUMP);
				}

				// Second button - Activate GPS
				else if (unk == button_hashes[1])
				{
					NFSC::FEStateManager_ChangeState(fe_state_manager, NFSC::FESM::WorldMap::CLICK_TP_GPS);
				}

				// Third button - Cancel
				else if (unk == button_hashes[2])
				{
					NFSC::FEStateManager_PopBack(fe_state_manager, 3);
				}
			}

			// First button - OK
			else if (unk == button_hashes[0])
			{
				NFSC::FEStateManager_PopBack(fe_state_manager, 3);
			}

			return;
		}
		/*
		* TODO fepausemain gpsonly
		*
		// Only offer GPS if the option is enabled and we're not in FE
		if (g::gps_only::enabled && gfs == NFSC::GFS::RACING)
		{
			if (state == NFSC::FESM::WorldMap::RACE_EVENT || state == NFSC::FESM::WorldMap::CAR_LOT || state == NFSC::FESM::WorldMap::SAFEHOUSE)
			{
				// For these types of dialogs, use the button hashes of the GPS to safehouse prompt during pursuits
				//Write<int>(fe_state_manager + 4, 18);

				// First button - Activate GPS
				if (unk == button_hashes[0])
				{
					NFSC::FEStateManager_ChangeState(fe_state_manager, 21);
				}

				// Second button - Cancel
				else if (unk == button_hashes[1])
				{
					NFSC::FEStateManager_PopBack(fe_state_manager, 3);
				}

				return;
			}
		}
		*/

		FEWorldMapStateManager_HandleButtonPressed(fe_state_manager, edx, unk);
	}

	void __fastcall FEWorldMapStateManager_HandleScreenTick_(uintptr_t fe_state_manager)
	{
		// Revert snap radius
		Write<uintptr_t>(0x5C344D, 0x9D10A4);

		NFSC::FEColor color = { 255, 255, 255, 255 }; // white/none
		auto world_map = NFSC::BulbToys_GetWorldMap();

		if (world_map)
		{
			// WorldMap->mEventIconGlow
			uintptr_t cursor = Read<uintptr_t>(world_map + 0x28);

			if (IO::Get()->KeyHeld(VK_SHIFT))
			{
				// Only change the color for the "normal" mode of the world map (not quick list, etc...)
				if (Read<int>(fe_state_manager + 4) == NFSC::FESM::WorldMap::NORMAL)
				{
					// (Un)Patch non-conforming !!!
					constexpr float zero = .0f;
					Write<const float*>(0x5C344D, &zero);

					color = { 0, 0, 255, 255 }; // red

					// Get the current position of the cursor relative to the screen
					float x, y;
					NFSC::FE_Object_GetCenter(cursor, &x, &y);

					// Account for WorldMap pan
					NFSC::Vector2 temp;
					temp.x = x;
					temp.y = y;

					NFSC::WorldMap_GetPanFromMapCoordLocation(world_map, &temp, &temp);

					x = temp.x;
					y = temp.y;

					// Account for WorldMap zoom
					auto top_left = Read<NFSC::Vector2>(world_map + 0x44);
					auto size = Read<NFSC::Vector2>(world_map + 0x4C);

					x = x * size.x + top_left.x;
					y = y * size.y + top_left.y;

					// TrackInfo::GetTrackInfo(5000) - it just works tm
					auto track_info = reinterpret_cast<uintptr_t(*)(int)>(0x7990C0)(5000);

					// Inverse WorldMap::ConvertPos to get world coordinates
					float calibration_width = Read<float>(track_info + 0xB4);
					float calibration_offset_x = Read<float>(track_info + 0xAC);
					float calibration_offset_y = Read<float>(track_info + 0xB0);

					x = x - top_left.x;
					y = y - top_left.y;
					x = x / size.x;
					y = y / size.y;
					y = y - 1.0f;
					y = y * calibration_width;
					x = x * calibration_width;
					x = x + calibration_offset_x;
					y = -y;
					y = y - calibration_offset_y - calibration_width;

					// Inverse GetVehicleVectors to get position from world coordinates
					click_tp::location.x = -y;
					click_tp::location.y = 0; // z
					click_tp::location.z = x;

					// Attempt to get world height at given position
					NFSC::WCollisionMgr mgr;
					mgr.fSurfaceExclusionMask = 0;
					mgr.fPrimitiveMask = 3;

					float height = NAN;
					if (NFSC::WCollisionMgr_GetWorldHeightAtPointRigorous(mgr, &click_tp::location, &height, nullptr))
					{
						// Successful
						color = { 0, 255, 0, 255 }; // green
					}
					click_tp::location.y = height;
				}
			}

			NFSC::FE_Object_SetColor(cursor, &color);
		}

		FEWorldMapStateManager_HandleScreenTick(fe_state_manager);
	}

	void __fastcall FEWorldMapStateManager_HandleShowDialog_(uintptr_t fe_state_manager)
	{
		constexpr uintptr_t HideCursor = 0xA97B38;

		const char* COMMON_CANCEL = NFSC::GetLocalizedString(0x1A294DAD);
		const char* COMMON_OK = NFSC::GetLocalizedString(0x417B2601);
		const char* DIALOG_MSG_ACTIVATE_GPS = NFSC::GetLocalizedString(0x6EB0EACE);

		// mCurrentState
		auto type = Read<int>(fe_state_manager + 4);

		if (type == NFSC::FESM::WorldMap::CLICK_TP)
		{
			Write<bool>(HideCursor, false);

			char title[64];
			if (isnan(click_tp::location.y))
			{
				sprintf_s(title, 64, "Selected coordinates: (%.2f, N/A, %.2f)", click_tp::location.x, click_tp::location.z);

				NFSC::FEDialogScreen_ShowDialog(title, COMMON_OK, nullptr, nullptr);
			}
			else
			{
				sprintf_s(title, 64, "Selected coordinates: (%.2f, %.2f, %.2f)", click_tp::location.x, click_tp::location.y, click_tp::location.z);

				NFSC::FEDialogScreen_ShowDialog(title, "Jump to Location", DIALOG_MSG_ACTIVATE_GPS, COMMON_CANCEL);
			}

			return;
		}

		/*
		* TODO fepausemain gpsonly
		*
		// Only offer GPS if the option is enabled and we're not in FE
		if (g::gps_only::enabled && NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
		{
			if (type == NFSC::FESM::WorldMap::RACE_EVENT)
			{
				// Unhide the cursor and show the dialog with its respective string
				Write<bool>(HideCursor, false);

				// DIALOG_MSG_WORLDMAP_EVENT
				NFSC::FEDialogScreen_ShowDialog(NFSC::GetLocalizedString(0xCF93709B), DIALOG_MSG_ACTIVATE_GPS, COMMON_CANCEL, nullptr);
				return;
			}
			else if (type == NFSC::FESM::WorldMap::CAR_LOT)
			{
				Write<bool>(HideCursor, false);

				// DIALOG_MSG_WORLDMAP_CARLOT
				NFSC::FEDialogScreen_ShowDialog(NFSC::GetLocalizedString(0xBBE2483E), DIALOG_MSG_ACTIVATE_GPS, COMMON_CANCEL, nullptr);

				return;
			}
			else if (type == NFSC::FESM::WorldMap::SAFEHOUSE)
			{
				Write<bool>(HideCursor, false);

				// DIALOG_MSG_WORLDMAP_SAFEHOUSE
				NFSC::FEDialogScreen_ShowDialog(NFSC::GetLocalizedString(0x8B776D3C), DIALOG_MSG_ACTIVATE_GPS, COMMON_CANCEL, nullptr);

				return;
			}
		}
		*/

		FEWorldMapStateManager_HandleShowDialog(fe_state_manager);
	}

	void __fastcall FEWorldMapStateManager_HandleStateChange_(uintptr_t fe_state_manager)
	{
		// mCurrentState
		auto state = Read<int>(fe_state_manager + 4);

		if (state == NFSC::FESM::WorldMap::CLICK_TP_JUMP)
		{
			uintptr_t vehicle = 0;
			uintptr_t simable = 0;
			if (!NFSC::BulbToys_GetMyVehicle(&vehicle, &simable))
			{
				return;
			}

			uintptr_t rigid_body = NFSC::PhysicsObject_GetRigidBody(simable);

			NFSC::Vector3 fwd_vec;
			NFSC::RigidBody_GetForwardVector(rigid_body, &fwd_vec);

			NFSC::Vector3 dimensions;
			NFSC::RigidBody_GetDimension(rigid_body, &dimensions);
			click_tp::location.y += dimensions.y + 0.5f;

			NFSC::PVehicle_SetVehicleOnGround(vehicle, &click_tp::location, &fwd_vec);

			// this->mNextManager = this->mParentManager;
			Write<uintptr_t>(fe_state_manager + 0xB4, Read<uintptr_t>(fe_state_manager + 0xAC));

			// this->mExitPoint = 2;
			Write<int>(fe_state_manager + 0xC, 2);

			// this->mSubState = 3;
			Write<int>(fe_state_manager + 0x18, 3);

			// FEStateManager::ProcessScreenTransition(this);
			reinterpret_cast<void(__thiscall*)(uintptr_t)>(0x59B1B0)(fe_state_manager);

			return;
		}
		else if (state == NFSC::FESM::WorldMap::CLICK_TP_GPS)
		{
			if (NFSC::GPS_Engage(&click_tp::location, 0.0, false))
			{
				NFSC::Vector3 position = { click_tp::location.z, -click_tp::location.x, click_tp::location.y };
				auto icon = NFSC::GManager_AllocIcon(NFSC::BulbToys_GetGManagerBase(), 0x15, &position, 0, false);

				// Set flag to ShowOnSpawn
				//Write<uint8_t>(icon_addr + 1, 0x40);

				// Set flag to ShowInWorld + ShowOnMap
				Write<uint8_t>(icon + 0x1, 3);

				// Set color to white
				Write<uint32_t>(icon + 0x20, click_tp::gps_color);

				// Set tex hash
				Write<uint32_t>(icon + 0x24, NFSC::bStringHash("MINIMAP_ICON_EVENT"));

				NFSC::GIcon_Spawn(icon);
				NFSC::WorldMap_SetGPSIng(icon);

				// Set flag to previous + Spawned + Enabled + GPSing
				Write<uint8_t>(icon + 1, 0x8F);
			}

			// this->mNextManager = this->mParentManager;
			Write<uintptr_t>(fe_state_manager + 0xB4, Read<uintptr_t>(fe_state_manager + 0xAC));

			// this->mExitPoint = 2;
			Write<int>(fe_state_manager + 0xC, 2);

			// this->mSubState = 3;
			Write<int>(fe_state_manager + 0x18, 3);

			// FEStateManager::ProcessScreenTransition(this);
			reinterpret_cast<void(__thiscall*)(uintptr_t)>(0x59B1B0)(fe_state_manager);

			return;
		}

		FEWorldMapStateManager_HandleStateChange(fe_state_manager);
	}
}

MODULE_NO_PANEL(click_tp);
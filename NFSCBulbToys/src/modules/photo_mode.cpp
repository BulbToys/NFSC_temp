#include "../../core/bulbtoys.h"
#include "../../core/bulbtoys/io.h"
#include "../nfsc.h"

namespace photo_mode
{
	uintptr_t GetCDActionDebug()
	{
		if (NFSC::BulbToys_GetGameFlowState() != NFSC::GFS::RACING)
		{
			return 0;
		}

		uintptr_t first_camera_director = Read<uintptr_t>(0xA8ACC4 + 4);
		uintptr_t camera_director = Read<uintptr_t>(first_camera_director);
		uintptr_t cd_action = Read<uintptr_t>(camera_director + 0x18);

		// Check if we're in CDActionDebug (CDActionDebug::`vftable'{for `CameraAI::Action'})
		if (Read<uintptr_t>(cd_action) != 0x9C7EE0)
		{
			return 0;
		}

		return cd_action;
	}

	constexpr uint16_t fov_step = 1000;
	constexpr uint16_t min_fov = 5000;
	constexpr uint16_t default_fov = 10000;
	constexpr uint16_t max_fov = 20000;

	bool in_debug = false;
	bool hide_hud = false;

	bool increase_fov = false;
	bool decrease_fov = false;

	HOOK(VIRTUAL, void, __fastcall, DebugWorldCameraMover_Update, uintptr_t this_, void* edx, float dt);

	HOOK(0x5BBCC0, void*, __fastcall, FEPhotoMode_Constructor, void* this_, void* edx, void* scd);
	HOOK(0x5BC010, void, __fastcall, FEPhotoMode_Destructor, void* this_);

	HOOK(0x59BBA0, void*, __fastcall, FEPhotoModeStateManager_Constructor, void* this_, void* edx, void* parent, unsigned int car_handle);
	HOOK(0x59BCD0, void, __fastcall, FEPhotoModeStateManager_Destructor, void* this_);

	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandleLTrigger, void* this_);
	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandleRTrigger, void* this_);

	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandleLTriggerRelease, void* this_);
	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandleRTriggerRelease, void* this_);

	HOOK(VIRTUAL, void, __fastcall, FEPhotoModeStateManager_HandlePadButton5, void* this_);

	void Init()
	{
		CREATE_VTABLE_PATCH(0x9C7A10, DebugWorldCameraMover_Update);

		CREATE_HOOK(FEPhotoMode_Constructor);
		CREATE_HOOK(FEPhotoMode_Destructor);

		CREATE_HOOK(FEPhotoModeStateManager_Constructor);
		CREATE_HOOK(FEPhotoModeStateManager_Destructor);

		CREATE_VTABLE_PATCH(0x9D306C, FEPhotoModeStateManager_HandleLTrigger);
		CREATE_VTABLE_PATCH(0x9D308C, FEPhotoModeStateManager_HandleRTrigger);

		CREATE_VTABLE_PATCH(0x9D3070, FEPhotoModeStateManager_HandleLTriggerRelease);
		CREATE_VTABLE_PATCH(0x9D3090, FEPhotoModeStateManager_HandleRTriggerRelease);

		CREATE_VTABLE_PATCH(0x9D3030, FEPhotoModeStateManager_HandlePadButton5);
	}

	void End()
	{
		Unpatch(0x9D3030);

		Unpatch(0x57BA20);
		Unpatch(0x57B990);

		Unpatch(0x9D308C);
		Unpatch(0x9D306C);

		Hooks::Destroy(0x59BCD0);
		Hooks::Destroy(0x59BBA0);

		Hooks::Destroy(0x5BC010);
		Hooks::Destroy(0x5BBCC0);

		Unpatch(0x9C7A10);
	}

	void __fastcall DebugWorldCameraMover_Update_(uintptr_t this_, void* edx, float dt)
	{
		if (in_debug)
		{
			uintptr_t camera = Read<uintptr_t>(this_ + 0x1C);
			auto horizontal_fov = camera + 0xE4;

			if (increase_fov)
			{
				*reinterpret_cast<uint16_t*>(horizontal_fov) += (uint16_t)((float)fov_step * dt);
				if (Read<uint16_t>(horizontal_fov) > max_fov)
				{
					Write<uint16_t>(horizontal_fov, max_fov);
				}
			}

			if (decrease_fov)
			{
				*reinterpret_cast<uint16_t*>(horizontal_fov) -= (uint16_t)((float)fov_step * dt);
				if (Read<uint16_t>(horizontal_fov) < min_fov)
				{
					Write<uint16_t>(horizontal_fov, min_fov);
				}
			}
		}

		DebugWorldCameraMover_Update(this_, edx, dt);
	}

	void* __fastcall FEPhotoMode_Constructor_(void* this_, void* edx, void* scd)
	{
		auto fng = FEPhotoMode_Constructor(this_, edx, scd);

		if (in_debug)
		{
			NFSC::CameraAI_SetAction(1, "CDActionDebug");

			uintptr_t camera_mover = Read<uintptr_t>(GetCDActionDebug() + 0x2BC);
			uintptr_t camera = Read<uintptr_t>(camera_mover + 0x1C);

			// DebugWorldCameraMover->pCamera->CurrentKey.HorizontalFOV
			Write<uint16_t>(camera + 0xE4, default_fov);
		}

		return fng;
	}

	void __fastcall FEPhotoMode_Destructor_(void* this_)
	{
		if (in_debug)
		{
			// close debugcam
			Write<bool>(GetCDActionDebug() + 0x2C8, true);
		}

		FEPhotoMode_Destructor(this_);
	}

	void* __fastcall FEPhotoModeStateManager_Constructor_(void* this_, void* edx, void* parent, unsigned int car_handle)
	{
		in_debug = car_handle == 0xFFFFFFFF;
		hide_hud = false;
		increase_fov = false;
		decrease_fov = false;

		if (in_debug)
		{
			// disable closing debug cam with backslash
			PatchNOP(0x47C585, 7);

			// stop overwriting our fov
			Patch<uint8_t>(0x4938B8, 0xEB);

			// disable teleportation
			Patch<uintptr_t>(0x493294, 0x49321D);
		}

		return FEPhotoModeStateManager_Constructor(this_, edx, parent, car_handle);
	}

	void __fastcall FEPhotoModeStateManager_Destructor_(void* this_)
	{
		if (in_debug)
		{
			Unpatch(0x493294);

			Unpatch(0x4938B8);

			Unpatch(0x47C585);
		}

		in_debug = false;

		FEPhotoModeStateManager_Destructor(this_);
	}

	void __fastcall FEPhotoModeStateManager_HandleLTrigger_(void* this_)
	{
		if (in_debug)
		{
			increase_fov = true;
			return;
		}

		FEPhotoModeStateManager_HandleLTrigger(this_);
	}

	void __fastcall FEPhotoModeStateManager_HandleRTrigger_(void* this_)
	{
		if (in_debug)
		{
			decrease_fov = true;
			return;
		}

		FEPhotoModeStateManager_HandleRTrigger(this_);
	}

	void __fastcall FEPhotoModeStateManager_HandleLTriggerRelease_(void* this_)
	{
		if (in_debug)
		{
			increase_fov = false;
			return;
		}

		FEPhotoModeStateManager_HandleLTriggerRelease(this_);
	}

	void __fastcall FEPhotoModeStateManager_HandleRTriggerRelease_(void* this_)
	{
		if (in_debug)
		{
			decrease_fov = false;
			return;
		}

		FEPhotoModeStateManager_HandleRTriggerRelease(this_);
	}

	void __fastcall FEPhotoModeStateManager_HandlePadButton5_(void* this_)
	{
		hide_hud = !hide_hud;

		auto package = NFSC::BulbToys_FindPackage("FePhotoMode.fng");
		auto group = NFSC::FEPackage_FindObjectByHash(package, 0xEB190B6D);
		auto script = NFSC::FEObject_FindScript(group, (hide_hud ? 0x16A259 : 0x1CA7C0));
		NFSC::FEObject_SetScript(group, script, 0);
	}
}

MODULE_NO_PANEL(photo_mode);
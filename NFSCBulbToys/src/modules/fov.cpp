#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace fov
{
	constexpr uintptr_t player = 0xB1D520;
	inline int player_fov = 0;
	inline bool player_override = false;

	constexpr uintptr_t rvm = 0xB1EE00;
	inline int rvm_fov = 0;
	inline bool rvm_override = false;

	constexpr uintptr_t pip = 0xAB0C90;
	inline int pip_fov = 0;
	inline bool pip_override = false;

	class FOVPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("FOV"))
			{
				// Player FOV
				ImGui::Checkbox("Player FOV:", &fov::player_override);
				ImGui::BeginDisabled(!fov::player_override);
				ImGui::SliderInt("##PFOV", &fov::player_fov, 0, 65535);
				ImGui::EndDisabled();

				// RVM FOV
				ImGui::Checkbox("RVM FOV:", &fov::rvm_override);
				ImGui::BeginDisabled(!fov::rvm_override);
				ImGui::SliderInt("##RVMFOV", &fov::rvm_fov, 0, 65535);
				ImGui::EndDisabled();

				// PIP FOV
				ImGui::Checkbox("PIP FOV:", &fov::pip_override);
				ImGui::BeginDisabled(!fov::pip_override);
				ImGui::SliderInt("##PIPFOV", &fov::pip_fov, 0, 65535);
				ImGui::EndDisabled();
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new FOVPanel();
		}

		return nullptr;
	}

	HOOK(0x4822F0, void, __fastcall, Camera_SetCameraMatrix, uintptr_t camera, uintptr_t edx, void* matrix4, float dt);

	void Init()
	{
		CREATE_HOOK(Camera_SetCameraMatrix);
	}

	void End()
	{
		Hooks::Destroy(0x4822F0);
	}

	void __fastcall Camera_SetCameraMatrix_(uintptr_t camera, uintptr_t edx, void* matrix4, float dt)
	{
		uintptr_t fov = camera + 0xE4;
		if (camera == fov::player)
		{
			if (fov::player_override)
			{
				Write<int>(fov, fov::player_fov);
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
			}
			else
			{
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
				fov::player_fov = Read<int>(fov);
			}
		}
		else if (camera == fov::rvm)
		{
			if (fov::rvm_override)
			{
				Write<int>(fov, fov::rvm_fov);
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
			}
			else
			{
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
				fov::rvm_fov = Read<int>(fov);
			}
		}
		else if (camera == fov::pip)
		{
			if (fov::pip_override)
			{
				Write<int>(fov, fov::pip_fov);
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
			}
			else
			{
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);
				fov::pip_fov = Read<int>(fov);
			}
		}
		else
		{
			Camera_SetCameraMatrix(camera, edx, matrix4, dt);
		}
	}
}

MODULE(fov);
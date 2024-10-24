#include "../../core/bulbtoys.h"
#include "../../core/bulbtoys/io.h"
#include "../nfsc.h"

namespace camera
{
	struct CustomCamera
	{
		const char* const name = "";
		uintptr_t address = 0;
		int eview_id = -1;

		int fov = 0;
		bool override_fov = false;

		int roll = 0;
		bool override_roll = false;

		CustomCamera(const char* const name, uintptr_t address, int eview_id) : name(name), address(address), eview_id(eview_id) {}
	};

	CustomCamera custom_cameras[]
	{
		{ "Player", 0xB1D520, 1 },
		{ "RVM", 0xB1EE00, 3 },
		{ "PIP", 0xAB0C90, 8 }
	};

	class CameraPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Camera"))
			{
				char buffer[32] { 0 };
				uint32_t id = 0;

				for (int i = 0; i < IM_ARRAYSIZE(custom_cameras); i++)
				{
					if (i > 0)
					{
						ImGui::Separator();
					}

					auto& camera = custom_cameras[i];

					ImGui::Text("Camera %d: %s", i, camera.name);

					MYPRINTF(buffer, IM_ARRAYSIZE(buffer), "FOV:" "##Cam%u", id++);
					ImGui::Checkbox(buffer, &camera.override_fov);

					ImGui::BeginDisabled(!camera.override_fov);
					MYPRINTF(buffer, IM_ARRAYSIZE(buffer), "##Cam%u", id++);
					ImGui::SliderInt(buffer, &camera.fov, 0, 65535);
					ImGui::EndDisabled();

					MYPRINTF(buffer, IM_ARRAYSIZE(buffer), "Roll:" "##Cam%u", id++);
					ImGui::Checkbox(buffer, &camera.override_roll);

					ImGui::BeginDisabled(!camera.override_roll);
					MYPRINTF(buffer, IM_ARRAYSIZE(buffer), "##Cam%u", id++);
					ImGui::SliderInt(buffer, &camera.roll, 0, 65535);
					ImGui::EndDisabled();
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new CameraPanel();
		}

		return nullptr;
	}

	HOOK(0x4822F0, void, __fastcall, Camera_SetCameraMatrix, uintptr_t camera, uintptr_t edx, NFSC::Matrix4* matrix4, float dt);

	void __declspec(noinline) DebugCam()
	{
		if (!NFSC::BulbToys_IsNFSCO())
		{
			NFSC::CameraAI_SetAction(1, "CDActionDebug");
		}
	}
	IO::Hotkey<"DebugCam", VK_BACK>* debug_cam = nullptr;

	void Init()
	{
		CREATE_HOOK(Camera_SetCameraMatrix);

		debug_cam = new IO::Hotkey<"DebugCam", VK_BACK> { DebugCam };
	}

	void End()
	{
		delete debug_cam;

		Hooks::Destroy(0x4822F0);
	}

	void __fastcall Camera_SetCameraMatrix_(uintptr_t camera, uintptr_t edx, NFSC::Matrix4* matrix4, float dt)
	{
		uintptr_t camera_fov = camera + 0xE4;

		for (int i = 0; i < IM_ARRAYSIZE(custom_cameras); i++)
		{
			auto& custom_camera = custom_cameras[i];

			if (camera == custom_camera.address)
			{
				// first handle custom camera overrides
				if (custom_camera.override_fov)
				{
					Write<int>(camera_fov, custom_camera.fov);
				}
				if (custom_camera.override_roll)
				{
					// eRotateZ
					reinterpret_cast<void(*)(void*, void*, uint16_t)>(0x70E460)(matrix4, matrix4, custom_camera.roll);
				}

				// then call the original function
				Camera_SetCameraMatrix(camera, edx, matrix4, dt);

				// finally, if we chose not to override something, copy the data for it instead
				if (!custom_camera.override_fov)
				{
					custom_camera.fov = Read<int>(camera_fov);
				}
				if (!custom_camera.override_roll)
				{
					// close enough
					custom_camera.roll = 0;
				}

				return;
			}
		}

		// no custom camera logic implemented for this camera, just call the original
		Camera_SetCameraMatrix(camera, edx, matrix4, dt);
	}
}

MODULE(camera);
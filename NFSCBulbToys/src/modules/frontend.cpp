#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace frontend
{
	class FrontendPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Frontend"))
			{
				// DebugCam
				if (ImGui::Button("DebugCam"))
				{
					NFSC::CameraAI_SetAction(1, "CDActionDebug");
				}

				// Jump to Safe House
				if (ImGui::Button("Jump to Safe House") && NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
				{
					ImGui::End();
					reinterpret_cast<void(*)()>(0x64B800)();
					return false;
				}

				ImGui::Separator();

				// DebugCarCustomize
				ImGui::Checkbox("DebugCarCustomize", reinterpret_cast<bool*>(0xA9E680));

				// ShowAllPresetsInFE
				ImGui::Checkbox("ShowAllPresetsInFE", reinterpret_cast<bool*>(0xA9E6C3));

				// Stop on Alt-Tab
				static bool stop = true;
				if (ImGui::Checkbox("Stop on Alt-Tab", &stop))
				{
					if (stop)
					{
						Unpatch(0x711F0A, true);
					}
					else
					{
						PatchNOP(0x711F0A, 8);
					}
				}

				// UnlockAll
				ImGui::Checkbox("UnlockAll", reinterpret_cast<bool*>(0xA9E6C0));

				/*
				* TODO wrong warp fix
				* TODO gps only
				// Wrong warp fix
				ImGui::Checkbox("Wrong warp fix", &g::wrong_warp::fixed);

				// World map GPS only
				if (ImGui::Checkbox("GPS only", &g::gps_only::enabled))
				{
					if (g::gps_only::enabled)
					{
						// Remove the "Jump to Safehouse" button from the pause menu
						Patch<uint8_t>(0x5D59F4, 0xEB);
					}
					else
					{
						Unpatch(0x5D59F4);
					}
				}
				*/

				ImGui::Separator();

				// Replace GPS Model
				static bool replace = false;
				static char gps_model[32] = { 0 };
				ImGui::InputText("##GPSModel", gps_model, IM_ARRAYSIZE(gps_model));
				if (ImGui::Checkbox("Replace GPS model", &replace))
				{
					if (replace)
					{
						Patch<char*>(0x41E476, gps_model);
					}
					else
					{
						Unpatch(0x41E476);
					}
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new FrontendPanel();
		}

		return nullptr;
	}
}

MODULE_PANEL_ONLY(frontend);
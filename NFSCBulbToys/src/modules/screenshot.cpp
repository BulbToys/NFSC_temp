#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace screenshot
{
	bool no_fe = false;

	class ScreenshotOverlay : public IPanel
	{
		virtual bool Draw() override final
		{
			//Write<int>(0xA5E20C, 1);
			return true;
		}
	};

	class ScreenshotPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Screenshot"))
			{
				if (ImGui::Button("Screenshot Everything"))
				{
					const char* target_ids[]
					{
						"VT",
						"PLAYER",
						"REFLECTION",
						"FLAILER",
						"RVM",
						"SHADOWMAP",
						"PIP",
						"MOTION_BLUR",
						"ENV_XPOS",
						"ENV_XNEG",
						"ENV_YPOS",
						"ENV_YNEG",
						"ENV_ZPOS",
						"ENV_ZNEG",
					};

					const char* view_ids[]
					{
						"FLAILER",
						"PLAYER1",
						"PLAYER2",
						"PLAYER1_RVM",
						"PLAYER1_REFLECTION",
						"PLAYER2_REFLECTION",
						"PLAYER1_GLOW",
						"PLAYER2_GLOW",
						"PLAYER1_PIP",
						"HEADLIGHT_P1",
						"HEADLIGHT_P2",
						"QUADRANT_TOP_LEFT",
						"QUADRANT_TOP_RIGHT",
						"QUADRANT_BOTTOM_LEFT",
						"QUADRANT_BOTTOM_RIGHT",
						"HDR_SCENE",
						"SHADOWMAP1",
						"SHADOWMAP2",
						"SHADOWPROJ1",
						"SHADOWPROJ2",
						"LIGHTSTREAKS",
						"SHADOWMATTE",
						"ENV_ZPOS",
						"ENV_XPOS",
						"ENV_ZNEG",
						"ENV_XNEG",
						"ENV_YPOS",
						"ENV_YNEG",
						"COUNT"
					};

					struct eRenderTarget
					{
						int TargetID;
						int ViewID;
						void* D3DTarget;
						void* D3DDepthStencil;
						int Active;
						int ResolutionX;
						int ResolutionY;
					} *TheRenderTargets = reinterpret_cast<eRenderTarget*>(0xAB04D0);

					for (int i = 0; i < 14; i++)
					{
						eRenderTarget rt = TheRenderTargets[i];
						if (!rt.D3DTarget)
						{
							continue;
						}

						char name[64];
						sprintf_s(name, 64, "%d_t%s_v%s_%dx%d_%s.jpg", i, target_ids[rt.TargetID], view_ids[rt.ViewID], rt.ResolutionX, rt.ResolutionY,
							(rt.Active ? "active" : "inactive"));
						reinterpret_cast<HRESULT(__stdcall*)(const char*, int, void*, PALETTEENTRY*, RECT*)>(0x86B2C0)(name, 1, rt.D3DTarget, 0, 0);
					}
				}

				if (ImGui::Button("Take SS w/o FE"))
				{
					Write<int>(0xAB0AE0, 1); // takess
					Write<int>(0xA5E20C, 0); // drawfeng
					//Error("0");
				}
				ImGui::SameLine();
				ImGui::Checkbox("Automatic##SSwoFE", &no_fe);
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new ScreenshotPanel();
		}

		if (dt == Module::DrawType::Overlay)
		{
			return new ScreenshotOverlay();
		}

		return nullptr;
	}

	HOOK(0x710750, void, __cdecl, TakeScreenshot, char* path);

	void TakeScreenshot_(char* path)
	{
		TakeScreenshot(path);
		Write<int>(0xA5E20C, 1); // drawfeng
		//Error("1");
	}

	void __fastcall GameDevice_PollDevice(uintptr_t game_device)
	{
		reinterpret_cast<void(__thiscall*)(uintptr_t)>(0x692240)(game_device);

		if (Read<int>(0xAB0AE0) && no_fe) // takess
		{
			//Error("0");
			Write<int>(0xA5E20C, 0); // drawfeng
		}
	}

	void Init()
	{
		Patch<void*>(0x9E2148, GameDevice_PollDevice);

		CREATE_HOOK(TakeScreenshot);
	}

	void End()
	{
		Hooks::Destroy(0x710750);

		Unpatch(0x9E2148);
	}
}

MODULE(screenshot);
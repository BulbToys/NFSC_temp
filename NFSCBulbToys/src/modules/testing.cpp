#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace testing
{
	class TestingPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Test Default Controls"))
			{
				struct DefaultControl
				{
					const char* mName;
					unsigned int mHasMainBind;
					unsigned int mMainBind;
					unsigned int mUnknown00;
					unsigned int mHasSecondaryBind;
					unsigned int mSecondaryBind;
					unsigned int mIndex;
					unsigned int mUnknown01;
					unsigned int mUnknown02;
					unsigned int mUnknown03;
					unsigned int mUnknown04;
					unsigned int mUnknown05;
					unsigned int mUnknown06;
				};

				auto controls = reinterpret_cast<DefaultControl*>(0xA61C88);
				char buffer[32];
				
				for (int i = 0; i < 72; i++)
				{
					auto& control = controls[i];

					ImGui::Text("%2d: %s", i + 1, control.mName);

					if (control.mHasMainBind)
					{
						GetKeyNameTextA(control.mMainBind << 16, buffer, 32);

						if (strlen(buffer) > 0)
						{
							ImGui::SameLine();
							ImGui::Text("(%s = %d)", buffer, control.mMainBind);
						}
						else
						{
							ImGui::SameLine();
							ImGui::Text("(%d)", control.mMainBind);
						}
					}

					if (control.mHasSecondaryBind)
					{
						GetKeyNameTextA(control.mSecondaryBind << 16, buffer, 32);

						if (strlen(buffer) > 0)
						{
							ImGui::SameLine();
							ImGui::Text("[%s = %d]", buffer, control.mSecondaryBind);
						}
						else
						{
							ImGui::SameLine();
							ImGui::Text("[%d]", control.mSecondaryBind);
						}
					}
				}
			}

			if (ImGui::BulbToys_Menu("Testing"))
			{
				// Replace Statistics with Template
				static bool replace = false;
				if (ImGui::Checkbox("Replace Statistics with Template", &replace))
				{
					if (replace)
					{
						Patch<uintptr_t>(0x867524, 0x8674B7);

						// "ICON SCROLLER(DEBUG)"
						Patch<uint32_t>(0x83C689, 0x7EF40CB4);
					}
					else
					{
						Unpatch(0x867524);
						Unpatch(0x83C689);
					}
				}

				ImGui::Separator();

				// Current error
				uintptr_t error_ptr = Read<uintptr_t>(0xA97B70) + 0x18;
				static int error = 0;
				if (error_ptr != 0x18)
				{
					error = Read<int>(error_ptr);
				}
				ImGui::Text("Current error: %d", error);

				// Error1 & Error2
				if (ImGui::Button("Error1"))
				{
					Write<int>(error_ptr, 1);
				}
				ImGui::SameLine();
				if (ImGui::Button("Error2"))
				{
					Write<int>(error_ptr, 2);
				}

				// AdvancedError
				if (ImGui::Button("AdvancedError"))
				{
					// todo
				}

				ImGui::Separator();

				// Chyron Message
				static char msg[128];
				ImGui::InputText("##CHYmsg", msg, IM_ARRAYSIZE(msg));

				// List all icons
				int icon = 0;

				constexpr int size = IM_ARRAYSIZE(NFSC::chyron_icons);
				static bool icons[size]{ false };
				for (int i = 0; i < IM_ARRAYSIZE(NFSC::chyron_icons); i++)
				{
					int value = (1 << i);

					ImGui::Checkbox(NFSC::chyron_icons[i], &icons[i]);
					ImGui::SameLine();
					ImGui::Text("(%d)", value);

					if (icons[i])
					{
						icon += value;
					}
				}

				// Icon
				ImGui::Text("Icon: %d", icon);

				// Linger
				static bool linger = false;
				ImGui::Checkbox("Linger", &linger);

				// Chyron Test
				if (ImGui::Button("Chyron Test"))
				{
					NFSC::FE_ShowChyron(msg, icon, linger);
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new TestingPanel();
		}

		return nullptr;
	}

	void TestBind()
	{
		Error("meow :3");
	}

	void Init()
	{
		Patch<uintptr_t>(0x59FFBD, reinterpret_cast<uintptr_t>(TestBind) - 0x59FFC1);
	}

	void End()
	{
		Unpatch(0x59FFBD);
	}
}

MODULE(testing);
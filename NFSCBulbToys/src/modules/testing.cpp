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

			if (ImGui::BulbToys_Menu("Test Drift Scores"))
			{
				static bool cs_scoring = false;
				if (ImGui::Checkbox("Force CS scoring", &cs_scoring))
				{
					if (cs_scoring)
					{
						Write<uint16_t>(0x6BAE4B, 0x9090);
					}
					else
					{
						Write<uint16_t>(0x6BAE4B, 0x0A74);
					}
				}

				struct PointsVector
				{
					int* begin;
					int* end;
				};

				auto gracedb = Read<uintptr_t>(0xA9828C);
				if (gracedb)
				{
					auto drift_scoring = Read<uintptr_t>(gracedb + 0x44);

					if (drift_scoring)
					{
						int sections = Read<int>(drift_scoring + 0x90);
						ImGui::Text("Sections: %d", sections);

						if (ImGui::Button("Reset Sections"))
						{
							auto section_map = drift_scoring + 0x7C;

							/*
								DoNukeSubtree(&this->mSectionMap.mAnchor.mpNodeParent->mpNodeRight);
								this->mSectionMap.mAnchor.mpNodeLeft = &this->mSectionMap.mAnchor;
								this->mSectionMap.mAnchor.mpNodeParent = 0;
								this->mSectionMap.mAnchor.mColor = 0;
								this->mSectionMap.mnSize = 0;
								this->mSectionMap.mAnchor.mpNodeRight = &this->mSectionMap.mAnchor;
							*/

							reinterpret_cast<void(__stdcall*)(uintptr_t)>(0x6ACE50)(Read<uintptr_t>(section_map + 0xC));
							Write<uintptr_t>(section_map + 0x8, section_map + 0x4);
							Write<uintptr_t>(section_map + 0xC, 0);
							Write<uint8_t>(section_map + 0x10, 0);
							Write<uint32_t>(section_map + 0x14, 0);
							Write<uintptr_t>(section_map + 0x4, section_map + 0x4);
						}

						PointsVector pts[3];

						for (int i = 0; i < 3; i++)
						{
							int racer_index = i + 2;
							pts[i] = *reinterpret_cast<PointsVector * (__thiscall*)(uintptr_t, int*)>(0x6BA960)(drift_scoring + 0x98, &racer_index);
						}

						if (ImGui::BeginTable("ScoresTable", 4, ImGuiTableFlags_SizingFixedFit))
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Seg");

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("Racer 1");

							ImGui::TableSetColumnIndex(2);
							ImGui::Text("Racer 2");

							ImGui::TableSetColumnIndex(3);
							ImGui::Text("Racer 3");

							bool more[3] = { true };
							for (int row = 0; more[0] || more[1] || more[2]; row++)
							{
								ImGui::TableNextRow();

								for (int column = 0; column < 4; column++)
								{
									ImGui::TableSetColumnIndex(column);

									if (column == 0)
									{
										ImGui::Text("%d", row + 1);
									}

									if (column == 1)
									{
										if (pts[0].begin == pts[0].end)
										{
											ImGui::Text("X");
											more[0] = false;
										}
										else
										{
											auto points = *pts[0].begin;
											ImGui::Text("%d", points);
											pts[0].begin++;
										}
									}

									if (column == 2)
									{
										if (pts[1].begin == pts[1].end)
										{
											ImGui::Text("X");
											more[1] = false;
										}
										else
										{
											auto points = *pts[1].begin;
											ImGui::Text("%d", points);
											pts[1].begin++;
										}
									}

									if (column == 3)
									{
										if (pts[2].begin == pts[2].end)
										{
											ImGui::Text("X");
											more[2] = false;
										}
										else
										{
											auto points = *pts[2].begin;
											ImGui::Text("%d", points);
											pts[2].begin++;
										}
									}
								}
							}

							ImGui::EndTable();
						}
					}
				}
			}

			if (ImGui::BulbToys_Menu("Test Packages"))
			{
				static char* packages[115]{ nullptr };
				if (!packages[0])
				{
					for (int i = 0; i < 115; i++)
					{
						packages[i] = *reinterpret_cast<char**>(0xA5E230 + 0x8 * i);
					}
				}

				static int package = 0;
				ImGui::BulbToys_ListBox("Packages:", "##TestPackages", &package, packages, 115);

				static char package_status[128] = "Awaiting package push...";

				if (ImGui::Button("Push"))
				{
					if (NFSC::cFEng_PushNoControlPackage(NFSC::BulbToys_GetCFEng(), packages[package], 169))
					{
						MYPRINTF(package_status, 128, "Package '%s' was pushed.", packages[package]);
					}
					else
					{
						MYPRINTF(package_status, 128, "Package '%s' was NOT pushed!", packages[package]);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Pop"))
				{
					if (NFSC::cFEng_PopNoControlPackage(NFSC::BulbToys_GetCFEng(), packages[package]))
					{
						MYPRINTF(package_status, 128, "Package '%s' was popped.", packages[package]);
					}
					else
					{
						MYPRINTF(package_status, 128, "Package '%s' was NOT popped!", packages[package]);
					}
				}

				ImGui::Text(package_status);
			}

			if (ImGui::BulbToys_Menu("Testing"))
			{
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

				uintptr_t error_ptr = Read<uintptr_t>(0xA97B70) + 0x18;
				static int error = 0;
				if (error_ptr != 0x18)
				{
					error = Read<int>(error_ptr);
				}
				ImGui::Text("Current error: %d", error);

				if (ImGui::Button("Error1"))
				{
					Write<int>(error_ptr, 1);
				}
				ImGui::SameLine();
				if (ImGui::Button("Error2"))
				{
					Write<int>(error_ptr, 2);
				}

				if (ImGui::Button("AdvancedError"))
				{
					// todo
				}

				ImGui::Separator();

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

				ImGui::Text("Icon: %d", icon);

				static bool linger = false;
				ImGui::Checkbox("Linger", &linger);

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
}

MODULE_PANEL_ONLY(testing);
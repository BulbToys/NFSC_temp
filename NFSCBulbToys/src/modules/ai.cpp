#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace ai
{
	namespace encounter
	{
		inline char vehicle[32] = { 0 };
		inline bool overridden = false;
	}

	namespace needs_encounter
	{
		inline bool value = false;
		inline bool overridden = false;
	}

	namespace needs_traffic
	{
		inline bool value = false;
		inline bool overridden = false;
	}

	namespace pursue_racers
	{
		inline bool value = false;
		inline bool overridden = false;
	}

	class AIPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("AI"))
			{
				// Vehicle name
				ImGui::Checkbox("Next encounter vehicle:", &ai::encounter::overridden);
				ImGui::InputText("##NEVehicle", ai::encounter::vehicle, IM_ARRAYSIZE(ai::encounter::vehicle));

				ImGui::Separator();

				// Traffic crash speed
				ImGui::BulbToys_SliderFloat("Traffic crash speed:", "##TCSpeed", reinterpret_cast<float*>(0x9C1790), 1.0, 1000.0);

				// Traffic type
				static int traffic_type = static_cast<int>(NFSC::AIGoal::TRAFFIC);
				if (ImGui::BulbToys_ListBox("Traffic type:", "##TType", &traffic_type, NFSC::ai_goals, IM_ARRAYSIZE(NFSC::ai_goals)))
				{
					Write<const char*>(0x419738, NFSC::ai_goals[traffic_type]);
				}

				ImGui::Separator();

				// Racer post-race type
				static int racer_postrace_type = static_cast<int>(NFSC::AIGoal::RACER);
				if (ImGui::BulbToys_ListBox("Racer post-race type:", "##RPRType", &racer_postrace_type, NFSC::ai_goals, IM_ARRAYSIZE(NFSC::ai_goals)))
				{
					Write<const char*>(0x4292D0, NFSC::ai_goals[racer_postrace_type]);
				}

				ImGui::Separator();

				// Boss override
				const char* bosses[] = { "None", "Angie", "Darius", "Wolf", "Kenji", "Neville" };
				static int boss_override = 0;
				if (ImGui::BulbToys_ListBox("Boss override:", "##BOverride", &boss_override, bosses, IM_ARRAYSIZE(bosses)))
				{
					Write<int>(0xA9E66C, boss_override);
				}

				ImGui::Separator();

				// Override NeedsEncounter
				ImGui::Checkbox("Override NeedsEncounter:", &ai::needs_encounter::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##NEValue", &ai::needs_encounter::value);

				// Override NeedsTraffic
				ImGui::Checkbox("Override NeedsTraffic:", &ai::needs_traffic::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##NTValue", &ai::needs_traffic::value);

				ImGui::Separator();

				// Override PursueRacers
				ImGui::Checkbox("Override PursueRacers:", &ai::pursue_racers::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##PRValue", &ai::pursue_racers::value);

				// Disable cops
				ImGui::Checkbox("Disable cops", reinterpret_cast<bool*>(0xA83A50));

				// Disable busting
				static bool no_busted = false;
				if (ImGui::Checkbox("Disable busting", &no_busted))
				{
					if (no_busted)
					{
						/*
							xor     eax, eax
							mov     [esi + 0x134], eax (mBustedIncrement = 0)
							mov     [esi + 0x138], eax (mBustedHUDTime = 0)
						*/
						PatchArray(0x449836, { 0x31, 0xC0, 0x89, 0x86, 0x34, 0x01, 0x00, 0x00, 0x89, 0x86, 0x38, 0x01, 0x00, 0x00, 0x90, 0x90,
							0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
							0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
							0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
					}
					else
					{
						Unpatch(0x449836);
					}
				}

				// Heat level
				static float heat_level = 1;
				if (my_simable)
				{
					uintptr_t my_perp = NFSC::BulbToys_FindInterface<0x4061D0>(my_simable);
					heat_level = reinterpret_cast<float(__thiscall*)(uintptr_t)>(0x40AF00)(my_perp);
				}
				if (ImGui::BulbToys_SliderFloat("Heat level:", "##HLevel", &heat_level, 1.f, 10.f))
				{
					reinterpret_cast<void(*)(float)>(0x65C550)(heat_level);
				}

				// Start pursuit
				if (ImGui::Button("Start pursuit"))
				{
					reinterpret_cast<void(*)(int)>(0x651430)(1);
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new AIPanel();
		}

		return nullptr;
	}


	void Init()
	{
		// Optionally override encounter spawn requirement
		CREATE_HOOK(AITrafficManager_NeedsEncounter);

		// Optionally override traffic spawn requirement
		CREATE_HOOK(AITrafficManager_NeedsTraffic);

		// Optionally override whether racers should be pursued or not
		CREATE_VTABLE_PATCH(0x9C3B58, AICopManager_CanPursueRacers);

		// Custom encounter vehicles
		CREATE_HOOK(AITrafficManager_GetAvailablePresetVehicle);
	}

	void End()
	{
		Hooks::Destroy();
	}

}

MODULE(ai);
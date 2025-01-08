#include "../../core/bulbtoys.h"
#include "../nfsc.h"

#include <random>

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
				ImGui::Checkbox("Override NeedsEncounter:", &ai::needs_encounter::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##AIOverrideNeedsEncounter", &ai::needs_encounter::value);

				ImGui::Checkbox("Force next encounter vehicle:", &ai::encounter::overridden);
				ImGui::InputText("##NEVehicle", ai::encounter::vehicle, IM_ARRAYSIZE(ai::encounter::vehicle));

				ImGui::Separator();

				ImGui::Checkbox("Override NeedsTraffic:", &ai::needs_traffic::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##AIOverrideNeedsTraffic", &ai::needs_traffic::value);

				ImGui::Text("Traffic crash speed:");
				ImGui::SliderFloat("##AITrafficCrashSpeed", reinterpret_cast<float*>(0x9C1790), 1.0, 1000.0);

				static int traffic_type = static_cast<int>(NFSC::AIGoal::TRAFFIC);
				ImGui::Text("Traffic type:");
				if (ImGui::ListBox("##AITrafficType", &traffic_type, NFSC::ai_goals, IM_ARRAYSIZE(NFSC::ai_goals)))
				{
					Write<const char*>(0x419738, NFSC::ai_goals[traffic_type]);
				}

				ImGui::Separator();

				const char* bosses[] = { "None", "Angie", "Darius", "Wolf", "Kenji", "Neville" };
				static int boss_override = 0;
				ImGui::Text("Race boss override:");
				if (ImGui::ListBox("##AIBossOverride", &boss_override, bosses, IM_ARRAYSIZE(bosses)))
				{
					Write<int>(0xA9E66C, boss_override);
				}

				static int racer_postrace_type = static_cast<int>(NFSC::AIGoal::RACER);
				ImGui::Text("Racer post-race type:");
				if (ImGui::ListBox("##AIRacerPostRaceType", &racer_postrace_type, NFSC::ai_goals, IM_ARRAYSIZE(NFSC::ai_goals)))
				{
					Write<const char*>(0x4292D0, NFSC::ai_goals[racer_postrace_type]);
				}

				ImGui::Separator();

				ImGui::Checkbox("Override PursueRacers:", &ai::pursue_racers::overridden);
				ImGui::SameLine();
				ImGui::Checkbox("##AIOverridePursueRacers", &ai::pursue_racers::value);

				ImGui::Checkbox("Disable cops", reinterpret_cast<bool*>(0xA83A50));

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

				static float heat_level = 1;
				uintptr_t my_simable = 0;
				if (NFSC::BulbToys_GetMyVehicle(nullptr, &my_simable))
				{
					uintptr_t my_perp = NFSC::BulbToys_FindInterface<0x4061D0>(my_simable);
					heat_level = reinterpret_cast<float(__thiscall*)(uintptr_t)>(0x40AF00)(my_perp);
				}
				ImGui::Text("Heat level:");
				if (ImGui::SliderFloat("##AIHeatLevel", &heat_level, 1.f, 10.f))
				{
					// Game_SetWorldHeat
					reinterpret_cast<void(*)(float)>(0x65C550)(heat_level);
				}

				if (ImGui::Button("Start pursuit"))
				{
					// Game_ForcePursuitStart
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

	// Optionally override encounter spawn requirement
	HOOK(0x422BF0, bool, __fastcall, AITrafficManager_NeedsEncounter, uintptr_t traffic_manager);

	// Optionally override traffic spawn requirement
	HOOK(0x422990, bool, __fastcall, AITrafficManager_NeedsTraffic, uintptr_t traffic_manager);

	// Optionally override whether racers should be pursued or not
	HOOK(VIRTUAL, bool, __fastcall, AICopManager_CanPursueRacers, uintptr_t cop_manager);

	// Custom encounter vehicles
	HOOK(0x42CB70, uintptr_t, __fastcall, AITrafficManager_GetAvailablePresetVehicle, uintptr_t ai_traffic_manager, uintptr_t edx, uint32_t skin_key, uint32_t encounter_key);

	// Random vehicle tier - causes sound crashes!
	//HOOK(0x618220, int, __fastcall, GCareer_GetOpponentVehicleTier, uintptr_t g_career, void* edx, int tier);

	void Init()
	{
		CREATE_HOOK(AITrafficManager_NeedsEncounter);

		CREATE_HOOK(AITrafficManager_NeedsTraffic);

		CREATE_VTABLE_PATCH(0x9C3B58, AICopManager_CanPursueRacers);

		CREATE_HOOK(AITrafficManager_GetAvailablePresetVehicle);

		//CREATE_HOOK(GCareer_GetOpponentVehicleTier);
	}

	void End()
	{
		//Hooks::Destroy(0x618220);

		Hooks::Destroy(0x42CB70);

		Unpatch(0x9C3B58);

		Hooks::Destroy(0x422990);

		Hooks::Destroy(0x422BF0);
	}

	/*
	int __fastcall GCareer_GetOpponentVehicleTier_(uintptr_t g_career, void* edx, int tier)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 3);

		return dist6(rng);
	}
	*/

	bool __fastcall AITrafficManager_NeedsEncounter_(uintptr_t traffic_manager)
	{
		// If we've overridden the value, use our own. Instead, let the game decide normally
		return ai::needs_encounter::overridden ? ai::needs_encounter::value : AITrafficManager_NeedsEncounter(traffic_manager);
	}

	bool __fastcall AITrafficManager_NeedsTraffic_(uintptr_t traffic_manager)
	{
		// Ditto
		return ai::needs_traffic::overridden ? ai::needs_traffic::value : AITrafficManager_NeedsTraffic(traffic_manager);
	}

	bool __fastcall AICopManager_CanPursueRacers_(uintptr_t ai_cop_manager)
	{
		// Ditto
		return ai::pursue_racers::overridden ? ai::pursue_racers::value : AICopManager_CanPursueRacers(ai_cop_manager);
	}

	uintptr_t __fastcall AITrafficManager_GetAvailablePresetVehicle_(uintptr_t ai_traffic_manager, uintptr_t edx, uint32_t skin_key, uint32_t encounter_key)
	{
		if (ai::encounter::overridden)
		{
			skin_key = 0;
			encounter_key = NFSC::Attrib_StringToKey(ai::encounter::vehicle);
		}

		return AITrafficManager_GetAvailablePresetVehicle(ai_traffic_manager, edx, skin_key, encounter_key);
	}
}

MODULE(ai);
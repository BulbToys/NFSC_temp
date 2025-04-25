#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace race_map
{
	bool enabled = true;
	uintptr_t flm;
	bool show_icons = true;

	void DestroyFLM()
	{
		if (race_map::flm)
		{
			reinterpret_cast<void(__thiscall*)(uintptr_t)>(0x7597C0)(race_map::flm);
			NFSC::free(race_map::flm);
			race_map::flm = 0;
		}
	}

	int GenerateFLM(float x_offs = 0, float z_offs = 0, int limit = INT_MAX)
	{
		DestroyFLM();

		int event_type = -1;
		int event_hash = -1;

		// don't make FLM if we're not in a race or if it's an encounter race
		if (!NFSC::DALManager_GetInt(NFSC::DALManager, 3021, &event_hash, -1, -1, -1) ||
			(NFSC::DALManager_GetInt(NFSC::DALManager, 3011, &event_type, -1, -1, -1) && event_type == 0xC))
		{
			return -1;
		}

		// almost everything below this point is a recreation of Minimap::UpdateRaceRoute
		// how does it work? no idea! it came to me in a dream. glhf

		/*
		// TEMP: we need the minimap
		auto minimap = Read<uintptr_t>(0xA97DE4);
		if (!minimap)
		{
			return;
		}
		*/

		float track_map_size_x = 128.0f;//Read<float>(minimap + 0xA0);

		int bezier_count = 0;

		auto segments = NFSC::WRoadNetwork::BulbToys_GetSegments();
		auto nodes = NFSC::WRoadNetwork::BulbToys_GetNodes();

		for (int i = 0; i < NFSC::WRoadNetwork::BulbToys_GetNumSegments(); i++)
		{
			auto road_segment = segments + i;

			if (road_segment &&
				((road_segment->fFlags & NFSC::WRoadSegmentFlags::IS_IN_RACE) != 0) &&
				((road_segment->fFlags & NFSC::WRoadSegmentFlags::IS_SIDE_ROUTE) == 0))
			{
				bool new_segment = true;

				if ((road_segment->fFlags & NFSC::WRoadSegmentFlags::IS_DECISION) != 0)
				{
					for (int k = 0; k < 2; k++)
					{
						auto road_node = nodes + road_segment->fNodeIndex[k];

						for (int j = 0; j < road_node->fNumSegments; j++)
						{
							uint16_t segment_index = road_node->fSegmentIndex[j];

							if (segment_index != i)
							{
								auto other_segment = segments + segment_index;
								if (((other_segment->fFlags & NFSC::WRoadSegmentFlags::IS_IN_RACE) != 0) &&
									((other_segment->fFlags & NFSC::WRoadSegmentFlags::IS_SIDE_ROUTE) != 0))
								{
									new_segment = false;
									break;
								}
							}
						}
					}
				}

				if (new_segment)
				{
					NFSC::Vector2 points[4]{ {0, 0}, {0, 0}, {0, 0}, {0, 0} };
					NFSC::Vector3 controls[2]{ {0, 0, 0}, {0, 0, 0} };
					NFSC::WRoadNode* new_nodes[2]{
						nodes + road_segment->fNodeIndex[0],
						nodes + road_segment->fNodeIndex[1]
					};

					// get start/end control
					reinterpret_cast<void(__thiscall*)(NFSC::WRoadSegment*, NFSC::Vector3*)>(0x404D80)(road_segment, &controls[0]);
					reinterpret_cast<void(__thiscall*)(NFSC::WRoadSegment*, NFSC::Vector3*)>(0x5D1CA0)(road_segment, &controls[1]);

					points[0].y = -new_nodes[0]->fPosition.x;
					points[0].x = new_nodes[0]->fPosition.z;
					points[1].y = -(controls[0].x + new_nodes[0]->fPosition.x);
					points[1].x = controls[0].z + new_nodes[0]->fPosition.z;
					points[2].y = -(controls[1].x + new_nodes[1]->fPosition.x);
					points[2].x = controls[1].z + new_nodes[1]->fPosition.z;
					points[3].y = -new_nodes[1]->fPosition.x;
					points[3].x = new_nodes[1]->fPosition.z;

					float scaled[2]{ track_map_size_x * 16.0f, track_map_size_x * -0.5f };

					for (int j = 0; j < 4; j++)
					{
						// apply our own offsets
						points[j].x += z_offs;
						points[j].y -= x_offs;

						points[j].x = (points[j].x - -7300.0f) * 0.000062500003f;
						points[j].y = (-2000.0f - points[j].y) * 0.000062500003f + 1.0f;

						points[j].x = scaled[0] * points[j].x + scaled[1];
						points[j].y = scaled[0] * points[j].y + scaled[1];

						// account for world map scale
						/*if (NFSC::BulbToys_IsNFSCO())
						{
							points[j].x -= 1247.f;
						}
						else
						{*/
							points[j].x -= 951.f;
						//}
						points[j].y -= 1411.f;

						points[j].x *= 1.27f;
						points[j].y *= 1.27f;
					}

					if (!race_map::flm)
					{
						// ctor
						race_map::flm = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, bool)>(0x759690)(NFSC::malloc(0x160), false);
					}

					/*
					Error("A: { %.2f, %.2f }\nB: { %.2f, %.2f }\nC: { %.2f, %.2f }\nC: { %.2f, %.2f }",
						points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, points[3].x, points[3].y);
					*/

					// add bezier
					reinterpret_cast<void(__thiscall*)(uintptr_t, NFSC::Vector2*, NFSC::Vector2*, NFSC::Vector2*, NFSC::Vector2*, float)>(0x7598E0)
						(race_map::flm, &points[0], &points[1], &points[2], &points[3], 0.25);

					// custom counter & limit for testing
					bezier_count++;
					if (bezier_count == limit)
					{
						break;
					}
				}
			}
		}

		if (race_map::flm)
		{
			// generate
			reinterpret_cast<void(__thiscall*)(uintptr_t, float)>(0x75BA60)(race_map::flm, 6.25f);

			// optimize
			reinterpret_cast<void(__thiscall*)(uintptr_t)>(0x74ADC0)(race_map::flm);

			// TODO: are the next 5 functions even necessary?
			// center and edge color (0xC0 - 75% opacity)
			reinterpret_cast<void(__thiscall*)(uintptr_t, uint32_t)>(0x740B00)(race_map::flm, 0xC02DC2FF);
			reinterpret_cast<void(__thiscall*)(uintptr_t, uint32_t)>(0x740B20)(race_map::flm, 0xC0030B0D);

			// alpha scale
			reinterpret_cast<void(__thiscall*)(uintptr_t, float)>(0x740BD0)(race_map::flm, 1.0f);

			// width
			reinterpret_cast<void(__thiscall*)(uintptr_t, float)>(0x740A70)(race_map::flm, 6.25f);

			// sharpness
			reinterpret_cast<void(__thiscall*)(uintptr_t, float)>(0x740AD0)(race_map::flm, 0.0f);

			NFSC::Vector2 min = { -100000, -100000 };
			NFSC::Vector2 max = { +100000, +100000 };

			// mask bbox
			reinterpret_cast<void(__thiscall*)(uintptr_t, NFSC::Vector2*, NFSC::Vector2*)>(0x740940)(race_map::flm, &min, &max);

			// tuned fx
			reinterpret_cast<void(__thiscall*)(uintptr_t, const char*)>(0x74B140)(race_map::flm, "mini_map_route");
		}

		return bezier_count;
	}

	class RaceMapPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Race Map"))
			{
				ImGui::Checkbox("Show map icons", &race_map::show_icons);

				if (ImGui::Button("Open World Map"))
				{
					reinterpret_cast<bool(__thiscall*)(void*, int, int, int, char)>(0x5A0D90)(NFSC::BulbToys_GetFEManager(), 0, 0, 0, 0);
				}

				ImGui::Separator();

				static int limit = 1000;
				ImGui::SliderInt("##BezierLimit", &limit, 1, 10000);

				static bool use_limit = false;
				ImGui::Checkbox("Bezier Limit", &use_limit);

				static float x_offs = .0f;
				ImGui::Text("X offset:");
				ImGui::SliderFloat("##FLMXOffset", &x_offs, -10000, +10000);

				static float z_offs = .0f;
				ImGui::Text("Z offset:");
				ImGui::SliderFloat("##FLMYOffset", &z_offs, -10000, +10000);

				static int bezier_count = 0;
				if (ImGui::Button("Generate FLM"))
				{
					ImGui::End();

					if (use_limit)
					{
						bezier_count = GenerateFLM(x_offs, z_offs, limit);
					}
					else
					{
						bezier_count = GenerateFLM(x_offs, z_offs);
					}

					return false;
				}
				if (race_map::flm)
				{
					ImGui::SameLine();
					if (ImGui::Button("Destroy FLM"))
					{
						DestroyFLM();
					}

					ImGui::Text("Bezier count: %d", bezier_count);
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow && race_map::enabled)
		{
			return new RaceMapPanel();
		}

		return nullptr;
	}

	HOOK(0x573D30, void, __stdcall, cFEngRender_RenderTerritoryBorder, uintptr_t feobject);

	//HOOK(0x4B57E0, bool, __fastcall, DALWorldMap_GetBool, uintptr_t dal_world_map, uintptr_t edx, int id, bool* result);

	HOOK(0x582E60, bool, __fastcall, WorldMap_IsInPursuit, uintptr_t world_map);
	HOOK(0x5AE880, void, __fastcall, WorldMap_SetupPursuit, uintptr_t world_map);

	void Init()
	{
		Settings::Bool<"RaceMap", "Enabled", true> setting;
		race_map::enabled = setting.Get();
		if (!race_map::enabled)
		{
			return;
		}

		CREATE_HOOK(cFEngRender_RenderTerritoryBorder);

		//CREATE_HOOK(DALWorldMap_GetBool);

		CREATE_HOOK(WorldMap_IsInPursuit);
		CREATE_HOOK(WorldMap_SetupPursuit);
	}

	void End()
	{
		if (!race_map::enabled)
		{
			return;
		}

		Hooks::Destroy(0x5AE880);
		Hooks::Destroy(0x582E60);

		//Hooks::Destroy(0x4B57E0);

		Hooks::Destroy(0x573D30);
	}

	void __stdcall cFEngRender_RenderTerritoryBorder_(uintptr_t object)
	{
		if (race_map::flm)
		{
			uintptr_t territory = Read<uintptr_t>(0xA977F8);

			// FatLineMesh::RenderFE(wm_flm, FEWorldMapTerritory::sInstance->views, &FEWorldMapTerritory::sInstance->matrix)
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, uintptr_t)>(0x74F0B0)(race_map::flm, Read<uintptr_t>(territory + 0xA0), territory + 0x50);
		}

		cFEngRender_RenderTerritoryBorder(object);
	}

	/*
	bool __fastcall DALWorldMap_GetBool_(uintptr_t dal_world_map, uintptr_t edx, int id, bool* result)
	{
		*result = true;
		return true;
	}
	*/

	bool __fastcall WorldMap_IsInPursuit_(uintptr_t world_map)
	{
		if (race_map::show_icons)
		{
			return WorldMap_IsInPursuit(world_map);
		}
		return true;
	}

	void __fastcall WorldMap_SetupPursuit_(uintptr_t world_map)
	{
		if (race_map::show_icons)
		{
			WorldMap_SetupPursuit(world_map);
		}
	}
}

MODULE(race_map);
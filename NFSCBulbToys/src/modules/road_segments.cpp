#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace road_segments
{
	bool overlay = false;
	bool draw_indexes = false;

	float max_distance = 512.f;
	float max = 4.f;
	float min = 0.f;

	class RoadSegmentsPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Road Segments"))
			{
				ImGui::Text("Segments: %d", NFSC::WRoadNetwork::BulbToys_GetNumSegments());
				ImGui::Checkbox("Overlay" "##RoadSegmentsOverlay", &road_segments::overlay);

				ImGui::BeginDisabled(!road_segments::overlay);

				ImGui::Separator(); 

				ImGui::Checkbox("Draw Indexes", &road_segments::draw_indexes);

				ImGui::Separator(); 

				ImGui::BulbToys_SliderFloat("Max Distance", "##RoadSegmentsMaxDistance", &road_segments::max_distance, 64, 4096);
				ImGui::BulbToys_SliderFloat("Max", "##RoadSegmentsMax", &road_segments::max, 4, 16);
				ImGui::BulbToys_SliderFloat("Min", "##RoadSegmentsMin", &road_segments::min, 0, 4);

				ImGui::EndDisabled();
			}

			return true;
		}
	};

	class RoadSegmentsOverlay : public IPanel
	{
		virtual bool Draw() override final
		{
			if (road_segments::overlay && NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
			{
				auto draw_list = ImGui::GetWindowDrawList();

				auto segments = NFSC::WRoadNetwork::BulbToys_GetSegments();
				auto nodes = NFSC::WRoadNetwork::BulbToys_GetNodes();

				for (int i = 0; i < NFSC::WRoadNetwork::BulbToys_GetNumSegments(); i++)
				{
					auto road_segment = segments + i;

					auto node_idx_1 = Read<uint16_t>(reinterpret_cast<uintptr_t>(road_segment) + 0x0);
					auto node_idx_2 = Read<uint16_t>(reinterpret_cast<uintptr_t>(road_segment) + 0x2);

					auto node_1 = nodes + node_idx_1;
					auto node_2 = nodes + node_idx_2;

					NFSC::Vector3 world[2]
					{ 
						Read<NFSC::Vector3>(reinterpret_cast<uintptr_t>(node_1) + 0x0),  
						Read<NFSC::Vector3>(reinterpret_cast<uintptr_t>(node_2) + 0x0)
					};
					NFSC::Vector3 screen[2] { 0, 0, 0 };

					NFSC::BulbToys_GetScreenPosition(world[0], screen[0]);
					NFSC::BulbToys_GetScreenPosition(world[1], screen[1]);

					if (screen[0].z < 1.0f && screen[1].z < 1.0f)
					{
						ImVec4 color(1, 1, 1, 1);

						auto width_1 = ImGui::NFSC_DistanceWidth(world[0], road_segments::max_distance, road_segments::min, road_segments::max);
						auto width_2 = ImGui::NFSC_DistanceWidth(world[1], road_segments::max_distance, road_segments::min, road_segments::max);

						auto width = (width_1 + width_2) / 2;

						if (width > road_segments::min + 0.5f)
						{
							draw_list->AddLine({ screen[0].x, screen[0].y }, { screen[1].x, screen[1].y }, ImGui::ColorConvertFloat4ToU32(color), width);

							if (road_segments::draw_indexes)
							{
								NFSC::Vector3 pos { (world[0].x + world[1].x) / 2, (world[0].y + world[1].y) / 2, ((world[0].z + world[1].z) / 2) };
								ImGui::NFSC_Overlay_Text(draw_list, pos, color, nullptr, "%d", i);
							}
						}
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
			return new RoadSegmentsPanel();
		}

		if (dt == Module::DrawType::Overlay)
		{
			return new RoadSegmentsOverlay();
		}

		return nullptr;
	}
}

MODULE_PANEL_ONLY(road_segments);
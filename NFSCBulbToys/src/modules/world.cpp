#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace world
{
	class WorldPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("World"))
			{
				// Clear skids
				if (ImGui::Button("Clear skids"))
				{
					NFSC::KillSkidsOnRaceRestart();
				}
				
				// Restore props
				if (ImGui::Button("Restore props"))
				{
					NFSC::World_RestoreProps();
				}

				ImGui::Separator();

				// Always rain
				ImGui::Checkbox("Always rain", reinterpret_cast<bool*>(0xB74D20));
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new WorldPanel();
		}

		return nullptr;
	}
}

MODULE_PANEL_ONLY(world);
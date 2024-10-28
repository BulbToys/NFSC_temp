#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace state_managers
{
	class StateManagerPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("State Managers"))
			{
				auto fesm = NFSC::BulbToys_GetFEManager();

				int i = 0;
				do
				{
					if (i)
					{
						ImGui::Separator();
					}

					ImGui::BulbToys_AddyLabel(reinterpret_cast<uintptr_t>(fesm), "%d: %s", i++, fesm->GetName());

					ImGui::BulbToys_AddyLabel(reinterpret_cast<uintptr_t>(fesm->vtable), " - VTable");

					ImGui::Text("");

					ImGui::Text(" - Entry point: %d", fesm->entry_point);
					ImGui::Text(" - Exit point: %d", fesm->exit_point);

					ImGui::Text("");

					ImGui::Text(" - Previous state: %d", fesm->previous_state);
					ImGui::Text(" - Current state: %d", fesm->current_state);
					ImGui::Text(" - Sub state: %d", fesm->sub_state);
					ImGui::Text(" - Next state: %d (%s)", fesm->next_state, fesm->next_state_valid ? "valid" : "invalid");

					ImGui::Text("");

					ImGui::Text(" - Last screen: %s", fesm->last_screen);
					ImGui::Text(" - Next screen: %s", (fesm->next_screen ? fesm->next_screen : ""));

					ImGui::Text("");

					ImGui::Text(" - Screens pushed: %d", fesm->screens_pushed);
					ImGui::Text(" - Screens to pop: %d", fesm->screens_to_pop);

					ImGui::Text("");

					ImGui::Text(" - Can skip movie: %s", fesm->can_skip_movie ? "Yes" : "No");

					ImGui::Text("");

					ImGui::BulbToys_AddyLabel(reinterpret_cast<uintptr_t>(fesm->next), " - Next manager (%s)", fesm->next ? fesm->GetName() : "N/A");

				} while (fesm = fesm->child);
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new StateManagerPanel();
		}

		return nullptr;
	}
}

MODULE_PANEL_ONLY(state_managers);
#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace fix_71E06B
{
	bool enabled = false;
	uint32_t good_reads = 0;
	uint32_t bad_reads = 0;

	class Fix71E06BPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Fix 71E06B"))
			{
				ImGui::Checkbox("Enabled", &fix_71E06B::enabled);
				
				ImGui::Text("Good reads: %u", fix_71E06B::good_reads);
				ImGui::Text("Bad  reads: %u", fix_71E06B::bad_reads);
				
				if (ImGui::Button("Reset"))
				{
					fix_71E06B::good_reads = 0;
					fix_71E06B::bad_reads = 0;
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new Fix71E06BPanel();
		}

		return nullptr;
	}

	void __fastcall eEffect_HandleMaterialData(uintptr_t e_effect, void* edx, void* material, int unused)
	{
		if (fix_71E06B::enabled && IsBadReadPtr(material, 4))
		{
			fix_71E06B::bad_reads++;
			return;
		}

		fix_71E06B::good_reads++;
		NFSC::eEffect_HandleMaterialData(e_effect, material, unused);
	}

	void Init()
	{
		Patch<void*>(0x9EACE8, eEffect_HandleMaterialData);
	}

	void End()
	{
		Unpatch(0x9EACE8);
	}
}

MODULE(fix_71E06B);
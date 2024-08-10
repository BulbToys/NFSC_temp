#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace crew
{
	class CrewPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Crew"))
			{
				ImGui::Text("Add to crew:");

				// Unlockable & Bosses + Manic
				if (ImGui::Button("Unlockable"))
				{
					uintptr_t KEY_NIKKI = 0xA982BC;
					uint32_t nikki = Read<uint32_t>(KEY_NIKKI);

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/sal"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/yumi"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/colin"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/samson"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, nikki);
					NFSC::Game_UnlockNikki();
				}
				ImGui::SameLine();
				if (ImGui::Button("Bosses + Manic"))
				{
					uintptr_t KEY_NIKKI = 0xA982BC;
					uint32_t nikki = Read<uint32_t>(KEY_NIKKI);

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/major_crew_darius/boss"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/major_crew_wolf/boss"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/major_crew_angela/boss"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/major_crew_kenji/boss"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, NFSC::Attrib_StringToKey("characters/manic"));
					NFSC::Game_UnlockNikki();

					Write<uint32_t>(KEY_NIKKI, nikki);
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new CrewPanel();
		}

		return nullptr;
	}

	__declspec(naked) void ASM_NoWingmanSound()
	{
		__asm
		{
			// Check if null, gtfo if it is to avoid a null dereference
			test    eax, eax
			jz      skip

			// Redo what we've overwritten
			mov[esi + 0xDC], eax
			push    0x79391A
			ret

			skip :
			push    0x7939AB
				ret
		}
	}

	void Init()
	{
		PatchJMP(0x793914, ASM_NoWingmanSound, 6);
	}

	void End()
	{
		Unpatch(0x793914);
	}
}

MODULE(crew);
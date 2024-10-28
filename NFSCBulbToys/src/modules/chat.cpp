#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace chat
{
	struct FEChatStateManager : NFSC::FEStateManager
	{
		static inline FEChatStateManager* instance = nullptr;
	private:
		FEChatStateManager* Construct(NFSC::FEStateManager* parent, int entry_point)
		{
			NFSC::FEStateManager_FEStateManager(this, parent, entry_point);

			this->vtable = new void*[0x11C];
			memcpy(this->vtable, reinterpret_cast<void*>(0x9D2420), 0x11C);

			this->vtable[0] = FEChatStateManager::VecDelDtor;
			this->vtable[7] = FEChatStateManager::Start;
			this->vtable[31] = FEChatStateManager::HandlePadBack;

			FEChatStateManager::instance = this;

			return this;
		}

		FEChatStateManager* Deconstruct()
		{
			FEChatStateManager::instance = nullptr;

			delete[] this->vtable;

			NFSC::FEStateManager_Destructor(this);

			return this;
		}

		static FEChatStateManager* __fastcall VecDelDtor(FEChatStateManager* this_, void* edx, uint8_t should_free)
		{
			this_->Deconstruct();
			if ((should_free & 1) != 0)
			{
				NFSC::free(reinterpret_cast<uintptr_t>(this_));
			}
			return this_;
		}

		static void __fastcall Start(uintptr_t this_)
		{
			NFSC::FEStateManager_Push(this_, "FeOnlineMessengerChatPopup.fng", 0);
		}

		static void __fastcall HandlePadBack(uintptr_t this_)
		{
			NFSC::FEStateManager_Exit(this_);
		}
	public:
		static FEChatStateManager* CreateInstance(NFSC::FEStateManager* parent, int entry_point = 0)
		{
			auto fesm = (FEChatStateManager*)NFSC::malloc(sizeof(FEChatStateManager));

			if (fesm)
			{
				return fesm->Construct(parent, entry_point);
			}
			else
			{
				return 0;
			}
		}
	};

	void OpenChat()
	{
		if (!FEChatStateManager::instance)
		{
			auto fe_manager = NFSC::BulbToys_GetFEManager();
			auto last_child = reinterpret_cast<NFSC::FEStateManager*(__thiscall*)(NFSC::FEStateManager*)>(0x579220)(fe_manager);

			auto chat = FEChatStateManager::CreateInstance(last_child, 0);
			if (chat)
			{
				reinterpret_cast<void(__thiscall*)(NFSC::FEStateManager*, NFSC::FEStateManager*)>(0x5890A0)(fe_manager, chat);
			}
		}
	}

	class ChatPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Chat"))
			{
				if (ImGui::Button("Open"))
				{
					OpenChat();
				}
			}

			return true;
		}
	};

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new ChatPanel();
		}

		return nullptr;
	}

	void Init()
	{
		Patch<uintptr_t>(0x59FFBD, reinterpret_cast<uintptr_t>(OpenChat) - 0x59FFC1);
	}

	void End()
	{
		Unpatch(0x59FFBD);
	}
}

MODULE(chat);
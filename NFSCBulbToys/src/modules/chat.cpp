#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace chat
{
	struct FEChatStateManager : NFSC::FEStateManager
	{
		static inline FEChatStateManager* instance = nullptr;

		enum
		{
			NORMAL = 0,
			TYPING = 1,
		};

	private:
		FEChatStateManager* Construct(NFSC::FEStateManager* parent, int entry_point)
		{
			NFSC::FEStateManager_FEStateManager(this, parent, entry_point);

			this->vtable = new void*[0x11C];
			memcpy(this->vtable, reinterpret_cast<void*>(0x9D2420), 0x11C);

			this->vtable[0] = FEChatStateManager::VecDelDtor;
			this->vtable[7] = FEChatStateManager::Start;
			this->vtable[30] = FEChatStateManager::HandlePadAccept;
			this->vtable[31] = FEChatStateManager::HandlePadBack;
			this->vtable[58] = FEChatStateManager::HandlePadStart;
			this->vtable[63] = FEChatStateManager::HandleScreenConstructed;

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

		static void __fastcall HandlePadAccept(FEChatStateManager* this_)
		{
			if (this_->current_state == NORMAL)
			{
				auto chat_popup = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
				if (chat_popup)
				{
					this_->previous_state = NORMAL;
					this_->current_state = TYPING;
					// HandleStateChange() - normally required, nullsub in our use case

					uintptr_t package = NFSC::BulbToys_FindPackage(reinterpret_cast<const char*>(chat_popup + 0xC));
					if (package)
					{
						uintptr_t object_by_hash = NFSC::FEPackage_FindObjectByHash(package, 0x553FD118);
						if (object_by_hash)
						{
							uintptr_t script = NFSC::FEObject_FindScript(object_by_hash, 0x1CA7C0);
							if (script)
							{
								NFSC::FEObject_SetScript(object_by_hash, script, false);

								// CurTime
								Write<int>(script + 0x10, 0);
							}
						}
					}
					NFSC::FEDialogScreen_ShowKeyboard(0xA828438D, 0, NFSC::NullPointer, NFSC::NullPointer, 0x28, 1, 0);
				}
			}
		}

		static void __fastcall HandlePadBack(FEChatStateManager* this_)
		{
			if (this_->current_state == TYPING)
			{
				this_->previous_state = TYPING;
				this_->current_state = NORMAL;
				// HandleStateChange() - normally required, nullsub in our use case

				auto chat_popup = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
				if (chat_popup)
				{
					// this->mChatPopup->PackageFilename
					NFSC::FE_String_Printf(reinterpret_cast<const char*>(chat_popup + 0xC), 0xA828438D, "");
				}
			}
			else
			{
				NFSC::FEStateManager_Exit(reinterpret_cast<uintptr_t>(this_));
			}
		}

		static void __fastcall HandlePadStart(FEChatStateManager* this_)
		{
			if (this_->current_state == TYPING)
			{
				this_->previous_state = TYPING;
				this_->current_state = NORMAL;
				// HandleStateChange() - normally required, nullsub in our use case

				uintptr_t histogram = Read<uintptr_t>(0xA97B54);
				uintptr_t conv_buf = 0xA605F4;

				// ConvertCharacters(EA::Localizer::LocalizerManager::mWideCharHistogram, TheConversionBuffer, 320, TheKeyboardBuffer);
				reinterpret_cast<bool(__thiscall*)(uintptr_t, uintptr_t, int, uintptr_t)>(0x578BB0)(histogram, conv_buf, 320, 0xA604BC);

				wchar_t txt[1300] { 0 };

				if (histogram)
				{
					reinterpret_cast<bool(__thiscall*)(uintptr_t, wchar_t*, int, uintptr_t)>(0x578CF0)(histogram, txt, 1300, conv_buf);
				}
				else
				{
					// PackedStringToWideString
					reinterpret_cast<void(*)(wchar_t*, uintptr_t)>(0x4718A0)(txt, conv_buf);
				}

				auto chat_popup = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
				if (chat_popup)
				{
					// this->mChatPopup->PackageFilename
					NFSC::FE_String_Printf(reinterpret_cast<const char*>(chat_popup + 0xC), 0xA828438D, "");

					uintptr_t text_scroller = chat_popup + 0x28;
					reinterpret_cast<void(__thiscall*)(uintptr_t, wchar_t*)>(0x5BCDF0)(text_scroller, txt);
				}
			}
		}

		static void __fastcall HandleScreenConstructed(uintptr_t this_)
		{
			// FeOnlineMessengerChatPopup::Setup
			reinterpret_cast<void(__thiscall*)(uintptr_t, int)>(0x5C8670)(NFSC::BulbToys_GetFeOnlineMessengerChatPopup(), 0);
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
				ImGui::SameLine();
				if (ImGui::Button("Leaky Close"))
				{
					FEChatStateManager::instance = nullptr;
				}

				static char chat_text[1300];
				ImGui::InputText("##ChatText", chat_text, IM_ARRAYSIZE(chat_text));
				if (ImGui::Button("Set Text"))
				{
					auto chat_fng = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
					if (chat_fng)
					{
						wchar_t chat_text_wide[1300];
						StringToWideString(chat_text, 1300, chat_text_wide, 1300);

						uintptr_t text_scroller = chat_fng + 0x28;
						reinterpret_cast<void(__thiscall*)(uintptr_t, wchar_t*)>(0x5BCDF0)(text_scroller, chat_text_wide);
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
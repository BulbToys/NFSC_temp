#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace chat
{
	wchar_t* chat = nullptr;

	void AddChat(uintptr_t text_scroller, const wchar_t* txt)
	{
		auto strlen_txt = lstrlenW(txt);
		if (chat)
		{
			auto strlen_chat = lstrlenW(chat);

			wchar_t* new_chat = new wchar_t[strlen_chat + 1 + strlen_txt + 1];
			lstrcpyW(new_chat, chat);
			lstrcpyW(new_chat + strlen_chat, L"^");
			lstrcpyW(new_chat + strlen_chat + 1, txt);

			delete[] chat;
			chat = new_chat;
		}
		else
		{
			chat = new wchar_t[strlen_txt + 1];
			lstrcpyW(chat, txt);
		}

		NFSC::CTextScroller_SetText(text_scroller, chat);

		auto added_lines = Read<int>(text_scroller + 0x34);
		auto visible_lines = Read<int>(text_scroller + 0x30);

		if (added_lines > visible_lines)
		{
			auto top_line = Read<int>(text_scroller + 0x48);

			auto v6 = top_line + 40;
			if (v6 > 0)
			{
				if (visible_lines + v6 >= added_lines)
				{
					v6 = added_lines - visible_lines;
				}
			}
			else
			{
				v6 = 0;
			}
			Write<int>(text_scroller + 0x48, v6);

			reinterpret_cast<void(__thiscall*)(uintptr_t, int)>(0x5A4CA0)(text_scroller, v6);

			auto scroll_bar = Read<uintptr_t>(text_scroller + 0x8);
			if (scroll_bar)
			{
				reinterpret_cast<void(__thiscall*)(uintptr_t, int, int, int, int)>(0x5B2C10)(scroll_bar, visible_lines, added_lines, top_line + 1, top_line + 1);
			}
		}
	}

	void ClearChat(uintptr_t text_scroller)
	{
		delete[] chat;
		chat = nullptr;

		NFSC::CTextScroller_SetText(text_scroller, L"");
	}

	struct FEChatStateManager : NFSC::FEStateManager
	{
		static inline FEChatStateManager* instance = nullptr;

		enum // States
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
			this->vtable[26] = FEChatStateManager::HandleNotifySound;
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

		static int __fastcall HandleNotifySound(FEChatStateManager* this_, void* edx, int unk1, int unk2)
		{
			if (/*this_->current_state == TYPING && */unk1 == 0x610FB237)
			{
				return -1;
			}
			else
			{
				return unk2;
			}
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
					NFSC::FE_String_Printf(Read<const char*>(chat_popup + 0xC), 0xA828438D, "");
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
					NFSC::FE_String_Printf(Read<const char*>(chat_popup + 0xC), 0xA828438D, "");

					AddChat(chat_popup + 0x28, txt);
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

				auto chat_fng = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
				if (chat_fng)
				{
					uintptr_t text_scroller = chat_fng + 0x28;

					static char chat_text[40];
					ImGui::InputText("##ChatText", chat_text, IM_ARRAYSIZE(chat_text));
					if (ImGui::Button("Add Chat"))
					{
						wchar_t chat_text_wide[40];
						StringToWideString(chat_text, 40, chat_text_wide, 40);

						AddChat(text_scroller, chat_text_wide);
					}
					ImGui::SameLine();
					if (ImGui::Button("Clear Chat"))
					{
						ClearChat(text_scroller);
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

	HOOK(0x833270, NFSC::NFSMessengerUser*, __fastcall, sub_00833270, void* OnlineThing, void* edx, NFSC::NFSMessengerUser* user, int unk);

	void Init()
	{
		Patch<uintptr_t>(0x59FFBD, reinterpret_cast<uintptr_t>(OpenChat) - 0x59FFC1);

		CREATE_HOOK(sub_00833270);

		Patch<uint8_t>(0x5C1470, 0xC3);
	}

	void End()
	{
		Unpatch(0x5C1470);

		Hooks::Destroy(0x833270);

		Unpatch(0x59FFBD);
	}

	NFSC::NFSMessengerUser* __fastcall sub_00833270_(void* OnlineThing, void* edx, NFSC::NFSMessengerUser* user, int unk)
	{
		memset(user, 0, sizeof(NFSC::NFSMessengerUser));
		strcpy_s(user->displayName, "brokenphilip's bulb hub");
		return user;
	}
}

MODULE(chat);
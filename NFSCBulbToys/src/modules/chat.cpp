#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace chat
{
	namespace example_manager
	{
		// don't go over 1000
		wchar_t* buffer = nullptr;

		void AddChat(const wchar_t* txt)
		{
			auto strlen_txt = lstrlenW(txt);
			if (buffer)
			{
				auto strlen_chat = lstrlenW(buffer);

				wchar_t* new_chat = new wchar_t[strlen_chat + 1 + strlen_txt + 1];
				lstrcpyW(new_chat, buffer);
				lstrcpyW(new_chat + strlen_chat, L"^");
				lstrcpyW(new_chat + strlen_chat + 1, txt);

				delete[] buffer;
				buffer = new_chat;
			}
			else
			{
				buffer = new wchar_t[strlen_txt + 1];
				lstrcpyW(buffer, txt);
			}
		}

		void ClearChat()
		{
			delete[] buffer;
			buffer = nullptr;
		}

		wchar_t* GetChat()
		{
			return buffer;
		}
	}

	void CTextScroller_Scroll(uintptr_t text_scroller, int scroll_offset = 0)
	{
		auto added_lines = Read<int>(text_scroller + 0x34);
		auto visible_lines = Read<int>(text_scroller + 0x30);

		if (added_lines > visible_lines)
		{
			auto top_line = added_lines - visible_lines - scroll_offset;
			if (top_line < 0)
			{
				top_line = 0;
			}

			// mTopLine
			Write<int>(text_scroller + 0x48, top_line);

			// sub_005A4CA0 (CTextScroller::ShowLinesFromIndex)
			reinterpret_cast<void(__thiscall*)(uintptr_t, int)>(0x5A4CA0)(text_scroller, top_line);

			auto scroll_bar = Read<uintptr_t>(text_scroller + 0x8);
			if (scroll_bar)
			{
				// sub_005B2C10 (FEScrollBar::Scroll)
				reinterpret_cast<void(__thiscall*)(uintptr_t, int, int, int, int)>(0x5B2C10)(scroll_bar, visible_lines, added_lines, top_line + 1, top_line + 1);
			}
		}
	}

	void FeOnlineMessengerChatPopup_Update(uintptr_t fng, int scroll_offset)
	{
		uintptr_t text_scroller = fng + 0x28;

		NFSC::CTextScroller_SetText(text_scroller, example_manager::GetChat());

		CTextScroller_Scroll(text_scroller, scroll_offset);
	}

	struct FEChatStateManager : NFSC::FEStateManager
	{
		static inline FEChatStateManager* instance = nullptr;
		
		int offset = 0;

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
			this->vtable[32] = FEChatStateManager::HandlePadDown;
			this->vtable[56] = FEChatStateManager::HandlePadUp;
			this->vtable[58] = FEChatStateManager::HandlePadStart;
			this->vtable[63] = FEChatStateManager::HandleScreenConstructed;

			FEChatStateManager::instance = this;

			if (NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
			{
				NFSC::SetSoundControlState(true, 1, "FEChatStateManager()");
			}

			return this;
		}

		FEChatStateManager* Deconstruct()
		{
			if (NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
			{
				NFSC::SetSoundControlState(false, 1, "~FEChatStateManager()");
			}

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
				// do not play sound for pressing [2] Block
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
				this_->previous_state = NORMAL;
				this_->current_state = TYPING;
				// HandleStateChange() - normally required, nullsub in our use case

				// ChatPopup->PackageFilename
				uintptr_t package = NFSC::BulbToys_FindPackage(reinterpret_cast<const char*>(NFSC::BulbToys_GetFeOnlineMessengerChatPopup() + 0xC));
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
				NFSC::FEDialogScreen_ShowKeyboard(0xA828438D, 0, reinterpret_cast<const char*>(NFSC::NullPointer),
					reinterpret_cast<const char*>(NFSC::NullPointer), 130 /* max chat message length, default 40 */, 1, 0);
			}
		}

		static void __fastcall HandlePadBack(FEChatStateManager* this_)
		{
			if (this_->current_state == TYPING)
			{
				this_->previous_state = TYPING;
				this_->current_state = NORMAL;
				// HandleStateChange() - normally required, nullsub in our use case

				// ChatPopup->PackageFilename
				NFSC::FE_String_Printf(Read<const char*>(NFSC::BulbToys_GetFeOnlineMessengerChatPopup() + 0xC), 0xA828438D, "");
			}
			else
			{
				NFSC::FEStateManager_Exit(reinterpret_cast<uintptr_t>(this_));
			}
		}

		static void __fastcall HandlePadDown(FEChatStateManager* this_)
		{
			this_->offset--;
			if (this_->offset <= 0)
			{
				this_->offset = 0;
			}
			FeOnlineMessengerChatPopup_Update(NFSC::BulbToys_GetFeOnlineMessengerChatPopup(), this_->offset);
		}

		static void __fastcall HandlePadUp(FEChatStateManager* this_)
		{
			auto fng = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();

			uintptr_t text_scroller = fng + 0x28;

			auto added_lines = Read<int>(text_scroller + 0x34);
			auto visible_lines = Read<int>(text_scroller + 0x30);

			this_->offset++;
			if (this_->offset >= added_lines - visible_lines)
			{
				this_->offset = added_lines - visible_lines;
			}
			FeOnlineMessengerChatPopup_Update(fng, this_->offset);
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
					// sub_00578CF0
					reinterpret_cast<bool(__thiscall*)(uintptr_t, wchar_t*, int, uintptr_t)>(0x578CF0)(histogram, txt, 1300, conv_buf);
				}
				else
				{
					// PackedStringToWideString
					reinterpret_cast<void(*)(wchar_t*, uintptr_t)>(0x4718A0)(txt, conv_buf);
				}

				auto chat_popup = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();

				// this->mChatPopup->PackageFilename
				NFSC::FE_String_Printf(Read<const char*>(chat_popup + 0xC), 0xA828438D, "");

				example_manager::AddChat(txt);

				this_->offset = 0;
				FeOnlineMessengerChatPopup_Update(chat_popup, this_->offset);
			}
		}

		static void __fastcall HandleScreenConstructed(FEChatStateManager* this_)
		{
			auto chat_fng = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();

			// FeOnlineMessengerChatPopup::Setup
			reinterpret_cast<void(__thiscall*)(uintptr_t, int)>(0x5C8670)(chat_fng, 0);

			this_->offset = 0;
			FeOnlineMessengerChatPopup_Update(chat_fng, this_->offset);
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

	class ChatPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Chat"))
			{
				uintptr_t fng = NFSC::BulbToys_GetFeOnlineMessengerChatPopup();
				if (fng)
				{
					ImGui::BulbToys_AddyLabel(fng, "FNG Address");
				}
				else
				{
					ImGui::Text("FNG not allocated.");
				}

				ImGui::Separator();

				if (example_manager::buffer)
				{
					// breaks after 50 lines !!!!
					ImGui::Text("Chat length: %d", lstrlenW(example_manager::buffer));

					if (ImGui::Button("Clear"))
					{
						example_manager::ClearChat();
					}
				}
				else
				{
					ImGui::Text("Chat not allocated.");
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

	HOOK(0x833270, NFSC::NFSMessengerUser*, __fastcall, OnlineThing_GetNFSMessengerUser, void* OnlineThing, void* edx, NFSC::NFSMessengerUser* user, int unk);

	HOOK(0x5A0E80, void, __fastcall, FEManager_OnPadButton, void* fe_manager, void* edx, int button);

	uintptr_t __fastcall EWorldMapOn_VecDelDtor(uintptr_t e_world_map_on, void* edx, int unk)
	{
		auto fe_manager = NFSC::BulbToys_GetFEManager();

		NFSC::FEManager_OnPauseRequest(fe_manager, 1337, 0, 0);
		auto chat = FEChatStateManager::CreateInstance(fe_manager, 0);
		if (chat)
		{
			NFSC::FEStateManager_PushChildManager(reinterpret_cast<uintptr_t>(fe_manager), reinterpret_cast<uintptr_t>(chat), 11);
		}

		// *this = &EventSys::Event::`vftable'
		Write<uintptr_t>(e_world_map_on, 0x9CEAE4);

		return e_world_map_on;
	}

	void Init()
	{
		// set custom "recipient" title
		CREATE_HOOK(OnlineThing_GetNFSMessengerUser);

		// press M to open chat in FE
		CREATE_HOOK(FEManager_OnPadButton);

		// fng doesn't exist in vanilla while racing
		if (NFSC::BulbToys_IsNFSCO())
		{
			// press M to open chat in-game
			Patch<void*>(0x9E1BD0, EWorldMapOn_VecDelDtor);
		}

		// remove frame delay
		Patch<uint8_t>(0x5C5271, 0);

		// disable FeOnlineMessengerChatPopup::SetupPartTwo
		Patch<uint8_t>(0x5C1470, 0xC3);
	}

	void End()
	{
		Unpatch(0x5C1470);

		Unpatch(0x5C5271);

		if (NFSC::BulbToys_IsNFSCO())
		{
			Unpatch(0x9E1BD0);
		}
		Hooks::Destroy(0x5A0E80);
		Hooks::Destroy(0x833270);
	}


	NFSC::NFSMessengerUser* __fastcall OnlineThing_GetNFSMessengerUser_(void* OnlineThing, void* edx, NFSC::NFSMessengerUser* user, int unk)
	{
		memset(user, 0, sizeof(NFSC::NFSMessengerUser));
		strcpy_s(user->displayName, "brokenphilip's bulb hub");
		return user;
	}

	void __fastcall FEManager_OnPadButton_(void* fe_manager, void* edx, int button)
	{
		if (button == 0 && NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::IN_FRONTEND)
		{
			if (FEChatStateManager::instance)
			{
				return;
			}

			auto fe_manager = NFSC::BulbToys_GetFEManager();

			// FEManager::FindLastChild
			auto last_child = reinterpret_cast<NFSC::FEStateManager*(__thiscall*)(NFSC::FEStateManager*)>(0x579220)(fe_manager);

			auto chat = FEChatStateManager::CreateInstance(last_child, 0);
			if (chat)
			{
				// FEManager::PushChildManagerToLastChild
				reinterpret_cast<void(__thiscall*)(NFSC::FEStateManager*, NFSC::FEStateManager*)>(0x5890A0)(fe_manager, chat);
			}

			return;
		}

		// FEStateManager::OnPadButton
		reinterpret_cast<void(__thiscall*)(void*, int)>(0x579540)(fe_manager, button);
	}
}

MODULE(chat);
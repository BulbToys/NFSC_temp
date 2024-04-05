#pragma once
#include <cstdint>

namespace NFSC
{
	struct FEStateManager
	{
		uintptr_t vtable;
		int current_state;
		int entry_point;
		int exit_point;
		int next_state;
		int previous_state;
		int sub_state;
		char last_screen[128];
		char* next_screen;
		bool next_state_valid;
		bool exiting_all;
		int screens_pushed;
		int screens_to_pop;
		FEStateManager* parent;
		FEStateManager* child;
		FEStateManager* next;
		char options[8];
		bool can_skip_movie;

		const char* GetName();
	};

	constexpr uintptr_t FEManager = 0xA97A7C;
}
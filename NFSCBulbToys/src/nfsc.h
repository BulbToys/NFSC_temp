#pragma once
#include <cstdint>

#include "../core/bulbtoys/utils.h"
#include "../core/bulbtoys/my_imgui.h"

namespace NFSC
{
	/* ===== N A M E S P A C E S ===== */

	namespace Attrib
	{
		struct Instance
		{
			uintptr_t mCollection = 0;
			uintptr_t mLayoutPtr = 0;
			uint32_t mMsgPort = 0;
			uint32_t mFlags = 0;
		};
	}

	/* ===== E N U M S ===== */

	namespace AIGoal
	{
		enum {
			ENCOUNTER_PURSUIT = 0,
			NONE              = 1,
			RACER             = 2,
			TRAFFIC           = 3,
			PATROL            = 4,
		};
	}
	inline const char* ai_goals[] = { "AIGoalEncounterPursuit", "AIGoalNone", "AIGoalRacer", "AIGoalTraffic", "AIGoalPatrol" };

	namespace Chyron
	{
		enum
		{
			CONNECT     = 0x1,
			DISCONNECT  = 0x2,
			MAIL        = 0x4,
			RACE_INVITE = 0x8,
			SOUND       = 0x10,
			NO_SOUND    = 0x20,
			REWARD      = 0x40,
		};
	}
	inline const char* chyron_icons[] = { "Connect", "Disconnect", "Mail", "Race Invite", "Sound", "No Sound", "Reward" };

	namespace DriverClass
	{
		enum
		{
			HUMAN        = 0x0,
			TRAFFIC      = 0x1,
			COP          = 0x2,
			RACER        = 0x3,
			NONE         = 0x4,
			NIS          = 0x5,
			REMOTE       = 0x6,
			REMOTE_RACER = 0x7,
			GHOST        = 0x8,
			HUB          = 0x9,
			MAX          = 0xA,
		};
	}
	inline const char* driver_classes[] = { "traffic", "cop", "racer", "none", "nis", "remote", "remote_racer", "ghost", "hub" };

	namespace FESM
	{
		namespace PhotoMode
		{
			enum
			{
				// This state is used on Photo Mode when we're entering the Customization state manager, so it can later give us our FNG back
				// This is exactly what the Crew Management state manager uses to push FeCrewCar.fng again once customization has finished
				GIMME_MY_QR_FNG = 100,
			};
		}

		namespace WorldMap
		{
			enum
			{
				// Normal states
				NORMAL       = 3,
				TERRITORIES  = 4,
				QUICK_LIST   = 5,
				ENGAGE_EVENT = 6,

				// Dialog states
				RACE_EVENT = 13,
				CAR_LOT    = 16,
				SAFEHOUSE  = 17,

				// Click TP states
				CLICK_TP      = 100,
				CLICK_TP_JUMP = 101,
				CLICK_TP_GPS  = 102,
			};
		}
	}

	// GameFlowState
	namespace GFS
	{
		enum
		{
			NONE               = 0x0,
			LOADING_FRONTEND   = 0x1,
			UNLOADING_FRONTEND = 0x2,
			IN_FRONTEND        = 0x3,
			LOADING_REGION     = 0x4,
			LOADING_TRACK      = 0x5,
			RACING             = 0x6,
			UNLOADING_TRACK    = 0x7,
			UNLOADING_REGION   = 0x8,
			EXIT_DEMO_DISC     = 0x9,
		};
	}

	// VehicleListType
	namespace VLType
	{
		enum
		{
			ALL           = 0x0,
			PLAYERS       = 0x1,
			AI            = 0x2,
			AI_RACERS     = 0x3,
			AI_COPS       = 0x4,
			AI_TRAFFIC    = 0x5,
			RACERS        = 0x6,
			REMOTE        = 0x7,
			INACTIVE      = 0x8,
			TRAILERS      = 0x9,
			ACTIVE_RACERS = 0xA,
			GHOST         = 0xB,
			MAX           = 0xC,
		};
	}
	inline const char* vehicle_lists[] = 
		{ "All", "Players", "AI", "AI Racers", "AI Cops", "AI Traffic", "Racers", "Remote", "Inactive", "Trailers", "Active Racers", "Ghosts" };

	namespace WRoadSegmentFlags
	{
		enum : uint16_t
		{
			IS_DECISION = 0x1,
			TRAFFIC_NOT_ALLOWED = 0x2,
			RACE_ROUTE_FORWARD = 0x4,
			UNKNOWN_0x08 = 0x8,
			IS_ENTRANCE = 0x10,
			COPS_XOR_TRAFFIC = 0x20,
			IS_ONE_WAY = 0x40,
			IS_SHORTCUT = 0x80,
			IS_CURVA_PATH = 0x100,
			IS_END_INVERTED = 0x200,
			IS_START_INVERTED = 0x400,
			IS_SIDE_ROUTE = 0x800,
			CROSSES_BARRIER = 0x1000,
			CROSSES_DRIVE_THRU_BARRIER = 0x2000,
			UNKNOWN_0x4000 = 0x4000,
			IS_IN_RACE = 0x8000,
		};
	}

	/* ===== S T R U C T S ===== */

	// B, G, R, A (stored as ints, but they're actually chars in practice)
	struct FEColor
	{
		int b = 0, g = 0, r = 0, a = 0;
	};

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
		NFSC::FEStateManager* parent;
		NFSC::FEStateManager* child;
		NFSC::FEStateManager* next;
		char options[8];
		bool can_skip_movie;

		const char* GetName();
	};

	template <typename T>
	struct ListableSet
	{
		uintptr_t vtbl;
		T* begin;
		size_t capacity;
		size_t size;
	};

	struct Vector2
	{
		float x = 0;
		float y = 0;
	};

	struct Vector3
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};

	struct Vector4
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;
	};

	struct Matrix4
	{
		NFSC::Vector4 v0;
		NFSC::Vector4 v1;
		NFSC::Vector4 v2;
		NFSC::Vector4 v3;
	};

	struct WCollisionMgr
	{
		unsigned int fSurfaceExclusionMask = 0;
		unsigned int fPrimitiveMask = 0;
	};

	struct WRoadNav // 780u
	{
		uint8_t pad0[0x58]{ 0 };

		bool fValid = false;

		uint8_t pad1[0x1B]{ 0 };

		int fNavType = 0;

		uint8_t pad2[0x10]{ 0 };

		char fNodeInd = 0;

		uint8_t pad3 = 0;

		short fSegmentInd = 0;

		uint8_t pad4[0x8]{ 0 };

		Vector3 fPosition{ 0, 0, 0 };
		Vector3 fLeftPosition{ 0, 0, 0 };
		Vector3 fRightPosition{ 0, 0, 0 };
		Vector3 fForwardVector{ 0, 0, 0 };

		uint8_t pad5[0x248]{ 0 };
	};

	struct WRoadSegment
	{
		uint16_t fNodeIndex[2];

		uint8_t pad0[0x6];

		// WRoadSegmentFlags
		uint16_t fFlags;

		uint8_t pad1[0xA];
	};

	struct WRoadNode
	{
		Vector3 fPosition;

		uint8_t pad0[0x4];

		char fNumSegments;
		uint16_t fSegmentIndex[7];
	};

	/* ===== C O N S T A N T S ===== */

	namespace WRoadNetwork
	{
		inline uint32_t BulbToys_GetNumSegments() { return Read<uint32_t>(0xB77E84); }

		inline NFSC::WRoadSegment* BulbToys_GetSegments() { return Read<NFSC::WRoadSegment*>(0xB77ECC); }

		inline NFSC::WRoadNode* BulbToys_GetNodes() { return Read<NFSC::WRoadNode*>(0xB77EC8); }
	}

	// Globals
	inline NFSC::FEStateManager* BulbToys_GetFEManager() { return Read<NFSC::FEStateManager*>(0xA97A7C); }

	inline int BulbToys_GetGameFlowState() { return Read<int>(0xA99BBC); }

	inline uintptr_t BulbToys_GetGManagerBase() { return Read<uintptr_t>(0xA98294); }

	inline uintptr_t BulbToys_GetWorldMap() { return Read<uintptr_t>(0xA977F0); }

	// Global Objects
	constexpr uintptr_t DALManager = 0xA8AD30;

	// ListableSets
	inline ListableSet<uintptr_t>* VehicleList[VLType::MAX] = {
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x0),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x1),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x2),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x3),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x4),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x5),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x6),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x7),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x8),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0x9),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0xA),
		reinterpret_cast<ListableSet<uintptr_t>*>(0xA9F158 + 0x88 * 0xB)
	};

	/* ===== H A S H I N G ===== */

	// "BIN Memory" Hash, FEHash
	constexpr uint32_t bStringHash(const char* string)
	{
		uint32_t result = -1;

		while (*string)
		{
			result = *string + 33 * result;
			string++;
		}

		return result;
	}

	/*
	#define HASH(str) { bStringHash(str), str }
	inline std::unordered_map<uint32_t, const char*> bin_hashes
	{
		HASH("example"),
	};
	#undef HASH
	*/

	// "VLT Memory" Hash
	constexpr uint32_t hash32(const char* string, unsigned int seed, unsigned int length)
	{
		unsigned int v3;
		unsigned int v4;
		unsigned int v5;
		unsigned int v6;
		unsigned int v8;
		unsigned int v9;
		unsigned int v10;
		unsigned int v11;
		unsigned int v12;
		unsigned int v13;
		unsigned int v14;
		unsigned int v15;
		unsigned int v16;
		unsigned int v17;
		unsigned int v18;
		unsigned int v19;
		unsigned int v20;
		unsigned int v21;
		unsigned int v22;
		unsigned int v23;
		unsigned int v25;

		v3 = length;
		v4 = 0x9E3779B9;
		v5 = length;
		v6 = 0x9E3779B9;
		if (length >= 0xC)
		{
			v25 = length / 0xC;
			do
			{
				v8 = v4 + *(string + 4) + ((*(string + 5) + ((*(string + 6) + (*(string + 7) << 8)) << 8)) << 8);
				v9 = seed + *(string + 8) + ((*(string + 9) + ((*(string + 10) + (*(string + 11) << 8)) << 8)) << 8);
				v10 = (v9 >> 13) ^ (v6 + *string
					+ ((*(string + 1) + ((*(string + 2) + (*(string + 3) << 8)) << 8)) << 8)
					- v9
					- v8);
				v11 = (v10 << 8) ^ (v8 - v9 - v10);
				v12 = (v11 >> 13) ^ (v9 - v11 - v10);
				v13 = (v12 >> 12) ^ (v10 - v12 - v11);
				v14 = (v13 << 16) ^ (v11 - v12 - v13);
				v15 = (v14 >> 5) ^ (v12 - v14 - v13);
				v6 = (v15 >> 3) ^ (v13 - v15 - v14);
				v4 = (v6 << 10) ^ (v14 - v15 - v6);
				seed = (v4 >> 15) ^ (v15 - v4 - v6);
				string += 12;
				v5 -= 12;
				--v25;
			} while (v25);
			v3 = length;
		}
		v16 = v3 + seed;
		switch (v5)
		{
		case 11:
			v16 += *(string + 10) << 24;
		case 10:
			v16 += *(string + 9) << 16;
		case 9:
			v16 += *(string + 8) << 8;
		case 8:
			v4 += *(string + 7) << 24;
		case 7:
			v4 += *(string + 6) << 16;
		case 6:
			v4 += *(string + 5) << 8;
		case 5:
			v4 += *(string + 4);
		case 4:
			v6 += *(string + 3) << 24;
		case 3:
			v6 += *(string + 2) << 16;
		case 2:
			v6 += *(string + 1) << 8;
		case 1:
			v6 += *string;
			break;
		default:
			break;
		}
		v17 = (v16 >> 13) ^ (v6 - v16 - v4);
		v18 = (v17 << 8) ^ (v4 - v16 - v17);
		v19 = (v18 >> 13) ^ (v16 - v18 - v17);
		v20 = (v19 >> 12) ^ (v17 - v19 - v18);
		v21 = (v20 << 16) ^ (v18 - v19 - v20);
		v22 = (v21 >> 5) ^ (v19 - v21 - v20);
		v23 = (v22 >> 3) ^ (v20 - v22 - v21);
		return (((v23 << 10) ^ (v21 - v22 - v23)) >> 15) ^ (v22 - ((v23 << 10) ^ (v21 - v22 - v23)) - v23);
	}

	/*
	#define HASH(str) { hash32(str, 0xABCDEF00, strlen(str)), str }
	inline std::unordered_map<uint32_t, const char*> vlt_hashes
	{
		HASH("example"),
	};
	#undef HASH
	*/

	/* ===== F U N C T I O N S ===== */

	FUNC(0x436820, void, __thiscall, AIGoal_AddAction, uintptr_t ai_goal, char const* name);
	FUNC(0x42A2D0, void, __thiscall, AIGoal_ChooseAction, uintptr_t ai_goal, float dt);
	FUNC(0x42A240, void, __thiscall, AIGoal_ClearAllActions, uintptr_t ai_goal);

	FUNC(0x40EBC0, bool, __thiscall, AITarget_GetVehicleInterface, uintptr_t ai_target, uintptr_t* i_vehicle);
	FUNC(0x429C80, void, __thiscall, AITarget_Acquire, uintptr_t ai_target, uintptr_t simable);

	FUNC(0x43BD80, uintptr_t, __thiscall, AIVehicle_GetPursuit, uintptr_t ai_vehicle);
	FUNC(0x406700, uintptr_t, __thiscall, AIVehicle_GetVehicle, uintptr_t ai_vehicle);

	FUNC(0x4639D0, uint32_t, , Attrib_StringToKey, const char* string);

	FUNC(0x46DFD0, uint16_t, , bATan, float x, float y);

	FUNC(0x48D620, void, , CameraAI_SetAction, int e_view, const char* name);

	FUNC(0x65B000, void, , ChangeLocalPlayerCameraInfo);

	FUNC(0x4A0890, char, __stdcall, DALCareer_GetPodiumVehicle, uint32_t* index);

	FUNC(0x4D1DE0, bool, __stdcall, DALFeVehicle_AddCarToMyCarsDB, uint32_t index);

	FUNC(0x4A0490, bool, __thiscall, DALManager_GetInt, uintptr_t dal_manager, int id, int* retval, int param_0, int param_1, int param_2);

	FUNC(0x6F7790, float, __thiscall, DamageVehicle_GetHealth, uintptr_t damage_vehicle);

	FUNC(0x571040, void, , FE_Image_SetTextureHash, uintptr_t fe_image, uint32_t key);

	FUNC(0x5A0250, uintptr_t, , FE_Object_FindObject, const char* package_name, uint32_t key);
	FUNC(0x597900, void, , FE_Object_GetCenter, uintptr_t fe_object, float* x, float* y);
	FUNC(0x570CC0, void, , FE_Object_SetColor, uintptr_t fe_object, NFSC::FEColor* color);
	FUNC(0x570460, void, , FE_Object_SetVisibility, uintptr_t fe_object, bool visible);

	FUNC(0x59D450, void, , FE_ShowChyron, const char* message, int icon, bool linger);

	FUNC(0x5711C0, uint32_t, , FE_String_HashString, const char* fmt, ...);
	FUNC(0x583B10, void, , FE_String_SetString, uintptr_t object, const wchar_t* wide_string);

	FUNC(0x5CDEA0, void, , FEDialogScreen_ShowDialog, const char* message, const char* button1, const char* button2, const char* button3);
	FUNC(0x5CF440, void, , FEDialogScreen_ShowOK, const char* message);

	FUNC(0x572B90, uintptr_t, __thiscall, FEManager_GetUserProfile, NFSC::FEStateManager* fe_manager, int index);

	FUNC(0x49C020, uintptr_t, __thiscall, FEPlayerCarDB_GetCarRecordByHandle, uintptr_t player_car_db, uint32_t handle);

	FUNC(0x579200, void, __thiscall, FEStateManager_ChangeState, uintptr_t state_manager, int current_state);
	FUNC(0x5792A0, bool, __thiscall, FEStateManager_IsGameMode, uintptr_t state_manager, int efegamemode);
	FUNC(0x5A53A0, void, __thiscall, FEStateManager_PopBack, uintptr_t state_manager, int next_state);
	FUNC(0x593750, void, __thiscall, FEStateManager_Push, uintptr_t state_manager, const char* next_screen, int next_state);
	FUNC(0x579C10, void, __thiscall, FEStateManager_ShowDialog, uintptr_t state_manager, int next_state);
	FUNC(0x59B140, void, __thiscall, FEStateManager_Switch, uintptr_t state_manager, const char* next_screen, uint32_t leave_message, int next_state, int screens_to_pop);
	FUNC(0x5A52B0, void, __thiscall, FEStateManager_SwitchChildManager, uintptr_t state_manager, uintptr_t state_manager_child, int next_state, int screens_to_pop);

	FUNC(0x65C330, void, , Game_ClearAIControl, int unk);
	FUNC(0x65C2C0, void, , Game_ForceAIControl, int unk);
	FUNC(0x6517E0, uintptr_t, , Game_GetWingman, uintptr_t i_simable);
	FUNC(0x65D620, uintptr_t, , Game_PursuitSwitch, int racer_index, bool is_busted, int* result);
	FUNC(0x651750, void, , Game_SetAIGoal, uintptr_t simable, const char* goal);
	FUNC(0x6513E0, void, , Game_SetCopsEnabled, bool enable);
	FUNC(0x6517A0, void, , Game_SetPursuitTarget, uintptr_t chaser_simable, uintptr_t target_simable);
	FUNC(0x65DD60, void, , Game_TagPursuit, int index1, int index2, bool busted);
	FUNC(0x667FF0, void, , Game_UnlockNikki);

	FUNC(0x578830, const char*, , GetLocalizedString, uint32_t key);
	FUNC(0x55CFD0, uintptr_t, , GetTextureInfo, uint32_t key, int return_default_texture_if_not_found, int include_unloaded_textures);
	FUNC(0x5D89B0, void, , GetVehicleVectors, NFSC::Vector2& position, NFSC::Vector2& direction, uintptr_t i_simable);

	FUNC(0x627840, void, __thiscall, GIcon_Spawn, uintptr_t icon);

	FUNC(0x616FE0, uint32_t, , GKnockoutRacer_GetPursuitVehicleKey, bool unk);

	FUNC(0x626F90, uintptr_t, __thiscall, GManager_AllocIcon, uintptr_t g_manager, char type, Vector3* position, float rotation, bool is_disposable);

	FUNC(0x433AB0, bool, , GPS_Engage, NFSC::Vector3* target, float max_deviation, bool always_re_establish);
	FUNC(0x41ECD0, bool, , GPS_IsEngaged);

	FUNC(0x422730, uintptr_t, __thiscall, GRacerInfo_GetSimable, uintptr_t g_racer_info);
	FUNC(0x61B8F0, void, __thiscall, GRacerInfo_SetSimable, uintptr_t g_racer_info, uintptr_t simable);

	FUNC(0x624E80, uintptr_t, __thiscall, GRaceStatus_GetRacePursuitTarget, uintptr_t g_race_status, int* index);
	FUNC(0x612230, int, __thiscall, GRaceStatus_GetRacerCount, uintptr_t g_race_status);
	FUNC(0x6121E0, uintptr_t, __thiscall, GRaceStatus_GetRacerInfo, uintptr_t g_race_status, int index);
	FUNC(0x629670, uintptr_t, __thiscall, GRaceStatus_GetRacerInfo2, uintptr_t g_race_status, uintptr_t simable);

	FUNC(0x7BF9B0, void, , KillSkidsOnRaceRestart);

	FUNC(0x75DA60, void, __thiscall, LocalPlayer_ResetHUDType, uintptr_t local_player, int hud_type);

	FUNC(0x6A1560, uintptr_t, , malloc, size_t size);
	FUNC(0x6A1590, void, , free, uintptr_t ptr);

	FUNC(0x59DD90, uintptr_t, __thiscall, MapItem_MapItem, uintptr_t map_item, uint32_t flags, uintptr_t object, NFSC::Vector2& position, float rotation, int unk,
		uintptr_t icon);

	FUNC(0x6C6740, bool, __thiscall, PhysicsObject_Attach, uintptr_t physics_object, uintptr_t player);
	FUNC(0x6D6C40, uintptr_t, __thiscall, PhysicsObject_GetPlayer, uintptr_t physics_object);
	FUNC(0x6D6CD0, uintptr_t, __thiscall, PhysicsObject_GetRigidBody, uintptr_t physics_object);
	FUNC(0x6D6D10, uint32_t, __thiscall, PhysicsObject_GetWorldID, uintptr_t physics_object);
	FUNC(0x6D19A0, void, __thiscall, PhysicsObject_Kill, uintptr_t physics_object);

	FUNC(0x803B40, bool, , Props_CreateInstance, uintptr_t& placeable_scenery, const char* name, uint32_t attributes);

	FUNC(0x6C0BE0, void, __thiscall, PVehicle_Activate, uintptr_t pvehicle);
	FUNC(0x6C0C00, void, __thiscall, PVehicle_Deactivate, uintptr_t pvehicle);
	FUNC(0x6C0C40, void, __thiscall, PVehicle_ForceStopOn, uintptr_t pvehicle, uint8_t force_stop);
	FUNC(0x6C0C70, void, __thiscall, PVehicle_ForceStopOff, uintptr_t pvehicle, uint8_t force_stop);
	FUNC(0x6D8110, uintptr_t, __thiscall, PVehicle_GetAIVehiclePtr, uintptr_t pvehicle);
	FUNC(0x6D7F60, int, __thiscall, PVehicle_GetDriverClass, uintptr_t pvehicle);
	FUNC(0x6D7F80, uint8_t, __thiscall, PVehicle_GetForceStop, uintptr_t pvehicle);
	FUNC(0x6D7EC0, uintptr_t, __thiscall, PVehicle_GetSimable, uintptr_t pvehicle);
	FUNC(0x6D8070, float, __thiscall, PVehicle_GetSpeed, uintptr_t pvehicle);
	FUNC(0x6D7F20, char*, __thiscall, PVehicle_GetVehicleName, uintptr_t pvehicle);
	FUNC(0x6C0BA0, void, __thiscall, PVehicle_GlareOn, uintptr_t pvehicle, uint32_t fx_id);
	FUNC(0x6D80C0, bool, __thiscall, PVehicle_IsActive, uintptr_t pvehicle);
	FUNC(0x6C0A00, bool, __thiscall, PVehicle_IsLoading, uintptr_t pvehicle);
	FUNC(0x6D43A0, void, __thiscall, PVehicle_Kill, uintptr_t pvehicle);
	FUNC(0x6D4410, void, __thiscall, PVehicle_ReleaseBehaviorAudio, uintptr_t pvehicle);
	FUNC(0x6DA500, void, __thiscall, PVehicle_SetDriverClass, uintptr_t pvehicle, int driver_class);
	FUNC(0x6C61E0, void, __thiscall, PVehicle_SetSpeed, uintptr_t pvehicle, float speed);
	FUNC(0x6D1100, bool, __thiscall, PVehicle_SetVehicleOnGround, uintptr_t pvehicle, NFSC::Vector3* position, NFSC::Vector3* forward_vector);

	FUNC(0x6C6CD0, void, __thiscall, RigidBody_GetDimension, uintptr_t rigid_body, NFSC::Vector3* dimension);
	FUNC(0x6C70E0, void, __thiscall, RigidBody_GetForwardVector, uintptr_t rigid_body, NFSC::Vector3* forward_vector);
	FUNC(0x6C6FF0, NFSC::Vector3*, __thiscall, RigidBody_GetPosition, uintptr_t rigid_body);
	FUNC(0x6E8210, void, __thiscall, RigidBody_SetPosition, uintptr_t rigid_body, NFSC::Vector3* position);

	FUNC(0x761550, float, , Sim_DistanceToCamera, NFSC::Vector3* target);

	FUNC(0x6ECE10, uintptr_t, __thiscall, Sim_Effect_vecDelDtor, uintptr_t sim_effect, bool should_free);
	FUNC(0x7625E0, uintptr_t, __thiscall, Sim_Effect_Effect, uintptr_t sim_effect, uint32_t owner_id, uintptr_t participant);


	FUNC(0x411FD0, float, , UMath_Distance, NFSC::Vector3* vec1, Vector3* vec2);
	FUNC(0x401DD0, float, , UMath_DistanceNoSqrt, NFSC::Vector3* vec1, Vector3* vec2);
	FUNC(0x412190, void, , UMath_Normalize, NFSC::Vector3* vec);
	FUNC(0x401B50, void, , UMath_Rotate, NFSC::Vector3* vec, NFSC::Matrix4* rot, Vector3* result);

	FUNC(0x764E00, void, , Util_GenerateMatrix, NFSC::Matrix4* result, Vector3* fwd_vec, Vector3* in_up);

	FUNC(0x406340, void, , VU0_v3add, float a1, Vector3* a2, Vector3* a3);

	FUNC(0x816DF0, bool, __thiscall, WCollisionMgr_GetWorldHeightAtPointRigorous, NFSC::WCollisionMgr& mgr, Vector3* point, float* height, NFSC::Vector3* normal);

	FUNC(0x7CA1A0, void, , World_RestoreProps);

	FUNC(0x56E770, void, , WorldMap_ConvertPos, float& x, float& y, Vector2& track_map_tl, Vector2& track_map_size);
	FUNC(0x5ACA90, void, __thiscall, WorldMap_GetPanFromMapCoordLocation, uintptr_t world_map, NFSC::Vector2* output, NFSC::Vector2* input);
	FUNC(0x582E60, bool, __thiscall, WorldMap_IsInPursuit, uintptr_t world_map);
	FUNC(0x582C30, void, , WorldMap_SetGPSIng, uintptr_t icon);

	FUNC(0x7F7BF0, void, __thiscall, WRoadNav_Destructor, NFSC::WRoadNav& nav);
	FUNC(0x7FB090, bool, __thiscall, WRoadNav_FindPath, uintptr_t roadnav, NFSC::Vector3* goal_position, NFSC::Vector3* goal_direction, bool shortcuts_allowed);
	FUNC(0x80C600, void, __thiscall, WRoadNav_IncNavPosition, NFSC::WRoadNav& nav, float distance, NFSC::Vector3* to, float max_look_ahead, bool excl_shortcuts);
	FUNC(0x80F180, void, __thiscall, WRoadNav_InitAtPoint, NFSC::WRoadNav& nav, Vector3* pos, NFSC::Vector3* dir, bool force_center_lane, float dir_weight);
	FUNC(0x806820, uintptr_t, __thiscall, WRoadNav_WRoadNav, NFSC::WRoadNav& nav);

	/* ===== H E L P E R   F U N C S ===== */

	inline uintptr_t BulbToys_FastMemAlloc(size_t size, const char* debug_name = nullptr) 
		{ return reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, size_t, const char*)>(0x60BA70)(0xA99720, size, debug_name); }

	template <uintptr_t handle>
	inline uintptr_t BulbToys_FindInterface(uintptr_t iface)
	{
		uintptr_t ucom_object = Read<uintptr_t>(iface + 4);

		// IInterface* UTL::COM::Object::_IList::Find(UCOM::Object::_IList*, IInterface::IHandle);
		return reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uintptr_t)>(0x60CB50)(ucom_object, handle);
	}

	bool BulbToys_GetDebugCamVectors(Vector3* position = nullptr, Vector3* fwd_vec = nullptr);

	bool BulbToys_GetMyVehicle(uintptr_t* my_vehicle = nullptr, uintptr_t* my_simable = nullptr);

	void BulbToys_GetScreenPosition(Vector3& world, Vector3& screen);

	inline bool BulbToys_IsNFSCO() { return Read<uint32_t>(0x692539) == 28; }

	inline bool BulbToys_IsPlayerLocal(uintptr_t player) { return Read<uintptr_t>(player) == 0x9EC8C0; /* RecordablePlayer::`vftable'{for `IPlayer'} */ }
}

/* ===== I M G U I ===== */
namespace ImGui
{
	void NFSC_DistanceBar(float distance, float max_distance = 1000.0f, float width = 50.0f, float height = 14.0f);

	float NFSC_DistanceWidth(NFSC::Vector3& other_position, float max_distance = 50.0f, float min = 1.0f, float max = 5.0f);

	ImVec4 NFSC_DriverColor(int dc);

	void NFSC_Location(const char* label, const char* id, float* location);

	void NFSC_OverlayText(ImDrawList* draw_list, ImVec2& position, ImVec4& color, const ImVec4* bg_color = nullptr, const char* fmt = nullptr, ...);
}
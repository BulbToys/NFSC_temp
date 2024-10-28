#include "nfsc.h"

const char* NFSC::FEStateManager::GetName()
{
	switch (reinterpret_cast<uintptr_t>(this->vtable))
	{
		case 0x9D2420: return "FEStateManager";
		case 0x9D2540: return "FEBootFlowStateManager (PURE)";
		case 0x9D2688: return "FECareerStateManager";
		case 0x9D27A8: return "FEPostPursuitStateManager";
		case 0x9D28F8: return "FESmsStateManager";
		case 0x9D2A40: return "FEPauseStateManager";
		case 0x9D2B60: return "FEConfirmFeStateManager";
		case 0x9D2CA8: return "FEPostRaceStateManager";
		case 0x9D2E10: return "FEWorldMapStateManager";
		case 0x9D2F98: return "FEPhotoModeStateManager";
		case 0x9D30C0: return "FEOptionsStateManager";
		case 0x9D31E8: return "FEMovieStateManager";
		case 0x9D3330: return "FEOnlineMessengerStateManager";
		case 0x9D3480: return "FEOnlineLoginStateManager";
		case 0x9D35A0: return "FEOnlineQuickMatchStateManager";
		case 0x9D36C0: return "FEOnlineCustomMatchStateManager";
		case 0x9D37E8: return "FEOnlineGameroomStateManager";
		case 0x9D3920: return "FELeaderboardStateManager";
		case 0x9D3B20: return "FEManager";
		case 0x9D4430: return "FEMemcardStateManager";
		case 0x9E8B30: return "FEBootFlowStateManager";
		case 0x9E8E30: return "FEOnlineStateManager";
		case 0x9F7C30: return "FEShoppingCartStateManager";
		case 0x9F7E68: return "FEMainStateManager";
		case 0x9F8058: return "FEChallengeStateManager";
		case 0x9F8198: return "FEWingmanStateManager";
		case 0x9F82D0: return "FECrewManagementStateManager";
		case 0x9F8428: return "FEQuickRaceStateManager";
		case 0x9F8560: return "FEAutosculptStateManager";
		case 0x9F8688: return "FETemplateStateManager";
		case 0x9F87A8: return "FEMyCarsStateManager";
		case 0x9F88C8: return "FECarClassSelectStateManager";
		case 0x9F89E8: return "FEGameModeTutorialStateManager";
		case 0x9F8B08: return "FERaceStarterStateManager";
		case 0x9F8C40: return "FECrewLogoStateManager";
		case 0x9F8D60: return "FERewardsCardStateManager";
		case 0x9F8E80: return "FEBossStateManager";
		case 0x9F8FA0: return "FEGameOverStateManager";
		case 0x9F90C0: return "FEGameWonStateManager";
		case 0x9F9200: return "FECreditsStateManager";
		case 0x9F9320: return "FEPressStartStateManager";
		case 0x9FA010: return "FEStatisticsStateManager";
		case 0x9FAD58: return "FECustomizeStateManager";
		case 0x9FB350: return "FECarSelectStateManager";
		case 0x9FB488: return "FEDebugCarStateManager";
		default: return "(INVALID)";
	}
}

// Returns false if we're not in Debug Cam
bool NFSC::BulbToys_GetDebugCamVectors(Vector3* position, Vector3* fwd_vec)
{
	if (NFSC::BulbToys_GetGameFlowState() != GFS::RACING)
	{
		return false;
	}

	uintptr_t first_camera_director = Read<uintptr_t>(0xA8ACC4 + 4);
	uintptr_t camera_director = Read<uintptr_t>(first_camera_director);
	uintptr_t cd_action = Read<uintptr_t>(camera_director + 0x18);

	// Check if we're in CDActionDebug (CDActionDebug::`vftable'{for `CameraAI::Action'})
	if (Read<uintptr_t>(cd_action) != 0x9C7EE0)
	{
		return false;
	}

	uintptr_t camera_mover = Read<uintptr_t>(cd_action + 0x2BC);
	uintptr_t camera = Read<uintptr_t>(camera_mover + 0x1C);

	if (position)
	{
		// z, -x, y -> x, y, z
		position->x = -Read<float>(camera + 0x44);
		position->y = Read<float>(camera + 0x48);
		position->z = Read<float>(camera + 0x40);
	}

	if (fwd_vec)
	{
		// z, -x, y -> x, y, z
		fwd_vec->x = -Read<float>(camera + 0x54);
		fwd_vec->y = Read<float>(camera + 0x58);
		fwd_vec->z = Read<float>(camera + 0x50);
	}

	return true;
}

bool NFSC::BulbToys_GetMyVehicle(uintptr_t* my_vehicle, uintptr_t* my_simable)
{
	auto gfs = NFSC::BulbToys_GetGameFlowState();
	if (gfs == GFS::RACING || gfs == GFS::LOADING_REGION || gfs == GFS::LOADING_TRACK)
	{
		for (int i = 0; i < (int)VehicleList[VLType::PLAYERS]->size; i++)
		{
			uintptr_t vehicle = VehicleList[VLType::PLAYERS]->begin[i];

			if (vehicle)
			{
				uintptr_t simable = PVehicle_GetSimable(vehicle);

				if (simable)
				{
					uintptr_t player = PhysicsObject_GetPlayer(simable);

					if (player && BulbToys_IsPlayerLocal(player))
					{
						if (my_vehicle)
						{
							*my_vehicle = vehicle;
						}
						if (my_simable)
						{
							*my_simable = simable;
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

void NFSC::BulbToys_GetScreenPosition(Vector3& world, Vector3& screen)
{
	Vector4 out, input = { world.z, -world.x, world.y, 1.0 };

	// ViewPlatInfoTable[EVIEW_PLAYER (1)].ViewProjectionMatrix
	Matrix4 proj = Read<Matrix4>((0xB1A780 + 1 * 0x1A0) + 0x80);

	// D3DXVec4Transform
	reinterpret_cast<Vector4* (__stdcall*)(Vector4*, Vector4*, Matrix4*)>(0x86B29C)(&out, &input, &proj);

	float i_w = 1.0f / out.w;
	out.x *= i_w;
	out.y *= i_w;

	screen.x = (out.x + 1.0f) * Read<int>(0xAB0AC8) * 0.5f; // ResolutionX
	screen.y = (out.y - 1.0f) * Read<int>(0xAB0ACC) * -0.5f; // ResolutionY
	screen.z = i_w * out.z;
}

void ImGui::NFSC_DistanceBar(float distance, float max_distance, float width, float height)
{
	char overlay[8];
	if (distance > 99999)
	{
		sprintf_s(overlay, 8, "99999+");
	}
	else
	{
		sprintf_s(overlay, 8, "%.0f", distance);
	}

	ImGui::SameLine();
	ImGui::ProgressBar(distance / max_distance, ImVec2(width, height), overlay);
}

void ImGui::NFSC_Location(const char* label, const char* id, float* location)
{
	char buffer[32];

	ImGui::Text(label);
	ImGui::SameLine();

	sprintf_s(buffer, 32, "Mine##%s_M", id);
	if (ImGui::Button(buffer) && NFSC::BulbToys_GetGameFlowState() == NFSC::GFS::RACING)
	{
		NFSC::Vector3 pos = { 0, 0, 0 };

		// If we're in Debug Cam, use those coordinates instead
		if (NFSC::BulbToys_GetDebugCamVectors(&pos, nullptr))
		{
			location[0] = pos.x;
			location[1] = pos.y;
			location[2] = pos.z;
			return;
		}

		// Else just use our own coordinates
		uintptr_t simable = 0;
		NFSC::BulbToys_GetMyVehicle(nullptr, &simable);
		if (simable)
		{
			uintptr_t rigid_body = NFSC::PhysicsObject_GetRigidBody(simable);
			if (rigid_body)
			{
				NFSC::Vector3* position = NFSC::RigidBody_GetPosition(rigid_body);
				location[0] = position->x;
				location[1] = position->y;
				location[2] = position->z;
			}
		}
	}
	ImGui::InputFloat3(id, location);
}

void ImGui::NFSC_OverlayText(ImDrawList* draw_list, ImVec2& position, ImVec4& color, const ImVec4* bg_color, const char* fmt, ...)
{
	char text[512];
	va_list va;
	va_start(va, fmt);
	vsprintf_s(text, 512, fmt, va);

	ImVec2 text_size = ImGui::CalcTextSize(text);

	auto bg_color_u32 = ImGui::ColorConvertFloat4ToU32(bg_color ? *bg_color : ImVec4(color.x / 5, color.y / 5, color.z / 5, color.w));

	draw_list->AddRectFilled({ position.x - text_size.x / 2, position.y }, { position.x + text_size.x / 2, position.y + text_size.y }, bg_color_u32, 1.0f);

	draw_list->AddText({ position.x - text_size.x / 2, position.y }, ImGui::ColorConvertFloat4ToU32(color), text);
}

ImVec4 ImGui::NFSC_DriverColor(int dc)
{
	switch (dc)
	{
		case NFSC::DriverClass::HUMAN:        return ImVec4(0, 1, 1, 1);           // cyan
		case NFSC::DriverClass::TRAFFIC:      return ImVec4(.6f, .4f, 0, 1);       // brown
		case NFSC::DriverClass::COP:          return ImVec4(.25f, .25f, 1, 1);     // light blue
		case NFSC::DriverClass::RACER:        return ImVec4(1, .6f, 0, 1);         // orange
		case NFSC::DriverClass::NONE:         return ImVec4(1, 1, 1, 1);           // white
		case NFSC::DriverClass::NIS:          return ImVec4(1, 1, 0, 1);           // yellow
		case NFSC::DriverClass::REMOTE:       return ImVec4(0, .75f, 0, 1);        // darker green
		case NFSC::DriverClass::REMOTE_RACER: return ImVec4(0, 1, 0, 1);           // green
		case NFSC::DriverClass::GHOST:        return ImVec4(.75f, .75f, .75f, 1);  // gray
		default: /* NFSC::DriverClass::HUB */ return ImVec4(1, 0, 1, 1);           // magenta
	}
}

float ImGui::NFSC_DistanceWidth(NFSC::Vector3& other_position, float max_distance, float min, float max)
{
	float scale = max_distance / (max - min);

	float distance = NFSC::Sim_DistanceToCamera(&other_position);
	if (distance < 1)
	{
		return max;
	}
	else if (distance > max_distance)
	{
		return min;
	}

	return max - (distance / scale);
}
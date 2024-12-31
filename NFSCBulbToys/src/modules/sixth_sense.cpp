#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace sixth_sense
{
	bool coords = false;
	bool own_vehicle = false;
	bool other_vehicles = false;
	bool allow_deactivated_vehicles = false;
	bool cop_health = false;

	class SixthSensePanel : public IPanel
	{
		virtual bool Draw() override final
		{
			if (ImGui::BulbToys_Menu("Sixth Sense"))
			{
				ImGui::Checkbox("Display coordinates", &sixth_sense::coords);

				ImGui::Checkbox("Draw my own vehicle", &sixth_sense::own_vehicle);

				ImGui::Checkbox("Draw other vehicles (", &sixth_sense::other_vehicles);
				ImGui::SameLine();
				ImGui::Checkbox("##DVehicles", &sixth_sense::allow_deactivated_vehicles);
				ImGui::SameLine();
				ImGui::Text("deactivated )");

				ImGui::Checkbox("Draw cop health", &sixth_sense::cop_health);
			}

			return true;
		}
	};

	class SixthSenseOverlay : public IPanel
	{
		virtual bool Draw() override final
		{
			uintptr_t my_vehicle = 0;
			uintptr_t my_simable = 0;
			uintptr_t my_rigid_body = 0;
			if (NFSC::BulbToys_GetMyVehicle(&my_vehicle, &my_simable))
			{
				my_rigid_body = NFSC::PhysicsObject_GetRigidBody(my_simable);
			}

			auto draw_list = ImGui::GetWindowDrawList();

			if (sixth_sense::coords)
			{
				NFSC::Vector3 pos = { 0, 0, 0 };
				NFSC::Vector3 fwd_vec = { 0, 0, 0 };

				if (my_rigid_body)
				{
					pos = *NFSC::RigidBody_GetPosition(my_rigid_body);
					NFSC::RigidBody_GetForwardVector(my_rigid_body, &fwd_vec);

					ImGui::Text("RigidBody coords:");
					ImGui::Text("- Position: { %.2f, %.2f, %.2f }", pos.x, pos.y, pos.z);
					ImGui::Text("- Rotation: { %.2f, %.2f, %.2f } (XZ: %.2f)", fwd_vec.x, fwd_vec.y, fwd_vec.z, NFSC::BulbToys_ToAngle(fwd_vec));
				}
				if (NFSC::BulbToys_GetDebugCamVectors(&pos, &fwd_vec))
				{
					ImGui::Text("DebugCam coords:");
					ImGui::Text("- Position: { %.2f, %.2f, %.2f }", pos.x, pos.y, pos.z);
					ImGui::Text("- Rotation: { %.2f, %.2f, %.2f } (XZ: %.2f)", fwd_vec.x, fwd_vec.y, fwd_vec.z, NFSC::BulbToys_ToAngle(fwd_vec));
				}
			}

			if (sixth_sense::own_vehicle && my_rigid_body)
			{
				NFSC::Vector3 position = *NFSC::RigidBody_GetPosition(my_rigid_body);

				NFSC::Vector3 dimension;
				NFSC::RigidBody_GetDimension(my_rigid_body, &dimension);

				NFSC::Vector3 fwd_vec;
				NFSC::RigidBody_GetForwardVector(my_rigid_body, &fwd_vec);

				ImVec4 cyan = ImVec4(0, 1, 1, 1);
				ImGui::NFSC_Overlay_Cuboid(draw_list, position, dimension, fwd_vec, cyan, ImGui::NFSC_DistanceWidth(position));
				ImGui::NFSC_Overlay_VehicleInfo(draw_list, my_vehicle, NFSC::VLType::ALL, cyan);
			}

			if (sixth_sense::other_vehicles)
			{
				for (uint32_t i = 0; i < NFSC::VehicleList[0]->size; i++)
				{
					uintptr_t vehicle = NFSC::VehicleList[0]->begin[i];

					bool active = NFSC::PVehicle_IsActive(vehicle);
					if (!active && !sixth_sense::allow_deactivated_vehicles)
					{
						continue;
					}

					auto dc = NFSC::PVehicle_GetDriverClass(vehicle);
					if (dc == NFSC::DriverClass::HUMAN)
					{
						continue;
					}

					uintptr_t simable = NFSC::PVehicle_GetSimable(vehicle);
					uintptr_t rigid_body = NFSC::PhysicsObject_GetRigidBody(simable);

					NFSC::Vector3 position = *NFSC::RigidBody_GetPosition(rigid_body);

					NFSC::Vector3 dimension;
					NFSC::RigidBody_GetDimension(rigid_body, &dimension);

					NFSC::Vector3 fwd_vec;
					NFSC::RigidBody_GetForwardVector(rigid_body, &fwd_vec);

					ImVec4 color = ImGui::NFSC_DriverColor(dc);
					float thickness = ImGui::NFSC_DistanceWidth(position);

					ImGui::NFSC_Overlay_Cuboid(draw_list, position, dimension, fwd_vec, color, thickness);
					ImGui::NFSC_Overlay_VehicleInfo(draw_list, i, NFSC::VLType::ALL, color);
					if (!active)
					{
						color = ImVec4(1, 0, 0, 0.25);
						ImGui::NFSC_Overlay_Cuboid(draw_list, position, dimension, fwd_vec, color, thickness * 2);
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
			return new SixthSensePanel();
		}

		if (dt == Module::DrawType::Overlay)
		{
			return new SixthSenseOverlay();
		}
	}

	HOOK(0x7AEFD0, void, __fastcall, CarRenderConn_UpdateIcon, uintptr_t car_render_conn, uintptr_t edx, uintptr_t pkt);

	void Init()
	{
		CREATE_HOOK(CarRenderConn_UpdateIcon);
	}

	void End()
	{
		// hooks destroy
	}

	void __fastcall CarRenderConn_UpdateIcon_(uintptr_t car_render_conn, uintptr_t edx, uintptr_t pkt)
	{
		CarRenderConn_UpdateIcon(car_render_conn, edx, pkt);

		if (!sixth_sense::cop_health)
		{
			return;
		}

		// Don't do anything if we have an icon already
		if (Read<uintptr_t>(car_render_conn + 0x1AC) != 0)
		{
			return;
		}

		uint32_t world_id = Read<uint32_t>(car_render_conn + 0x2C);

		uintptr_t this_vehicle = 0;
		for (size_t i = 0; i < NFSC::VehicleList[NFSC::VLType::AI_COPS]->size; i++)
		{
			uintptr_t vehicle = NFSC::VehicleList[NFSC::VLType::AI_COPS]->begin[i];
			uintptr_t simable = NFSC::PVehicle_GetSimable(vehicle);

			uint32_t simable_wid = NFSC::PhysicsObject_GetWorldID(simable);

			if (simable_wid == world_id)
			{
				this_vehicle = vehicle;
				break;
			}
		}
		if (!this_vehicle)
		{
			return;
		}
		uintptr_t i_damageable = Read<uintptr_t>(this_vehicle + 0x44);

		// Set icon
		constexpr uint32_t INGAME_ICON_PLAYERCAR = 0x3E9CCFFA;
		Write<uintptr_t>(car_render_conn + 0x1AC, NFSC::GetTextureInfo(INGAME_ICON_PLAYERCAR, 1, 0));

		// Set scale
		Write<float>(car_render_conn + 0x1B4, 0.5f);

		float health = NFSC::DamageVehicle_GetHealth(i_damageable);

		struct color_
		{
			uint8_t a = 0xFF;
			uint8_t b = 0;
			uint8_t g = 0;
			uint8_t r = 0;
		} color;

		if (health > 0)
		{
			if (health > 0.5)
			{
				health -= 0.5;
				color.g = 0xFF;
				color.r = 0xFF - (uint8_t)(0x1FF * health);
			}
			else
			{
				color.g = (uint8_t)(0x1FF * health);
				color.r = 0xFF;
			}
		}

		// Set color
		Write<color_>(car_render_conn + 0x1B0, color);
	}
}

MODULE(sixth_sense);
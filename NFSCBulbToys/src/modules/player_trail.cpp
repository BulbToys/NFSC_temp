#include "../../core/bulbtoys.h"
#include "../nfsc.h"

namespace player_trail
{
	bool initialized = false;
	NFSC::Attrib::Instance carpathfinder;
	uintptr_t effect[2] = { 0, 0 };

	class TrailPanel : public IPanel
	{
		virtual bool Draw() override final
		{
			// you'd need to open the menu to enable the trail anyways, so we do init here (modules init is too early)
			if (!player_trail::initialized)
			{
				// Attrib::Gen::effects::effects
				reinterpret_cast<uintptr_t(__thiscall*)(NFSC::Attrib::Instance&, uint32_t, int)>(0x4131B0)
					(player_trail::carpathfinder, NFSC::Attrib_StringToKey("carpathfinder"), 0);

				player_trail::initialized = true;
			}

			if (ImGui::BulbToys_Menu("Player Trail"))
			{
				static bool enabled = false;
				if (ImGui::Checkbox("Enabled", &enabled))
				{
					uintptr_t my_simable = 0;
					if (!NFSC::BulbToys_GetMyVehicle(nullptr, &my_simable) && enabled)
					{
						enabled = false;
					}

					if (enabled)
					{
						player_trail::effect[0] = NFSC::Sim_Effect_Effect(NFSC::BulbToys_FastMemAlloc(0x44), NFSC::PhysicsObject_GetWorldID(my_simable), 0);
						player_trail::effect[1] = NFSC::Sim_Effect_Effect(NFSC::BulbToys_FastMemAlloc(0x44), NFSC::PhysicsObject_GetWorldID(my_simable), 0);
					}
					else
					{
						if (player_trail::effect[0])
						{
							NFSC::Sim_Effect_vecDelDtor(effect[0], 1);
							player_trail::effect[0] = 0;

							NFSC::Sim_Effect_vecDelDtor(effect[1], 1);
							player_trail::effect[1] = 0;
						}
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
			return new TrailPanel();
		}

		return nullptr;
	}

	HOOK(VIRTUAL, void, __fastcall, AIVehicleHuman_Update, uintptr_t ai_vehicle_human, uintptr_t edx, float dt);

	void Init()
	{
		CREATE_VTABLE_PATCH(0x9C5288, AIVehicleHuman_Update);
	}

	void End()
	{
		Unpatch(0x9C5288);
	}

	void __fastcall AIVehicleHuman_Update_(uintptr_t ai_vehicle_human, uintptr_t edx, float dt)
	{
		AIVehicleHuman_Update(ai_vehicle_human, edx, dt);

		if (player_trail::effect[0])
		{
			NFSC::Vector3 vecB { .0f, 1.0f, .0f };

			auto i_vehicle_ai = ai_vehicle_human + 0x44;
			auto vehicle = NFSC::AIVehicle_GetVehicle(i_vehicle_ai);
			auto simable = NFSC::PVehicle_GetSimable(vehicle);
			auto rigid_body = NFSC::PhysicsObject_GetRigidBody(simable);

			NFSC::Vector3 dim;
			NFSC::RigidBody_GetDimension(rigid_body, &dim);

			NFSC::Vector3 vecA;

			vecA.x = dim.x * -0.5f;
			vecA.y = dim.y * -0.5f;
			vecA.z = dim.z * 0.5f;
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, NFSC::Vector3&, NFSC::Vector3&, uintptr_t, bool, uintptr_t)>(0x767A50)
				(player_trail::effect[0], player_trail::carpathfinder.mCollection, vecA, vecB, 0, true, 0);

			vecA.x = dim.x * 0.5f;
			vecA.y = dim.y * -0.5f;
			vecA.z = dim.z * 0.5f;
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, NFSC::Vector3&, NFSC::Vector3&, uintptr_t, bool, uintptr_t)>(0x767A50)
				(player_trail::effect[1], player_trail::carpathfinder.mCollection, vecA, vecB, 0, true, 0);
		}
	}
}

MODULE(player_trail);
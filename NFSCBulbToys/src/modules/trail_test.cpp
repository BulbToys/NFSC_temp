#include "../../ext/base/bulbtoys.h"
#include "../nfsc.h"

namespace trail_test
{
	class TrailPanel : public IPanel
	{
		static inline bool initialized = false;
	public:
		static inline NFSC::Attrib::Instance carpathfinder;
		static inline uintptr_t effect[2] = { 0, 0 };
	private:
		virtual bool Draw() override final
		{
			if (!initialized)
			{
				// Attrib::Gen::effects::effects
				reinterpret_cast<uintptr_t(__thiscall*)(NFSC::Attrib::Instance&, uint32_t, int)>(0x4131B0)(carpathfinder, NFSC::Attrib_StringToKey("carpathfinder"), 0);
				initialized = true;
			}

			if (ImGui::BulbToys_Menu("Trail Test"))
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
						effect[0] = NFSC::Sim_Effect_Effect(NFSC::BulbToys_FastMemAlloc(0x44), NFSC::PhysicsObject_GetWorldID(my_simable), 0);
						effect[1] = NFSC::Sim_Effect_Effect(NFSC::BulbToys_FastMemAlloc(0x44), NFSC::PhysicsObject_GetWorldID(my_simable), 0);
					}
					else
					{
						if (effect[0])
						{
							NFSC::Sim_Effect_vecDelDtor(effect[0], 1);
							effect[0] = 0;

							NFSC::Sim_Effect_vecDelDtor(effect[1], 1);
							effect[1] = 0;
						}
					}
				}

				ImGui::BulbToys_AddyLabel(carpathfinder.mCollection, "mCollection = ");
				ImGui::BulbToys_AddyLabel(carpathfinder.mLayoutPtr, "mLayoutPtr = ");
				ImGui::Text("mMsgPort = %u", carpathfinder.mMsgPort);
				ImGui::Text("mFlags = %u", carpathfinder.mFlags);
			}

			return true;
		}
	};

	HOOK(VIRTUAL, void, __fastcall, AIVehicleHuman_Update, uintptr_t ai_vehicle_human, uintptr_t edx, float dt);

	void Init()
	{
		CREATE_VTABLE_PATCH(0x9C5288, AIVehicleHuman_Update);
	}

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new TrailPanel();
		}

		return nullptr;
	}

	void End()
	{
		Unpatch(0x9C5288);
	}

	void __fastcall AIVehicleHuman_Update_(uintptr_t ai_vehicle_human, uintptr_t edx, float dt)
	{
		AIVehicleHuman_Update(ai_vehicle_human, edx, dt);

		if (TrailPanel::effect[0])
		{
			NFSC::Vector3 vec2 { .0f, 1.0f, .0f };

			auto i_vehicle_ai = ai_vehicle_human + 0x44;
			auto vehicle = NFSC::AIVehicle_GetVehicle(i_vehicle_ai);
			auto simable = NFSC::PVehicle_GetSimable(vehicle);
			auto rigid_body = NFSC::PhysicsObject_GetRigidBody(simable);

			NFSC::Vector3 dim;
			NFSC::RigidBody_GetDimension(rigid_body, &dim);

			NFSC::Vector3 vec1;

			vec1.x = dim.x * -0.5f;
			vec1.y = dim.y * -0.5f;
			vec1.z = dim.z * 0.5f;
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, NFSC::Vector3&, NFSC::Vector3&, uintptr_t, bool, uintptr_t)>(0x767A50)
				(TrailPanel::effect[0], TrailPanel::carpathfinder.mCollection, vec1, vec2, 0, true, 0);

			vec1.x = dim.x * 0.5f;
			vec1.y = dim.y * -0.5f;
			vec1.z = dim.z * 0.5f;
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, NFSC::Vector3&, NFSC::Vector3&, uintptr_t, bool, uintptr_t)>(0x767A50)
				(TrailPanel::effect[1], TrailPanel::carpathfinder.mCollection, vec1, vec2, 0, true, 0);
		}
	}
}

MODULE(trail_test);
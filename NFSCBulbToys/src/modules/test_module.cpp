#include "../../ext/base/bulbtoys.h"

namespace test_module
{
	class MyPanel : public IPanel
	{
		const char* txt;
		bool checksies = false;
	public:
		MyPanel(const char* txt) : txt(txt) {}

		virtual bool Draw() override final { ImGui::Text(txt); ImGui::Checkbox("Checksies,,,", &checksies); return true; }
	};

	void Init()
	{
		
	}

	IPanel* Panel(Module::DrawType dt)
	{
		if (dt == Module::DrawType::MainWindow)
		{
			return new MyPanel("Hello main window :3");
		}
		else if (dt == Module::DrawType::Overlay)
		{
			return new MyPanel("Hello overlay >w<");
		}

		return nullptr;
	}

	void End()
	{
		
	}
}

MODULE(test_module);
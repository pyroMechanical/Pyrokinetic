#include <Pyrokinetic.h>

class ExampleLayer : public Pyrokinetic::Layer 
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}


	void OnUpdate() override
	{
		PK_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pyrokinetic::Event& event) override
	{
		PK_TRACE("{0}", event);
	}
};

	class Editor : public Pyrokinetic::Application
	{
	public:
		Editor()
		{
			PushLayer(new ExampleLayer());
			PushOverlay(new Pyrokinetic::ImGuiLayer());
		}

		~Editor()
		{

		}

	};

	Pyrokinetic::Application* Pyrokinetic::CreateApplication()
	{
		return new Editor();
	}
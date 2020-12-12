#include <Pyrokinetic.h>

using namespace Pyrokinetic;

	class Editor : public Pyrokinetic::Application
	{
	public:
		Editor()
		{

		}

		~Editor()
		{

		}

	};

	Pyrokinetic::Application* Pyrokinetic::CreateApplication()
	{
		return new Editor();
	}
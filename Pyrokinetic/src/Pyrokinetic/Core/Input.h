#pragma once

#include "pkpch.h"
#include "Pyrokinetic/Core/Core.h"

namespace pk
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button); 
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}

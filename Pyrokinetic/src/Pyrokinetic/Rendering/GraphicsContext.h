#pragma once
//#include "Renderer.h"

namespace Pyrokinetic
{
	class GraphicsContext
	{
	public:
		static std::unique_ptr<GraphicsContext> Create(void* window);

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetVSync(bool enabled) = 0;
	};
}
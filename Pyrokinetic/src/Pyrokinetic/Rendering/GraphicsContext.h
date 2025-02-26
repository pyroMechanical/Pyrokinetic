#pragma once
//#include "Renderer.h"

namespace pk
{
	class GraphicsContext
	{
	public:
		static std::unique_ptr<GraphicsContext> Create(void* window);

		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetVSync(bool enabled) = 0;
	};
}
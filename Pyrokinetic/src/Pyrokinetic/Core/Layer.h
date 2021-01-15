#pragma once

#include "Pyrokinetic/Core/Core.h"
#include "Pyrokinetic/Core/Timestep.h"
#include "Pyrokinetic/Events/Event.h"

namespace Pyrokinetic
{

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timestep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}

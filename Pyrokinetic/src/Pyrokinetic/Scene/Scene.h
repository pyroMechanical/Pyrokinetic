#pragma once

#include "Pyrokinetic/Core/Timestep.h"

#include <entt/entt.hpp>
#include <crossguid/guid.hpp>
namespace pk
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		Entity CreateEntity(const xg::Guid uuid, const std::string& name = std::string());

		void DestroyEntity(entt::entity entity);

		xg::Guid GetEntityUUID(entt::entity entity);

		entt::entity GetEntityFromUUID(xg::Guid uuid);

		void RemoveChild(entt::entity child);

		bool IsParent(entt::entity child, entt::entity parent);

		//TEMPORARY

		void OnUpdate(Timestep t);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
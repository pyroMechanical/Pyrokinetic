#pragma once

#include "Scene.h"

#include <entt/entt.hpp>

#include <memory>

namespace pk
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity id, Scene* scene)
		: m_EntityID(id), m_Scene(scene) {}
		
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			PK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			return m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			PK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");

			return m_Scene->m_Registry.get<T>(m_EntityID);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityID);
		}

		template<typename T>
		void RemoveComponent()
		{
			PK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");

			return m_Scene->m_Registry.remove<T>(m_EntityID);
		}

		operator bool() const { return m_EntityID != entt::null; }

	private:
		entt::entity m_EntityID{ entt::null };
		Scene* m_Scene = nullptr;
	};
}
#pragma once

#include "Scene.h"

#include <entt/entt.hpp>
#include <memory>

#include <glm/glm.hpp>

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
		T& AddOrGetComponent()
		{
			return m_Scene->m_Registry.get_or_emplace<T>(m_EntityID);
		}

		template<typename T>
		T& GetComponentIfExists()
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityID);
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

		void AddChild(Entity child);

		void RemoveChild(Entity child);

		glm::mat4 UpdateTransform(glm::mat4& world, bool dirty);

		operator bool() const { return m_EntityID != entt::null; }

		operator entt::entity() const { return m_EntityID; }

		operator uint32_t() const { return (uint32_t)m_EntityID; }

		bool operator==(const Entity& other) const { return m_EntityID == other.m_EntityID && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityID{ entt::null };
		Scene* m_Scene = nullptr;
	};

	class ScriptableEntity : public Entity
	{
	public:
		virtual ~ScriptableEntity() {};

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Entity.AddComponent<T>();
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			return m_Entity.RemoveComponent<T>()
		}

	protected:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};
		virtual void OnUpdate(Timestep t) {};

	private:
		Entity m_Entity;

		friend class Scene;
	};
}
#include "pkpch.h"
#include "Entity.h"
#include "Components.h"

namespace pk
{
	void Entity::AddChild(Entity child)
	{
		ChildComponent& childComponent = child.AddOrGetComponent<ChildComponent>();
		
		if (childComponent.Parent != entt::null)
		{
			auto& oldParent = m_Scene->m_Registry.get<ParentComponent>(childComponent.Parent);
			auto& prev = m_Scene->m_Registry.get<ChildComponent>(childComponent.Prev);
			auto& next = m_Scene->m_Registry.get<ChildComponent>(childComponent.Next);
			--oldParent.Children;
			if (oldParent.Children > 0)
			{
				prev.Next = childComponent.Next;
				next.Prev = childComponent.Prev;

				if (oldParent.First == child)
					oldParent.First = childComponent.Next;
			}
			else
			{
				m_Scene->m_Registry.remove<ParentComponent>(childComponent.Parent);
			}
		}

		ChildComponent& c = child.GetComponent<ChildComponent>();

		childComponent.Parent = m_EntityID;

		
		ParentComponent& parentComponent = AddOrGetComponent<ParentComponent>();
	
		if (parentComponent.First == entt::null || parentComponent.Children == 0)
		{

			++parentComponent.Children;
			parentComponent.First = child;
	
			childComponent.Next = child;
			childComponent.Prev = child;
		}
		else
		{
			++parentComponent.Children;
			auto& first = m_Scene->m_Registry.get<ChildComponent>(parentComponent.First);
			auto& last = m_Scene->m_Registry.get<ChildComponent>(first.Prev);
			childComponent.Next = parentComponent.First;
			childComponent.Prev = first.Prev;
			first.Prev = child;
			last.Next = child;
		}
	}

	void Entity::RemoveChild(Entity child)
	{
		m_Scene->RemoveChild(child);
	}

	glm::mat4 Entity::UpdateTransform(glm::mat4& world, bool dirty)
	{
		if (HasComponent<TransformComponent>())
		{
			auto& tc = GetComponent<TransformComponent>();
			dirty |= tc.Dirty;
			if (dirty)
			{
				tc.Local = tc.RecalculateMatrix();
				tc.World = world;
				tc.Dirty = false;
				//m_Scene->m_Registry.replace<TransformComponent>(m_EntityID, tc);
			}

			if (HasComponent<ParentComponent>())
			{
				auto& parent = GetComponent<ParentComponent>();
				auto child = parent.First;
				for (int i = 0; i < parent.Children; ++i)
				{
					Entity{ child, m_Scene }.UpdateTransform(world * tc.Local, dirty);
					child = m_Scene->m_Registry.get<ChildComponent>(child).Next;
				}
			}
		}


		return world;
	}
}
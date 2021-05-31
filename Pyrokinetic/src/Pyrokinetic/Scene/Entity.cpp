#include "pkpch.h"
#include "Entity.h"
#include "Components.h"

namespace pk
{
	void Entity::AddChild(Entity child)
	{
		m_Scene->RemoveChild(child);



		ChildComponent& childComponent = child.AddOrGetComponent<ChildComponent>();

		childComponent.Parent = m_EntityID;

		TransformComponent& parentTransform = GetComponent<TransformComponent>();
		TransformComponent& childTransform = child.GetComponent<TransformComponent>();

		child.UpdateTransform(childTransform.GetWorldMatrix(), childTransform.Dirty);
		glm::mat4 comp = glm::inverse(parentTransform.GetWorldMatrix()) * childTransform.GetWorldMatrix();
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		auto success = glm::decompose(comp, scale, rotation, translation, skew, perspective);
		PK_CORE_INFO("Decomposition status: {0}", success);
		childTransform.Translation = translation;
		childTransform.Scale = scale;
		childTransform.EulerAngles = glm::degrees(glm::eulerAngles(glm::conjugate(rotation)));

		childTransform.RecalculateQuaternion();

		glm::mat4 childMatrix = childTransform.GetLocalMatrix();

		PK_CORE_INFO("Composition Matrix:\n [ {0}, {1}, {2}, {3} ]\n[ {4}, {5}, {6}, {7} ]\n[ {8}, {9}, {10}, {11} ]\n[ {12}, {13}, {14}, {15} ]",
			comp[0][0], comp[1][0], comp[2][0], comp[3][0], comp[0][1], comp[1][1], comp[2][1], comp[3][1], comp[0][2], comp[1][2], comp[2][2], comp[3][2], comp[0][3], comp[1][3], comp[2][3], comp[3][3]);

		PK_CORE_INFO("Local Matrix:\n [ {0}, {1}, {2}, {3} ]\n[ {4}, {5}, {6}, {7} ]\n[ {8}, {9}, {10}, {11} ]\n[ {12}, {13}, {14}, {15} ]",
			childMatrix[0][0], childMatrix[1][0], childMatrix[2][0], childMatrix[3][0], childMatrix[0][1], childMatrix[1][1], childMatrix[2][1], childMatrix[3][1], childMatrix[0][2], childMatrix[1][2], childMatrix[2][2], childMatrix[3][2], childMatrix[0][3], childMatrix[1][3], childMatrix[2][3], childMatrix[3][3]);

		childTransform.World = parentTransform.GetWorldMatrix();
		childTransform.Dirty = true;

		childTransform.RecalculateMatrix();

		glm::mat4 childMatrix2 = childTransform.GetLocalMatrix();
		PK_CORE_INFO("Fixed Local Matrix:\n [ {0}, {1}, {2}, {3} ]\n[ {4}, {5}, {6}, {7} ]\n[ {8}, {9}, {10}, {11} ]\n[ {12}, {13}, {14}, {15} ]",
			childMatrix2[0][0], childMatrix2[1][0], childMatrix2[2][0], childMatrix2[3][0], childMatrix2[0][1], childMatrix2[1][1], childMatrix2[2][1], childMatrix2[3][1], childMatrix2[0][2], childMatrix2[1][2], childMatrix2[2][2], childMatrix2[3][2], childMatrix2[0][3], childMatrix2[1][3], childMatrix2[2][3], childMatrix2[3][3]);


		
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
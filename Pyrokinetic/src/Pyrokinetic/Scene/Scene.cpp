#include "pkpch.h"
#include "Scene.h"

#include "Components.h"

#include "Pyrokinetic/Rendering/Renderer2D.h"
#include "Pyrokinetic/Rendering/Camera.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace pk
{
	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>();
		
		if (!name.empty()) entity.AddComponent<TagComponent>().Tag = name;
		return entity;
	}

	Entity Scene::CreateEntity(const xg::Guid uuid, const std::string& name)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);

		if (!name.empty()) entity.AddComponent<TagComponent>().Tag = name;
		return entity;
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		if (m_Registry.all_of<ChildComponent>(entity))
		{
			RemoveChild(entity);
		}
		if (m_Registry.all_of<ParentComponent>(entity))
		{
			auto& parentComponent = m_Registry.get<ParentComponent>(entity);

			auto nextChild = parentComponent.First;
			auto totalChildren = parentComponent.Children;

			auto& tagComponent = m_Registry.get<TagComponent>(entity);
			
			for (int i = 0; i < totalChildren; ++i)
			{
				auto child = nextChild;
				nextChild = m_Registry.get<ChildComponent>(child).Next;

				auto& tag1 = m_Registry.get<TagComponent>(child);
				auto& tag2 = m_Registry.get<TagComponent>(nextChild);
				DestroyEntity(child);
			}
		}
		m_Registry.emplace<DeleteComponent>(entity);
	}

	xg::Guid Scene::GetEntityUUID(entt::entity entity)
	{
		return m_Registry.get<IDComponent>(entity).UUID;
	}

	entt::entity Scene::GetEntityFromUUID(xg::Guid uuid)
	{
		auto view = m_Registry.view<IDComponent>();

		for (auto entity : view)
		{
			if (uuid == GetEntityUUID(entity))
				return entity;
		}
		return entt::null;
	}

	void Scene::RemoveChild(entt::entity child)
	{
		if (!m_Registry.all_of<ChildComponent>(child)) return;
		ChildComponent& childComponent = m_Registry.get<ChildComponent>(child);
		auto& oldParent = m_Registry.get<ParentComponent>(childComponent.Parent);
		auto& prev = m_Registry.get<ChildComponent>(childComponent.Prev);
		auto& next = m_Registry.get<ChildComponent>(childComponent.Next);
		--oldParent.Children;
		TransformComponent& childTransform = m_Registry.get<TransformComponent>(child);
		glm::mat4 comp = childTransform.GetWorldMatrix();
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		auto success = glm::decompose(comp, scale, rotation, translation, skew, perspective);
		childTransform.Translation = translation;
		childTransform.Scale = scale;
		childTransform.EulerAngles = glm::degrees(glm::eulerAngles(glm::conjugate(rotation)));
		childTransform.RecalculateQuaternion();
		childTransform.World = glm::mat4(1.0f);

		if (child == oldParent.First)
		{
			oldParent.First = childComponent.Next;
		}
		if (oldParent.Children > 0)
		{
			prev.Next = childComponent.Next;
			next.Prev = childComponent.Prev;
		}
		else
		{
			m_Registry.remove<ParentComponent>(childComponent.Parent);
		}
		m_Registry.remove<ChildComponent>(child);

		Entity{ child, this }.UpdateTransform(glm::mat4(1.0f), true);

	}

	bool Scene::IsParent(entt::entity child, entt::entity parent)
	{
		entt::entity testing = child;

		bool isParent = false;
		
		while(m_Registry.all_of<ChildComponent>(testing))
		{
			ChildComponent& c = m_Registry.get<ChildComponent>(testing);
			isParent |= (c.Parent == parent);
			testing = c.Parent;
		}

		//PK_CORE_TRACE("{0}, {1}, {2}", isParent, child, parent);

		return isParent;
	}

	void Scene::OnUpdate(Timestep t)
	{
		auto group = m_Registry.group<TransformComponent>(entt::exclude<ChildComponent>);
		
		for (auto entity : group)
		{
			auto entityObject = Entity{ entity, this };
			auto& tc = entityObject.GetComponent<TransformComponent>();
			entityObject.UpdateTransform(glm::mat4(1.0f), tc.Dirty);
		}
		//Update scripts
		/*{
			m_Registry.view<CPPScriptComponent>().each([=](auto entity, auto& sc)
				{
					//TODO: Move to Scene::OnScenePlay
					if (!sc.Instance)
					{
						sc.Instance = sc.InstantiateScript();
						sc.Instance->m_Entity = Entity{ entity, this };
						sc.Instance->OnCreate();
					}

					sc.Instance->OnUpdate(t);
				});


		}*/

		// Render sprites
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);

			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
				
				if(camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.World * glm::translate(glm::mat4(1.0f), transform.Translation) * glm::mat4_cast(transform.Quaternion);
					break;
				}

			}

		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			auto group = m_Registry.view<SpriteRendererComponent, TransformComponent>(); //multiple groups one after the other cause problems. find out why!
			/// <summary>
			/// Multiple groups must be part of the same group "family" if they share a component type; you can't have both group<A, B> and group<A, C> without switching
			/// one of the groups to be weakly owning, such as group<A>(get<B>) and group<A, C>, or group<A, B> and group<A>(get<C>)
			/// </summary>
			/// <param name="t"></param>
				for (auto entity : group)
				{
					auto [transform, sprite] = m_Registry.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawQuad(transform.GetWorldMatrix(), sprite.Color, sprite.Texture);
				}
			Renderer2D::EndScene();
		}

		m_Registry.view<DeleteComponent>().each([=](auto entity) {m_Registry.destroy(entity); });
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		//Resize non-FixedAspectRatio Cameras
		
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}
}
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
		auto& tag = entity.AddComponent<TagComponent>();
		name.empty() ? tag.Tag = "Entity" : tag.Tag = name;
		return entity;
	}

	void Scene::OnUpdate(Timestep t)
	{
		// Render sprites
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();

			for (auto entity : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				
				if(camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}

			}

		}

		if (mainCamera) //test, would be `if (mainCamera)`
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			/* group = m_Registry.group<TransformComponent, SpriteRendererComponent>(); //multiple groups one after the other cause problems. find out why!
			for (auto spriteEntity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(spriteEntity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}*/

			Renderer2D::EndScene();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		//Resize non-FixedAspectRatio Cameras
		
		//auto& view = m_Registry.view<CameraComponent>();
		/*for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}*/
	}
}
#pragma once


#include "Entity.h"
#include "SceneCamera.h"

#include "Pyrokinetic/Rendering/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Pyrokinetic/Scene/SceneCamera.h>

namespace pk
{

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f };
		glm::vec3 EulerAngles{ 0.0f };
		glm::quat Quaternion = glm::quat(glm::radians(EulerAngles));
		glm::vec3 Scale{ 1.0f };
		glm::mat4 World{ 1.0f };
		glm::mat4 Local{ 1.0f };
		bool Dirty = true;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			:Translation(translation) {}

		void RecalculateQuaternion()
		{
			Quaternion = glm::quat(glm::radians(EulerAngles));
			Dirty = true;
		}

		glm::mat4 RecalculateMatrix()
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::mat4_cast(Quaternion)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetWorldMatrix() const
		{
			return World * Local;
		}
		glm::mat4 GetLocalMatrix() const
		{
			return Local;
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<Texture2D> Texture = nullptr;
		
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			:Color(color) {}
		SpriteRendererComponent(const std::shared_ptr<Texture2D> texture)
			:Texture(texture) {}
		SpriteRendererComponent(const glm::vec4& color, const std::shared_ptr<Texture2D> texture)
			:Color(color), Texture(texture) {}
	};
	
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct CPPScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(CPPScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](CPPScriptComponent* sc) { delete sc->Instance; sc->Instance = nullptr; };
		}
	};
	
	struct ParentComponent
	{
		std::size_t Children{};
		entt::entity First{ entt::null };

		ParentComponent() = default;
		ParentComponent(const ParentComponent&) = default;
		ParentComponent(Entity child)
			:First(child), Children(1) {}
	};

	struct ChildComponent
	{
		entt::entity Parent{ entt::null };
		entt::entity Prev{ entt::null };
		entt::entity Next{ entt::null };

		ChildComponent() = default;
		ChildComponent(const ChildComponent&) = default;
		ChildComponent(entt::entity parent)
			:Parent(parent) {}
	};

	struct DeleteComponent
	{
		DeleteComponent() = default;
	};
}
#include "pkpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace pk
{

	YAML::Emitter&  operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
		:m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity, Scene* scene)
	{
		out << YAML::BeginMap; //Entity Map;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; //Tag

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; //Tag
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; //Transform

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.EulerAngles;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; //Transform
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; //Sprite

			auto& sprite = entity.GetComponent<SpriteRendererComponent>();

			out << YAML::Key << "Color" << YAML::Value << sprite.Color;

			std::string textureFile;

			if (sprite.Texture) textureFile = sprite.Texture->GetTexture()->GetPath();
			else textureFile = "NULL";
			out << YAML::Key << "Texture" << YAML::Value << textureFile;
			out << YAML::EndMap; //Sprite
		}

		if (entity.HasComponent<CameraComponent>())
		{
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; //Camera

			out << YAML::Key << "ProjectionType" << YAML::Value << (uint32_t)camera.GetProjectionType();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::Key << "VerticalFOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "InfiniteFarPlane" << YAML::Value << camera.IsFarPlaneInfinite();

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; //Camera
		}

		if (entity.HasComponent<ParentComponent>())
		{
			auto& parent = entity.GetComponent<ParentComponent>();

			out << YAML::Key << "ParentComponent";
			out << YAML::BeginMap; //Parent
			out << YAML::Key << "Children" << YAML::Value << parent.Children;
			out << YAML::Key << "First" << YAML::Value << Entity{ parent.First, scene }.GetUUID();
			out << YAML::EndMap; //Parent
		}

		if (entity.HasComponent<ChildComponent>())
		{
			auto& child = entity.GetComponent<ChildComponent>();

			out << YAML::Key << "ChildComponent";
			out << YAML::BeginMap; //Child
			out << YAML::Key << "Parent" << YAML::Value << Entity{ child.Parent,  scene }.GetUUID();
			out << YAML::Key << "Previous" << YAML::Value << Entity{ child.Prev, scene }.GetUUID();
			out << YAML::Key << "Next" << YAML::Value << Entity{ child.Next, scene }.GetUUID();
			out << YAML::EndMap; //Child
		}

		out << YAML::EndMap; //Entity Map
	}

	void SceneSerializer::SerializeText(const std::string& path)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "<Scene Name>";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity) return;
				SerializeEntity(out, entity, m_Scene.get());
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeBinary(const std::string& path)
	{
		PK_CORE_ASSERT(false, "Binary Serialization Not implemented!");
	}

	bool SceneSerializer::DeserializeText(const std::string& path)
	{
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		PK_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				std::string uuid = entity["Entity"].as<std::string>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntity(xg::Guid(uuid), name);

				auto transformComponent = entity["TransformComponent"];
				if(transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.EulerAngles = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					tc.RecalculateQuaternion();
					tc.Dirty = true;
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if(spriteRendererComponent)
				{
					auto& sprite = deserializedEntity.AddOrGetComponent<SpriteRendererComponent>();
					sprite.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if(spriteRendererComponent["Texture"].as<std::string>() != "NULL")
						sprite.Texture = SubTexture2D::CreateFromPath(spriteRendererComponent["Texture"].as<std::string>());
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddOrGetComponent<CameraComponent>();
					cc.Camera.SetProjectionType(cameraComponent["ProjectionType"].as<int>());
					cc.Camera.SetPerspectiveFOV(cameraComponent["VerticalFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraComponent["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraComponent["PerspectiveFar"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraComponent["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraComponent["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
			}
			//adding children needs to be done after all entities are created!
			for (auto entity : entities)
			{
				auto deserializedEntity = m_Scene->GetEntityFromUUID(xg::Guid(entity["Entity"].as<std::string>()));
				if (deserializedEntity != entt::null)
				{
					auto entObj = Entity{ deserializedEntity, m_Scene.get() };

					auto parentComponent = entity["ParentComponent"];
					if (parentComponent)
					{
						auto& pc = entObj.AddOrGetComponent<ParentComponent>();
						pc.Children = parentComponent["Children"].as<size_t>();
						pc.First = m_Scene->GetEntityFromUUID(xg::Guid(parentComponent["First"].as<std::string>()));
					}

					auto childComponent = entity["ChildComponent"];
					if(childComponent)
					{
						auto& cc = entObj.AddOrGetComponent<ChildComponent>();
						cc.Parent = m_Scene->GetEntityFromUUID(xg::Guid(childComponent["Parent"].as<std::string>()));
						cc.Next = m_Scene->GetEntityFromUUID(xg::Guid(childComponent["Next"].as<std::string>()));
						cc.Prev = m_Scene->GetEntityFromUUID(xg::Guid(childComponent["Previous"].as<std::string>()));
					}
				}
			}
		}

		return false;
	}

	bool SceneSerializer::DeserializeBinary(const std::string& path)
	{
		PK_CORE_ASSERT(false, "Binary Deserialization Not implemented!");

		return false;
	}
}
#pragma once

#include "Scene.h"


namespace pk
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const std::shared_ptr<Scene>& scene);

		void SerializeText(const std::string& path);
		void SerializeBinary(const std::string& path);

		bool DeserializeText(const std::string& path);
		bool DeserializeBinary(const std::string& path);

	private:
		std::shared_ptr<Scene> m_Scene;
	};
}
#include "pkpch.h"
#include "AssetManager.h"

#include "Pyrokinetic/Rendering/Texture.h"

namespace pk
{
	
	static std::vector<std::string> ImageTypes = { ".png", ".jpg" };
	static std::vector<std::string> MeshTypes = { ".obj", ".blend" };
	static std::vector<std::string> ShaderTypes = { ".glsl" };
	static std::vector<std::string> SoundTypes = { ".wav", ".mp3", ".ogg" };

	xg::Guid AssetManager::CreateAsset(std::string& path, xg::Guid id)
	{
		auto assetID = s_AssetIDs.find(path);
		if (assetID != s_AssetIDs.end())
		{
			PK_CORE_WARN("Asset at ID {0} already exists!", s_AssetIDs.at(path));
			return s_AssetIDs.at(path);
		}

		s_AssetIDs[path] = id;

		auto lastDot = path.rfind(".");
		auto type = path.substr(lastDot, path.length());
		bool isImage = false;
		for (auto imgType : ImageTypes)
		{
			if (type == imgType) isImage = true;
		}

		if(isImage)
		{
			auto asset = std::dynamic_pointer_cast<Asset>(SubTexture2D::CreateFromPath(path));
			asset->SetUUID(id);

			s_Assets[id] = asset;

			return id;
		}

		return xg::Guid();
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(xg::Guid id)
	{
		if (s_Assets.find(id) != s_Assets.end()) 
			return s_Assets.at(id);
		return nullptr;
	}


}
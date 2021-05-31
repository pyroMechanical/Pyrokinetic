#pragma once

#include "Asset.h"
#include "Pyrokinetic/Core/FileBrowser.h"

#include <unordered_map>
#include <memory>

namespace pk
{
	class AssetManager
	{
	public:
		static std::shared_ptr<Asset> GetAsset(xg::Guid id);

		static xg::Guid CreateAsset(std::string& path, xg::Guid id = xg::newGuid());

		static std::unordered_map<xg::Guid, std::shared_ptr<Asset>>& GetAssetList() { return s_Assets; }

	private:
		inline static std::unordered_map<xg::Guid, std::shared_ptr<Asset>> s_Assets;

		inline static std::unordered_map<std::string, xg::Guid> s_AssetIDs;

		friend class SceneSerializer;
	};
}
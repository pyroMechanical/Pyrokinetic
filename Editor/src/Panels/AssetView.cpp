#include "AssetView.h"

#include <imgui.h>

#include <imgui_internal.h>

namespace pk
{
	void AssetView::OnImGuiRender()
	{
		ImGui::Begin("Assets");

		std::unordered_map<xg::Guid, std::shared_ptr<Asset>>& assets = AssetManager::GetAssetList();

		for(auto& [id, asset] : assets)
		{
			
		}

	}
}

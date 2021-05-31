#pragma once

#include "Pyrokinetic.h"

namespace pk
{
	class AssetView
	{
		AssetView() = default;
		~AssetView() = default;

		void OnImGuiRender();
	};
}
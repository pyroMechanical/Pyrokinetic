#pragma once

#include <crossguid/guid.hpp>

namespace pk
{
	class Asset
	{
	public:
		virtual ~Asset() = default;

		xg::Guid GetUUID() { return UUID; }
		
		void SetUUID(std::string& id) { UUID = xg::Guid(id); }

		void SetUUID(xg::Guid id) { UUID = id; }

	protected:
		xg::Guid UUID;
	};
}
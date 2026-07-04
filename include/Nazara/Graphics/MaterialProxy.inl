// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline std::size_t MaterialProxy::GetBindingSetCount() const
	{
		return m_bindingSetHashes.size();
	}

	inline std::size_t MaterialProxy::GetBindingSetHash(UInt32 setIndex) const
	{
		return m_bindingSetHashes[setIndex];
	}
}


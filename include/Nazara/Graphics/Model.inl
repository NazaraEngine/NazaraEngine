// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t Model::GetSubMeshCount() const
	{
		return m_subMeshes.size();
	}
	
	inline void Model::SetMaterial(std::size_t subMeshIndex, std::shared_ptr<Material> material)
	{
		assert(subMeshIndex < m_subMeshes.size());

		OnMaterialInvalidated(this, subMeshIndex, material);
		m_subMeshes[subMeshIndex].material = std::move(material);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t Model::GetSubMeshCount() const
	{
		return m_submeshes.size();
	}
	
	inline void Model::SetMaterial(std::size_t subMeshIndex, std::shared_ptr<Material> material)
	{
		assert(subMeshIndex < m_submeshes.size());
		assert(material);

		if (m_submeshes[subMeshIndex].material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_submeshes[subMeshIndex].material = std::move(material);

			OnElementInvalidated(this);
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

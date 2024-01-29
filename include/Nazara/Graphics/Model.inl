// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t Model::GetSubMeshCount() const
	{
		return m_submeshes.size();
	}

	inline void Model::SetMaterial(std::size_t subMeshIndex, std::shared_ptr<MaterialInstance> material)
	{
		NazaraAssertFmt(subMeshIndex < m_submeshes.size(), "submesh index out of range ({0} >= {1})", subMeshIndex, m_submeshes.size());
		NazaraAssert(material, "invalid material");

		if (m_submeshes[subMeshIndex].material != material)
		{
			OnMaterialInvalidated(this, subMeshIndex, material);
			m_submeshes[subMeshIndex].material = std::move(material);

			OnElementInvalidated(this);
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

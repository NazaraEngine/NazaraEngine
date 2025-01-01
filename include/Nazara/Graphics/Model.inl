// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline std::size_t Model::GetSubMeshCount() const
	{
		return m_submeshes.size();
	}

	inline void Model::SetIndexCount(std::size_t submeshIndex, std::size_t indexCount)
	{
		NazaraAssertMsg(submeshIndex < m_submeshes.size(), "submesh index out of range (%zu >= %zu)", submeshIndex, m_submeshes.size());

		if (m_submeshes[submeshIndex].indexCount != indexCount)
		{
			m_submeshes[submeshIndex].indexCount = indexCount;
			OnElementInvalidated(this);
		}
	}

	inline void Model::SetMaterial(std::size_t submeshIndex, std::shared_ptr<MaterialInstance> material)
	{
		NazaraAssertMsg(submeshIndex < m_submeshes.size(), "submesh index out of range (%zu >= %zu)", submeshIndex, m_submeshes.size());
		NazaraAssertMsg(material, "invalid material");

		if (m_submeshes[submeshIndex].material != material)
		{
			OnMaterialInvalidated(this, submeshIndex, material);
			m_submeshes[submeshIndex].material = std::move(material);

			OnElementInvalidated(this);
		}
	}
}

// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	template <typename T>
	SparsePtr<T> VertexMapper::GetComponentPtr(VertexComponent component, std::size_t componentIndex)
	{
		// On récupère la déclaration depuis le buffer
		const std::shared_ptr<const VertexDeclaration>& declaration = m_mapper.GetBuffer()->GetVertexDeclaration();

		if (const auto* componentData = declaration->GetComponentByType<T>(component, componentIndex))
			return SparsePtr<T>(static_cast<UInt8*>(m_mapper.GetPointer()) + componentData->offset, declaration->GetStride());
		else
			return SparsePtr<T>();
	}

	inline const VertexBuffer* VertexMapper::GetVertexBuffer() const
	{
		return m_mapper.GetBuffer();
	}

	inline UInt32 VertexMapper::GetVertexCount() const
	{
		return GetVertexBuffer()->GetVertexCount();
	}

	template<typename T>
	bool VertexMapper::HasComponentOfType(VertexComponent component) const
	{
		return m_mapper.GetBuffer()->GetVertexDeclaration()->HasComponentOfType<T>(component);
	}
}

#include <Nazara/Utility/DebugOff.hpp>

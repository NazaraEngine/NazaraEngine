// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Debug.hpp>
#include <Nazara/Utility/Algorithm.hpp>

namespace Nz
{
	template <typename T>
	SparsePtr<T> VertexMapper::GetComponentPtr(VertexComponent component)
	{
		// On récupère la déclaration depuis le buffer
		const VertexDeclaration* declaration = m_mapper.GetBuffer()->GetVertexDeclaration();

		// Ensuite le composant qui nous intéresse
		bool enabled;
		ComponentType type;
		std::size_t offset;
		declaration->GetComponent(component, &enabled, &type, &offset);

		if (enabled)
		{
			///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
			NazaraAssert(ComponentTypeOf<T>() == type, "Wanted type is not the same than the declaration");
			return SparsePtr<T>(static_cast<UInt8*>(m_mapper.GetPointer()) + offset, declaration->GetStride());
		}
		else
		{
			NazaraError("Attribute 0x" + String::Number(component, 16) + " is not enabled");
			return SparsePtr<T>();
		}
	}

	template<typename T> 
	bool VertexMapper::HaveComponent(VertexComponent component) const
	{
		return m_mapper.GetBuffer()->GetVertexDeclaration()->HaveComponent<T>(component);
	}
}

#include <Nazara/Utility/DebugOff.hpp>

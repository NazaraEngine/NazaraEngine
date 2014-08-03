// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

template <typename T>
NzSparsePtr<T> NzVertexMapper::GetComponentPtr(nzVertexComponent component)
{
	// On récupère la déclaration depuis le buffer
	const NzVertexDeclaration* declaration = m_mapper.GetBuffer()->GetVertexDeclaration();

	// Ensuite le composant qui nous intéresse
	bool enabled;
	nzComponentType type;
	unsigned int offset;
	declaration->GetComponent(component, &enabled, &type, &offset);

	if (enabled)
	{
		///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
		return NzSparsePtr<T>(static_cast<nzUInt8*>(m_mapper.GetPointer()) + offset, declaration->GetStride());
	}
	else
	{
		NazaraError("Attribute 0x" + NzString::Number(component, 16) + " is not enabled");
		return NzSparsePtr<T>();
	}
}

#include <Nazara/Utility/DebugOff.hpp>

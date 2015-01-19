// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

template <typename T>
NzSparsePtr<T> NzParticleMapper::GetComponentPtr(nzParticleComponent component)
{
	// Ensuite le composant qui nous intéresse
	bool enabled;
	nzComponentType type;
	unsigned int offset;
	m_declaration->GetComponent(component, &enabled, &type, &offset);

	if (enabled)
	{
		///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
		return NzSparsePtr<T>(m_ptr + offset, m_declaration->GetStride());
	}
	else
	{
		NazaraError("Attribute 0x" + NzString::Number(component, 16) + " is not enabled");
		return NzSparsePtr<T>();
	}
}

template <typename T>
NzSparsePtr<const T> NzParticleMapper::GetComponentPtr(nzParticleComponent component) const
{
	// Ensuite le composant qui nous intéresse
	bool enabled;
	nzComponentType type;
	unsigned int offset;
	m_declaration->GetComponent(component, &enabled, &type, &offset);

	if (enabled)
	{
		///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
		return NzSparsePtr<const T>(m_ptr + offset, m_declaration->GetStride());
	}
	else
	{
		NazaraError("Attribute 0x" + NzString::Number(component, 16) + " is not enabled");
		return NzSparsePtr<const T>();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

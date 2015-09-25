// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	template <typename T>
	SparsePtr<T> ParticleMapper::GetComponentPtr(ParticleComponent component)
	{
		// Ensuite le composant qui nous intéresse
		bool enabled;
		ComponentType type;
		unsigned int offset;
		m_declaration->GetComponent(component, &enabled, &type, &offset);

		if (enabled)
		{
			///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
			return SparsePtr<T>(m_ptr + offset, m_declaration->GetStride());
		}
		else
		{
			NazaraError("Attribute 0x" + String::Number(component, 16) + " is not enabled");
			return SparsePtr<T>();
		}
	}

	template <typename T>
	SparsePtr<const T> ParticleMapper::GetComponentPtr(ParticleComponent component) const
	{
		// Ensuite le composant qui nous intéresse
		bool enabled;
		ComponentType type;
		unsigned int offset;
		m_declaration->GetComponent(component, &enabled, &type, &offset);

		if (enabled)
		{
			///TODO: Vérifier le rapport entre le type de l'attribut et le type template ?
			return SparsePtr<const T>(m_ptr + offset, m_declaration->GetStride());
		}
		else
		{
			NazaraError("Attribute 0x" + String::Number(component, 16) + " is not enabled");
			return SparsePtr<const T>();
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

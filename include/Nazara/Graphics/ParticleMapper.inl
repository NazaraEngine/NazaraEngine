// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>
#include <Nazara/Utility/Algorithm.hpp>

namespace Nz
{
	/*!
	* \brief Gets a pointer to iterate through same components
	* \return SparsePtr pointing to same components
	*
	* \param component Component to get in the declaration
	*
	* \remark The same components are not continguous but separated by sizeof(ParticleSize)
	* \remark Produces a NazaraError if component is disabled
	*/

	template <typename T>
	SparsePtr<T> ParticleMapper::GetComponentPtr(ParticleComponent component)
	{
		// Then the component that are interesting
		bool enabled;
		ComponentType type;
		std::size_t offset;
		m_declaration->GetComponent(component, &enabled, &type, &offset);

		if (enabled && ComponentTypeOf<T>() == type)
		{
			///TODO: Check the ratio between the type of the attribute and the template type ?
			return SparsePtr<T>(m_ptr + offset, m_declaration->GetStride());
		}
		else
		{
			NazaraError("Attribute 0x" + String::Number(component, 16) + " is not enabled");
			return SparsePtr<T>();
		}
	}

	/*!
	* \brief Gets a pointer to iterate through same components
	* \return SparsePtr pointing to same components
	*
	* \param component Component to get in the declaration
	*
	* \remark The same components are not continguous but separated by sizeof(ParticleSize)
	* \remark Produces a NazaraError if component is disabled
	*/

	template <typename T>
	SparsePtr<const T> ParticleMapper::GetComponentPtr(ParticleComponent component) const
	{
		// Then the component that are interesting
		bool enabled;
		ComponentType type;
		std::size_t offset;
		m_declaration->GetComponent(component, &enabled, &type, &offset);

		if (enabled && ComponentTypeOf<T>() == type)
		{
			///TODO: Check the ratio between the type of the attribute and the template type ?
			return SparsePtr<const T>(m_ptr + offset, m_declaration->GetStride());
		}
		else
		{
			NazaraError("Attribute 0x" + String::Number(component, 16) + " is not enabled");
			return SparsePtr<const T>();
		}
	}

	/*!
	* \brief Gets a raw pointer to the particle buffer
	*
	* This can be useful when working directly with a struct
	*
	* \return Pointer to the buffer
	*/
	inline void* ParticleMapper::GetPointer()
	{
		return m_ptr;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

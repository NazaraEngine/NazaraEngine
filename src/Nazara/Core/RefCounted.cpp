// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::RefCounted
	* \brief Core class that represents a reference with a counter
	*/

	/*!
	* \brief Constructs a RefCounted object with a persistance aspect
	*
	* \param persistent if false, object is destroyed when no more referenced
	*/

	RefCounted::RefCounted(bool persistent) :
	m_persistent(persistent),
	m_referenceCount(0)
	{
	}

	/*!
	* \brief Destructs the object
	*
	* \remark Produces a NazaraWarning if still referenced with NAZARA_CORE_SAFE defined
	*/

	RefCounted::~RefCounted()
	{
		#if NAZARA_CORE_SAFE
		if (m_referenceCount > 0)
			NazaraWarning("Resource destroyed while still referenced " + String::Number(m_referenceCount) + " time(s)");
		#endif
	}

	/*!
	* \brief Adds a reference to the object
	*/

	void RefCounted::AddReference() const
	{
		m_referenceCount++;
	}

	/*!
	* \brief Gets the number of references to the object
	* \return Number of references
	*/

	unsigned int RefCounted::GetReferenceCount() const
	{
		return m_referenceCount;
	}

	/*!
	* \brief Checks whether the object is persistent
	* \return true if object is not destroyed when no more referenced
	*/

	bool RefCounted::IsPersistent() const
	{
		return m_persistent;
	}

	/*!
	* \brief Removes a reference to the object
	* \return true if object is deleted because no more referenced
	*
	* \remark Produces a NazaraError if counter is already 0 with NAZARA_CORE_SAFE defined
	*/

	bool RefCounted::RemoveReference() const
	{
		#if NAZARA_CORE_SAFE
		if (m_referenceCount == 0)
		{
			NazaraError("Impossible to remove reference (Ref. counter is already 0)");
			return false;
		}
		#endif

		if (--m_referenceCount == 0 && !m_persistent)
		{
			delete this; // Suicide

			return true;
		}
		else
			return false;
	}

	/*!
	* \brief Sets the persistence of the object
	* \return true if object is deleted because no more referenced
	*
	* \param persistent Sets the persistence of the object
	* \param checkReferenceCount Checks if the object should be destroyed if true
	*/

	bool RefCounted::SetPersistent(bool persistent, bool checkReferenceCount)
	{
		m_persistent = persistent;

		if (checkReferenceCount && !persistent && m_referenceCount == 0)
		{
			delete this;

			return true;
		}
		else
			return false;
	}
}

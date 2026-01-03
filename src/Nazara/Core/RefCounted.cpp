// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>

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
	*/

	RefCounted::~RefCounted()
	{
		#ifdef NAZARA_DEBUG
		if (m_referenceCount > 0)
			NazaraWarning("Resource destroyed while still referenced {0} time(s)", m_referenceCount);
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
	*/

	bool RefCounted::RemoveReference() const
	{
		NazaraAssertMsg(m_referenceCount > 0, "impossible to remove reference (Ref. counter is already 0)");

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

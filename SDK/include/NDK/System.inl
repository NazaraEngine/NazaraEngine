// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::System<SystemType>
	* \brief NDK class that represents a system which interacts on a world
	*
	* \remark This class is meant to be derived as CRTP: "System<Subtype>"
	*/

	/*!
	* \brief Constructs a System object by default
	*/

	template<typename SystemType>
	System<SystemType>::System() :
	BaseSystem(GetSystemIndex<SystemType>())
	{
	}

	template<typename SystemType>
	System<SystemType>::~System() = default;

	/*!
	* \brief Clones the system
	* \return The clone newly created
	*
	* \remark The system to clone should be trivially copy constructible
	*/

	template<typename SystemType>
	std::unique_ptr<BaseSystem> System<SystemType>::Clone() const
	{
		///FIXME: Not fully supported in GCC (4.9.2)
		//static_assert(std::is_trivially_copy_constructible<SystemType>::value, "SystemType should be copy-constructible");

		return std::make_unique<SystemType>(static_cast<const SystemType&>(*this));
	}

	/*!
	* \brief Registers the system by assigning it an index
	*/

	template<typename SystemType>
	SystemIndex System<SystemType>::RegisterSystem()
	{
		return GetNextIndex();
	}
}

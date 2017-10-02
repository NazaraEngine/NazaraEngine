// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::VelocityComponent
	* \brief NDK class that represents the component for velocity
	*/

	/*!
	* \brief Constructs a VelocityComponent object with a velocity
	*
	* \param velocity Linear velocity
	*/

	inline VelocityComponent::VelocityComponent(const Nz::Vector3f& velocity) :
	linearVelocity(velocity)
	{
	}

	/*!
	* \brief Assigns the velocity to this component
	* \return A reference to this
	*
	* \param vel Linear velocity
	*/

	inline VelocityComponent& VelocityComponent::operator=(const Nz::Vector3f& vel)
	{
		linearVelocity = vel;
		return *this;
	}
}

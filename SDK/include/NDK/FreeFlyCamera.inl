// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	/*!
	* \brief Enable/Disable smoothed movement
	*
	* When enabled, the movements won't stop as soon as the key is released, but there will be some slow-down until complete stops
	*
	* \see IsMovementSmoothed
	*/

	inline void FreeFlyCamera::EnableSmoothedMovement(bool smooth)
	{
		m_camera->GetComponent<Ndk::VelocityComponent>().damped = smooth;
	}

	/*!
	* \brief Get the mouse sensitivity for rotations
	* \return The mouse sensitivity
	* \see SetSensitivity
	*/

	inline float FreeFlyCamera::GetSensitivity() const
	{
		return m_sensitivity;
	}

	/*!
	* \brief Get the movement speed
	* \return The speed
	* \see SetSpeed
	*/

	inline float FreeFlyCamera::GetSpeed() const
	{
		return m_speed;
	}
	
	/*!
	* \brief Checks wether this free fly camera has smoothed movements
	* \return true If the movements are smoothed
	* \see EnableSmoothedMovement
	*/

	inline bool FreeFlyCamera::IsMovementSmoothed() const
	{
		return m_camera->GetComponent<Ndk::VelocityComponent>().damped;
	}

	/*!
	* \brief Set the mouse sensitivity for rotations
	* \see GetSensitivity
	*/

	inline void FreeFlyCamera::SetSensitivity(float sensitivity)
	{
		m_sensitivity = sensitivity;
	}

	/*!
	* \brief Set the movement speed
	* \see GetSpeed
	*/

	inline void FreeFlyCamera::SetSpeed(float speed)
	{
		m_speed = speed;
	}
}
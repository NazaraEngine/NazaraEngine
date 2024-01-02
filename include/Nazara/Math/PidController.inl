// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T>
	constexpr PidController<T>::PidController(float p, float i, float d) :
	m_lastError(0),
	m_integral(0),
	m_dFactor(d),
	m_iFactor(i),
	m_pFactor(p)
	{
	}

	template<typename T>
	constexpr T PidController<T>::Update(const T& currentError, float elapsedTime)
	{
		m_integral += currentError * elapsedTime;
		T deriv = (currentError - m_lastError) / elapsedTime;
		m_lastError = currentError;

		return currentError * m_pFactor + m_integral * m_iFactor + deriv * m_dFactor;
	}
}

#include <Nazara/Core/DebugOff.hpp>

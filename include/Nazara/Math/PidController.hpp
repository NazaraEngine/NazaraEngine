// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_PIDCONTROLLER_HPP
#define NAZARA_MATH_PIDCONTROLLER_HPP

namespace Nz
{
	template<typename T>
	class PidController
	{
		public:
			constexpr PidController(float p, float i, float d);

			constexpr T Update(const T& currentError, float elapsedTime);

		private:
			T m_lastError;
			T m_integral;
			float m_dFactor;
			float m_iFactor;
			float m_pFactor;
	};
}

#include <Nazara/Math/PidController.inl>

#endif // NAZARA_MATH_PIDCONTROLLER_HPP

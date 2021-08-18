// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIDCONTROLLER_HPP
#define NAZARA_PIDCONTROLLER_HPP

namespace Nz
{
	template<typename T>
	class PidController
	{
		public:
			PidController(float p, float i, float d);

			T Update(const T& currentError, float elapsedTime);

		private:
			T m_lastError;
			T m_integral;
			float m_dFactor;
			float m_iFactor;
			float m_pFactor;
	};
}

#include <Nazara/Math/PidController.inl>

#endif

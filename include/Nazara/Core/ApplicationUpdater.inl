// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationUpdater.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename F>
	ApplicationUpdaterFunctor<F>::ApplicationUpdaterFunctor(F functor) :
	m_functor(std::move(functor))
	{
	}

	template<typename F>
	void ApplicationUpdaterFunctor<F>::Update(Time elapsedTime)
	{
		m_functor(elapsedTime);
	}
}

#include <Nazara/Core/DebugOff.hpp>

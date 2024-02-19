// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	template<typename F>
	ApplicationUpdaterFunctor<F>::ApplicationUpdaterFunctor(F functor) :
	m_functor(std::move(functor))
	{
	}

	template<typename F>
	Time ApplicationUpdaterFunctor<F>::Update(Time elapsedTime)
	{
		if constexpr (std::is_invocable_v<F, Time>)
			return TriggerFunctor(elapsedTime);
		else if constexpr (std::is_invocable_v<F>)
			return TriggerFunctor();
		else
			static_assert(AlwaysFalse<F>(), "updater functor must be callable with a Time or nothing");
	}

	template<typename F>
	template<typename... Args>
	Time ApplicationUpdaterFunctor<F>::TriggerFunctor(Args&&... args)
	{
		if constexpr (std::is_same_v<std::invoke_result_t<F, Args...>, Time>)
			return m_functor(std::forward<Args>(args)...);
		else if constexpr (std::is_same_v<std::invoke_result_t<F, Args...>, void>)
		{
			m_functor(std::forward<Args>(args)...);
			return Time::Zero();
		}
		else
			static_assert(AlwaysFalse<F>(), "updater functor must either return Time or void");
	}


	template<typename F, bool FixedInterval>
	ApplicationUpdaterFunctorWithInterval<F, FixedInterval>::ApplicationUpdaterFunctorWithInterval(F functor, Time interval) :
	m_interval(interval),
	m_functor(std::move(functor))
	{
	}

	template<typename F, bool FixedInterval>
	Time ApplicationUpdaterFunctorWithInterval<F, FixedInterval>::Update(Time elapsedTime)
	{
		if constexpr (std::is_invocable_v<F, Time>)
		{
			if constexpr (FixedInterval)
				m_functor(m_interval);
			else
				m_functor(elapsedTime);
		}
		else
		{
			static_assert(std::is_invocable_v<F>, "updater functor must be callable with a Time or nothing");
			m_functor();
		}

		if constexpr (FixedInterval)
			return -m_interval;
		else
			return m_interval;
	}
}


// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FUNCTOR_HPP
#define NAZARA_CORE_FUNCTOR_HPP

#include <tuple>
#include <utility>

// Inspired from the of code of the SFML by Laurent Gomila

namespace Nz
{
	struct AbstractFunctor
	{
		virtual ~AbstractFunctor() {}

		virtual void Run() = 0;
	};

	template<typename F>
	struct FunctorWithoutArgs : AbstractFunctor
	{
		FunctorWithoutArgs(F func);

		void Run() override;

		private:
			F m_func;
	};

	template<typename F, typename... Args>
	struct FunctorWithArgs : AbstractFunctor
	{
		FunctorWithArgs(F func, Args&&... args);

		void Run() override;

		private:
			F m_func;
			std::tuple<Args...> m_args;
	};

	template<typename C>
	struct MemberWithoutArgs : AbstractFunctor
	{
		MemberWithoutArgs(void (C::*func)(), C* object);

		void Run() override;

		private:
			void (C::*m_func)();
			C* m_object;
	};
}

#include <Nazara/Core/Functor.inl>

#endif // NAZARA_CORE_FUNCTOR_HPP

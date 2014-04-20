// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CALLONEXIT_HPP
#define NAZARA_CALLONEXIT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <functional>

class NzCallOnExit : NzNonCopyable
{
	using Func = std::function<void()>;

	public:
		NzCallOnExit(Func func = nullptr);
		~NzCallOnExit();

		void Reset(Func func = nullptr);

	private:
		Func m_func;
};

#include <Nazara/Core/CallOnExit.inl>

#endif // NAZARA_CALLONEXIT_HPP

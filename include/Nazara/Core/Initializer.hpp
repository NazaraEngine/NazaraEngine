// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INITIALIZER_HPP
#define NAZARA_INITIALIZER_HPP

#include <Nazara/Prerequesites.hpp>

template<typename... Args>
class NzInitializer
{
	public:
		NzInitializer();
		~NzInitializer();

		bool IsInitialized() const;

		operator bool() const;

	private:
		bool m_initialized;
};

#include <Nazara/Core/Initializer.inl>

#endif // NAZARA_INITIALIZER_HPP

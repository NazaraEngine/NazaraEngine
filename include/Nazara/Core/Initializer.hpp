// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INITIALIZER_HPP
#define NAZARA_INITIALIZER_HPP

#include <Nazara/Prerequesites.hpp>

template<typename T>
class NzInitializer
{
	public:
		template<typename... Args> NzInitializer(Args... args);
		~NzInitializer();

		bool IsInitialized() const;

		operator bool() const;
};

#include <Nazara/Core/Initializer.inl>

#endif // NAZARA_INITIALIZER_HPP

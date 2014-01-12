// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMAT_HPP
#define NAZARA_FORMAT_HPP

#include <Nazara/Core/String.hpp>

template<typename... Args> NzString NzFormat(const NzString& str, Args... args);

#endif // NAZARA_FORMAT_HPP

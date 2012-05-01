// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMAT_HPP
#define NAZARA_FORMAT_HPP

#define NAZARA_FORMAT

#include <Nazara/Core/String.hpp>

template<typename... Args> NzString NzFormat(const NzString& str, Args... args);

#undef NAZARA_FORMAT

#endif // NAZARA_FORMAT_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETBUFFER_HPP
#define NAZARA_NETBUFFER_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	struct NetBuffer
	{
		void* data;
		std::size_t dataLength;
	};
}

#endif // NAZARA_NETBUFFER_HPP

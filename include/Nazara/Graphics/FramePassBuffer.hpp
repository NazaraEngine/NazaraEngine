// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASSBUFFER_HPP
#define NAZARA_GRAPHICS_FRAMEPASSBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <string>

namespace Nz
{
	struct FramePassBuffer
	{
		std::string name;
		UInt64 size;
		BufferUsageFlags additionalUsages;
		bool persistent = true;
	};
}

#endif // NAZARA_GRAPHICS_FRAMEPASSBUFFER_HPP

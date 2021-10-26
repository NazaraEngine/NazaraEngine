// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRAMEPASSATTACHMENT_HPP
#define NAZARA_FRAMEPASSATTACHMENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <string>

namespace Nz
{
	struct FramePassAttachment
	{
		std::string name;
		PixelFormat format;
		unsigned int width = 100'000;
		unsigned int height = 100'000;
	};
}

#include <Nazara/Graphics/FramePassAttachment.inl>

#endif

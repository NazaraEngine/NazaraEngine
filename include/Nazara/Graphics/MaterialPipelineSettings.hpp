// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALPIPELINESETTINGS_HPP
#define NAZARA_MATERIALPIPELINESETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <string>
#include <vector>

namespace Nz
{
	struct MaterialSettings
	{
		struct SharedUniformBlocks
		{
			String name;
			std::string bindingPoint;
		};

		struct Texture
		{
			String name;
			ImageType type;
			std::string bindingPoint;
		};

		struct UniformBlocks
		{
			String name;
			std::size_t blockSize;
			std::string bindingPoint;
		};

		std::vector<SharedUniformBlocks> sharedUniformBlocks;
		std::vector<Texture> textures;
		std::vector<UniformBlocks> uniformBlocks;
	};
}

#endif // NAZARA_MATERIALPIPELINESETTINGS_HPP

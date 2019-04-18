// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPipelineSettings.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t MaterialSettings::GetSharedUniformBlockIndex(const String& name) const
	{
		for (std::size_t i = 0; i < sharedUniformBlocks.size(); ++i)
		{
			if (sharedUniformBlocks[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline std::size_t MaterialSettings::GetTextureIndex(const String& name) const
	{
		for (std::size_t i = 0; i < textures.size(); ++i)
		{
			if (textures[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline std::size_t MaterialSettings::GetUniformBlockIndex(const String & name) const
	{
		for (std::size_t i = 0; i < uniformBlocks.size(); ++i)
		{
			if (uniformBlocks[i].name == name)
				return i;
		}

		return InvalidIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

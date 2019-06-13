// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPipelineSettings.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t MaterialSettings::GetSharedUniformBlockVariableOffset(std::size_t uniformBlockIndex, const String & name)
	{
		assert(uniformBlockIndex < sharedUniformBlocks.size());

		std::vector<UniformVariable>& variables = sharedUniformBlocks[uniformBlockIndex].uniforms;
		for (std::size_t i = 0; i < variables.size(); ++i)
		{
			if (variables[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

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

	inline std::size_t MaterialSettings::GetUniformBlockVariableOffset(std::size_t uniformBlockIndex, const String & name)
	{
		assert(uniformBlockIndex < uniformBlocks.size());

		std::vector<UniformVariable>& variables = uniformBlocks[uniformBlockIndex].uniforms;
		for (std::size_t i = 0; i < variables.size(); ++i)
		{
			if (variables[i].name == name)
				return i;
		}

		return InvalidIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

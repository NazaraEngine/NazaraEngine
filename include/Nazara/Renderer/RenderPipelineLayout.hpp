// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINELAYOUT_HPP
#define NAZARA_RENDERPIPELINELAYOUT_HPP

#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <string>
#include <vector>

namespace Nz
{
	struct RenderPipelineLayoutInfo
	{
		struct Binding
		{
			std::string name; //< FIXME: wtf is this?
			ShaderBindingType type;
			ShaderStageTypeFlags shaderStageFlags;
			unsigned int index;
		};

		std::vector<Binding> bindings;
	};

	class ShaderBinding;

	class NAZARA_RENDERER_API RenderPipelineLayout
	{
		public:
			RenderPipelineLayout() = default;
			virtual ~RenderPipelineLayout();

			virtual ShaderBinding& AllocateShaderBinding() = 0;
	};
}

#include <Nazara/Renderer/RenderPipelineLayout.inl>

#endif // NAZARA_RENDERPIPELINELAYOUT_HPP

// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERPIPELINELAYOUT_HPP
#define NAZARA_RENDERER_RENDERPIPELINELAYOUT_HPP

#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NZSL/Enums.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Nz
{
	struct RenderPipelineLayoutInfo
	{
		struct Binding
		{
			UInt32 setIndex = 0;
			UInt32 bindingIndex;
			UInt32 arraySize = 1;
			ShaderBindingType type;
			nzsl::ShaderStageTypeFlags shaderStageFlags;
		};

		std::vector<Binding> bindings;
	};

	class NAZARA_RENDERER_API RenderPipelineLayout
	{
		public:
			RenderPipelineLayout() = default;
			virtual ~RenderPipelineLayout();

			virtual ShaderBindingPtr AllocateShaderBinding(UInt32 setIndex) = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;
	};
}

#include <Nazara/Renderer/RenderPipelineLayout.inl>

#endif // NAZARA_RENDERER_RENDERPIPELINELAYOUT_HPP

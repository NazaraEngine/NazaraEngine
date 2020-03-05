// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERDEVICEINSTANCE_HPP
#define NAZARA_RENDERDEVICEINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <memory>
#include <string>

namespace Nz
{
	class Buffer;
	class ShaderStageImpl;

	class NAZARA_RENDERER_API RenderDevice
	{
		public:
			RenderDevice() = default;
			virtual ~RenderDevice();

			virtual std::unique_ptr<AbstractBuffer> InstantiateBuffer(Buffer* parent, BufferType type) = 0;
			virtual std::unique_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) = 0;
			virtual std::shared_ptr<ShaderStageImpl> InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize) = 0;
			std::shared_ptr<ShaderStageImpl> InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const std::filesystem::path& sourcePath);
	};
}

#include <Nazara/Renderer/RenderDevice.inl>

#endif // NAZARA_RENDERDEVICEINSTANCE_HPP

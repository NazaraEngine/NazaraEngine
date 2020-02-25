// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERDEVICEINSTANCE_HPP
#define NAZARA_RENDERDEVICEINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <memory>

namespace Nz
{
	class Buffer;

	class NAZARA_RENDERER_API RenderDevice
	{
		public:
			RenderDevice() = default;
			virtual ~RenderDevice();

			virtual std::unique_ptr<AbstractBuffer> InstantiateBuffer(Buffer* parent, BufferType type) = 0;
			virtual std::unique_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) = 0;
	};
}

#include <Nazara/Renderer/RenderDevice.inl>

#endif // NAZARA_RENDERDEVICEINSTANCE_HPP

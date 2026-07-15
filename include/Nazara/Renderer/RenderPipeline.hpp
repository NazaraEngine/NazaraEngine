// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERPIPELINE_HPP
#define NAZARA_RENDERER_RENDERPIPELINE_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	struct RenderPipelineInfo : RenderStates
	{
		struct VertexBufferData
		{
			std::size_t binding;
			std::shared_ptr<const VertexDeclaration> declaration;
		};

		using ShaderVector = HybridVector<std::shared_ptr<ShaderModule>, 2>;
		using VertexInputVector = HybridVector<VertexBufferData, 1>;

		std::shared_ptr<RenderPipelineLayout> pipelineLayout;
		ShaderVector shaderModules;
		VertexInputVector vertexBuffers;
	};

	class RenderDevice;

	class NAZARA_RENDERER_API RenderPipeline
	{
		public:
			RenderPipeline() = default;
			virtual ~RenderPipeline();

			virtual const RenderPipelineInfo& GetPipelineInfo() const = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;

			NazaraSignal(OnRenderPipelineRelease, RenderPipeline* /*emitter*/);

		protected:
			static void ValidatePipelineInfo(const RenderDevice& device, RenderPipelineInfo& pipelineInfo);
	};
}

#include <Nazara/Renderer/RenderPipeline.inl>

#endif // NAZARA_RENDERER_RENDERPIPELINE_HPP

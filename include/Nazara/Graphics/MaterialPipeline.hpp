// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALPIPELINE_HPP
#define NAZARA_MATERIALPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <array>
#include <memory>

namespace Nz
{
	class UberShader;

	struct MaterialPipelineInfo : RenderStates
	{
		struct ShaderStage
		{
			std::shared_ptr<UberShader> uberShader;
			Nz::UInt64 enabledConditions = 0;
		};

		bool alphaTest         = false;
		bool depthSorting      = false;
		bool hasVertexColor    = false;
		bool reflectionMapping = false;
		bool shadowReceive     = true;

		std::array<ShaderStage, ShaderStageTypeCount> shaders;
		std::shared_ptr<const MaterialSettings> settings;
	};

	inline bool operator==(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs);
	inline bool operator!=(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs);


	class NAZARA_GRAPHICS_API MaterialPipeline
	{
		friend class Graphics;

		struct Token {};

		public:
			inline MaterialPipeline(const MaterialPipelineInfo& pipelineInfo, Token);
			MaterialPipeline(const MaterialPipeline&) = delete;
			MaterialPipeline(MaterialPipeline&&) = delete;
			~MaterialPipeline() = default;

			MaterialPipeline& operator=(const MaterialPipeline&) = delete;
			MaterialPipeline& operator=(MaterialPipeline&&) = delete;

			inline const MaterialPipelineInfo& GetInfo() const;
			const std::shared_ptr<RenderPipeline>& GetRenderPipeline(const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers) const;

			static const std::shared_ptr<MaterialPipeline>& Get(const MaterialPipelineInfo& pipelineInfo);

		private:
			static bool Initialize();
			static void Uninitialize();

			mutable std::vector<std::shared_ptr<RenderPipeline>> m_renderPipelines;
			MaterialPipelineInfo m_pipelineInfo;

			using PipelineCache = std::unordered_map<MaterialPipelineInfo, std::shared_ptr<MaterialPipeline>>;
			static PipelineCache s_pipelineCache;
	};
}

#include <Nazara/Graphics/MaterialPipeline.inl>

#endif // NAZARA_MATERIALPIPELINE_HPP

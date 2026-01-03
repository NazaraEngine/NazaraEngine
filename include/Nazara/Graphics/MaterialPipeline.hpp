// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPIPELINE_HPP
#define NAZARA_GRAPHICS_MATERIALPIPELINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <array>
#include <memory>

namespace Nz
{
	class UberShader;

	struct MaterialPipelineInfo : RenderStates
	{
		struct Option
		{
			UInt32 hash;
			nzsl::Ast::ConstantSingleValue value;
		};

		struct Shader
		{
			std::shared_ptr<UberShader> uberShader;
		};

		std::shared_ptr<RenderPipelineLayout> pipelineLayout;
		FixedVector<Option, 32> optionValues;
		FixedVector<Shader, 8> shaders;
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
			const std::shared_ptr<RenderPipeline>& GetRenderPipeline(const RenderPipelineInfo::VertexBufferData* vertexBuffers, std::size_t vertexBufferCount) const;

			static const std::shared_ptr<MaterialPipeline>& Get(const MaterialPipelineInfo& pipelineInfo);

		private:
			static bool Initialize();
			static void Uninitialize();

			struct UberShaderEntry
			{
				NazaraSlot(UberShader, OnShaderUpdated, onShaderUpdated);
			};

			mutable std::vector<std::shared_ptr<RenderPipeline>> m_renderPipelines;
			std::vector<UberShaderEntry> m_uberShaderEntries;
			MaterialPipelineInfo m_pipelineInfo;

			using PipelineCache = std::unordered_map<MaterialPipelineInfo, std::shared_ptr<MaterialPipeline>>;
			static PipelineCache s_pipelineCache;
	};
}

#include <Nazara/Graphics/MaterialPipeline.inl>

#endif // NAZARA_GRAPHICS_MATERIALPIPELINE_HPP

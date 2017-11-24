// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALPIPELINE_HPP
#define NAZARA_MATERIALPIPELINE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <array>

namespace Nz
{
	struct MaterialPipelineInfo : RenderStates
	{
		bool alphaTest         = false;
		bool depthSorting      = false;
		bool hasAlphaMap       = false;
		bool hasDiffuseMap     = false;
		bool hasEmissiveMap    = false;
		bool hasHeightMap      = false;
		bool hasNormalMap      = false;
		bool hasSpecularMap    = false;
		bool reflectionMapping = false;
		bool shadowReceive     = true;

		UberShaderConstRef uberShader;
	};

	inline bool operator==(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs);
	inline bool operator!=(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs);

	class MaterialPipeline;

	using MaterialPipelineConstRef = ObjectRef<const MaterialPipeline>;
	using MaterialPipelineLibrary = ObjectLibrary<MaterialPipeline>;
	using MaterialPipelineRef = ObjectRef<MaterialPipeline>;

	class NAZARA_GRAPHICS_API MaterialPipeline : public RefCounted
	{
		friend class Graphics;
		friend MaterialPipelineLibrary;

		public:
			struct Instance;

			MaterialPipeline(const MaterialPipeline&) = delete;
			MaterialPipeline(MaterialPipeline&&) = delete;
			~MaterialPipeline() = default;

			inline const Instance& Apply(UInt32 flags = ShaderFlags_None) const;

			MaterialPipeline& operator=(const MaterialPipeline&) = delete;
			MaterialPipeline& operator=(MaterialPipeline&&) = delete;

			inline const MaterialPipelineInfo& GetInfo() const;
			inline const Instance& GetInstance(UInt32 flags = ShaderFlags_None) const;

			static MaterialPipelineRef GetPipeline(const MaterialPipelineInfo& pipelineInfo);

			struct Instance
			{
				RenderPipeline renderPipeline;
				UberShaderInstance* uberInstance = nullptr;
				std::array<int, MaterialUniform_Max + 1> uniforms;
			};

		private:
			inline MaterialPipeline(const MaterialPipelineInfo& pipelineInfo);

			void GenerateRenderPipeline(UInt32 flags) const;

			static bool Initialize();
			template<typename... Args> static MaterialPipelineRef New(Args&&... args);
			static void Uninitialize();

			MaterialPipelineInfo m_pipelineInfo;
			mutable std::array<Instance, ShaderFlags_Max + 1> m_instances;

			using PipelineCache = std::unordered_map<MaterialPipelineInfo, MaterialPipelineRef>;
			static PipelineCache s_pipelineCache;

			static MaterialPipelineLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/MaterialPipeline.inl>

#endif // NAZARA_MATERIALPIPELINE_HPP

// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LIGHTINGPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_LIGHTINGPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <memory>

namespace Nz
{
	class AbstractViewer;
	class FrameGraph;
	class FramePipeline;
	class Light;
	class GraphicalMesh;
	class RenderDevice;
	class RenderFrame;
	class RenderPipeline;
	class RenderPipelineLayout;
	class ShaderBinding;
	class UberShader;

	class NAZARA_GRAPHICS_API LightingPipelinePass : public FramePipelinePass
	{
		public:
			LightingPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters);
			LightingPipelinePass(PassData& passData, std::string passName, std::string shaderName);
			LightingPipelinePass(const LightingPipelinePass&) = delete;
			LightingPipelinePass(LightingPipelinePass&&) = delete;
			~LightingPipelinePass();

			inline void InvalidateCommandBuffers();
			inline void InvalidateLights();

			void Prepare(FrameData& frameData) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			LightingPipelinePass& operator=(const LightingPipelinePass&) = delete;
			LightingPipelinePass& operator=(LightingPipelinePass&&) = delete;

			static std::string GetShaderName(const ParameterList& parameters);

		private:
			void SetupMeshes();
			void SetupPipelineLayouts(RenderDevice& renderDevice, const std::string& shaderName);
			void SetupPipelines(RenderDevice& renderDevice, std::string&& shaderName);

			struct LightBlockMemory
			{
				std::shared_ptr<RenderBuffer> lightUbo;
				ShaderBindingPtr shaderBinding;
			};

			struct LightBlockMemoryShadow
			{
				std::shared_ptr<RenderBuffer> lightUbo;
				std::vector<ShaderBindingPtr> shaderBindings;
			};

			struct LightBufferPool
			{
				std::vector<LightBlockMemory> directionalLightPool;
				std::vector<LightBlockMemory> pointLightPool;
				std::vector<LightBlockMemory> spotLightPool;
				std::vector<LightBlockMemoryShadow> shadowDirectionalLightPool;
				std::vector<LightBlockMemoryShadow> shadowPointLightPool;
				std::vector<LightBlockMemoryShadow> shadowSpotLightPool;
			};

			struct LightBlock
			{
				std::size_t lightCount = 0;
				LightBlockMemory memory;
				UploadPool::Allocation* uploadAllocation;
			};

			struct LightBlockShadow
			{
				std::size_t lightCount = 0;
				LightBlockMemoryShadow memory;
				UploadPool::Allocation* uploadAllocation;
			};

			struct LightPipeline
			{
				std::shared_ptr<RenderPipeline> lightingPipeline;
				std::shared_ptr<RenderPipeline> lightingPipelineShadow;
				std::shared_ptr<RenderPipeline> stencilPipeline;
				std::shared_ptr<RenderPipeline> stencilPipelineShadow;
			};

			void* PushLightData(RenderDevice& renderDevice, UInt64 maxLight, std::vector<LightBlockMemory>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlock>& lights, UInt64 lightSize);
			void* PushLightData(RenderDevice& renderDevice, UInt64 maxLight, std::vector<LightBlockMemoryShadow>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlockShadow>& lights, UInt64 lightSize, const Texture* shadowMap, const TextureSampler* shadowMapSampler);
			void ReleaseLights(std::vector<LightBlockMemory>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlock>& lights);
			void ReleaseLights(std::vector<LightBlockMemoryShadow>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlockShadow>& lights);

			std::shared_ptr<GraphicalMesh> m_pointLightMesh;
			std::shared_ptr<GraphicalMesh> m_spotLightMesh;
			std::shared_ptr<LightBufferPool> m_lightBufferPool;
			std::shared_ptr<RenderPipelineLayout> m_commonPipelineLayout;
			std::shared_ptr<RenderPipelineLayout> m_shadowPipelineLayout;
			std::shared_ptr<UberShader> m_fullscreenVertexShader;
			std::shared_ptr<UberShader> m_lightingShader;
			std::shared_ptr<UberShader> m_meshStencilShader;
			std::size_t m_lastVisibilityHash;
			std::string m_passName;
			std::vector<LightBlock> m_directionalLights;
			std::vector<LightBlock> m_pointLights;
			std::vector<LightBlock> m_spotLights;
			std::vector<LightBlockShadow> m_shadowDirectionalLights;
			std::vector<LightBlockShadow> m_shadowPointLights;
			std::vector<LightBlockShadow> m_shadowSpotLights;
			EnumArray<BasicLightType, LightPipeline> m_pipelines;
			FixedVector<UInt32, 8> m_gbufferBindingIndices;
			UInt32 m_depthMapBindingIndex;
			UInt32 m_directionalLightSize;
			UInt32 m_lightDataBindingIndex;
			UInt32 m_lightShadowmapBindingIndex;
			UInt32 m_pointLightSize;
			UInt32 m_spotLightSize;
			UInt32 m_viewerDataBindingIndex;
			ShaderBindingPtr m_commonShaderBinding;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
			bool m_rebuildCommandBuffer;
			bool m_rebuildLights;
	};
}

#include <Nazara/Graphics/LightingPipelinePass.inl>

#endif // NAZARA_GRAPHICS_LIGHTINGPIPELINEPASS_HPP

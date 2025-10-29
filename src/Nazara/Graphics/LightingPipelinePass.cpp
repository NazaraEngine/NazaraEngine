// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/LightingPipelinePass.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShaderTransfer.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <NZSL/Ast/Cloner.hpp>
#include <NZSL/Ast/TransformerExecutor.hpp>
#include <NZSL/Ast/Transformations/BindingResolverTransformer.hpp>
#include <NZSL/Ast/Transformations/ResolveTransformer.hpp>
#include <NZSL/Ast/Transformations/ValidationTransformer.hpp>

namespace Nz
{
	LightingPipelinePass::LightingPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	LightingPipelinePass(passData, std::move(passName), GetShaderName(parameters))
	{
	}

	LightingPipelinePass::LightingPipelinePass(PassData& passData, std::string passName, std::string shaderName) :
	FramePipelinePass(0),
	m_lastVisibilityHash(0),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_pipeline(passData.pipeline),
	m_rebuildCommandBuffer(false),
	m_rebuildLights(false)
	{
		Graphics* graphics = Graphics::Instance();
		const auto& renderDevice = graphics->GetRenderDevice();

		UInt64 uboAlignment = renderDevice->GetDeviceInfo().limits.minUniformBufferOffsetAlignment;
		m_directionalLightSize = SafeCaster(AlignPow2<UInt64>(PredefinedDirectionalLightOffsets.totalSize, uboAlignment));
		m_pointLightSize = SafeCaster(AlignPow2<UInt64>(PredefinedPointLightOffsets.totalSize, uboAlignment));
		m_spotLightSize = SafeCaster(AlignPow2<UInt64>(PredefinedSpotLightOffsets.totalSize, uboAlignment));

		SetupMeshes();
		SetupPipelineLayouts(*renderDevice, shaderName);
		SetupPipelines(*renderDevice, std::move(shaderName));

		m_lightBufferPool = std::make_shared<LightBufferPool>();
	}

	void LightingPipelinePass::Prepare(FrameData& frameData)
	{
		std::size_t lightVisibilityHash = 0;
		for (std::size_t blockIndex = 0; blockIndex < frameData.visibleLights->GetBlockCount(); ++blockIndex)
			HashCombine(lightVisibilityHash, frameData.visibleLights->GetBlock(blockIndex));

		//if (m_lastVisibilityHash != lightVisibilityHash || m_rebuildLights)
		{
			Graphics* graphics = Graphics::Instance();
			const auto& renderDevice = graphics->GetRenderDevice();

			ReleaseLights(m_lightBufferPool->directionalLightPool, frameData.renderResources, m_directionalLights);
			ReleaseLights(m_lightBufferPool->pointLightPool, frameData.renderResources, m_pointLights);
			ReleaseLights(m_lightBufferPool->spotLightPool, frameData.renderResources, m_spotLights);

			ReleaseLights(m_lightBufferPool->shadowDirectionalLightPool, frameData.renderResources, m_shadowDirectionalLights);
			ReleaseLights(m_lightBufferPool->shadowPointLightPool, frameData.renderResources, m_shadowPointLights);
			ReleaseLights(m_lightBufferPool->shadowSpotLightPool, frameData.renderResources, m_shadowSpotLights);

			const auto& defaultSampler = graphics->GetSamplerCache().Get({});
			const auto& depthSampler = graphics->GetSamplerCache().Get({ .depthCompare = true });

			for (std::size_t lightIndex : frameData.visibleLights->IterBits())
			{
				const Light* light = m_pipeline.RetrieveLight(lightIndex);
				const Texture* shadowMap = m_pipeline.RetrieveLightShadowmap(lightIndex, m_viewer);

				constexpr UInt8 DirectionalLightType = static_cast<UInt8>(BasicLightType::Directional);
				constexpr UInt8 PointLightType = static_cast<UInt8>(BasicLightType::Point);
				constexpr UInt8 SpotLightType = static_cast<UInt8>(BasicLightType::Spot);

				switch (light->GetLightType())
				{
					case DirectionalLightType:
					{
						void* lightData;
						if (shadowMap)
							lightData = PushLightData(*renderDevice, 256, m_lightBufferPool->shadowDirectionalLightPool, frameData.renderResources, m_shadowDirectionalLights, m_directionalLightSize, shadowMap, depthSampler.get());
						else
							lightData = PushLightData(*renderDevice, 256, m_lightBufferPool->directionalLightPool, frameData.renderResources, m_directionalLights, m_directionalLightSize);

						ShaderTransfer::WriteLight(SafeCast<const DirectionalLight*>(light), lightData);
						break;
					}

					case PointLightType:
					{
						void* lightData;
						if (shadowMap)
							lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->shadowPointLightPool, frameData.renderResources, m_shadowPointLights, m_pointLightSize, shadowMap, defaultSampler.get());
						else
							lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->pointLightPool, frameData.renderResources, m_pointLights, m_pointLightSize);

						ShaderTransfer::WriteLight(SafeCast<const PointLight*>(light), lightData);
						break;
					}

					case SpotLightType:
					{
						void* lightData;
						if (shadowMap)
							lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->shadowSpotLightPool, frameData.renderResources, m_shadowSpotLights, m_spotLightSize, shadowMap, depthSampler.get());
						else
							lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->spotLightPool, frameData.renderResources, m_spotLights, m_spotLightSize);

						ShaderTransfer::WriteLight(SafeCast<const SpotLight*>(light), lightData);
						break;
					}
				}
			}

			frameData.renderResources.Execute([&](CommandBufferBuilder& builder)
			{
				for (auto& lightBlockContainer : { &m_directionalLights, &m_pointLights, &m_spotLights })
				{
					for (auto& lightBlock : *lightBlockContainer)
						builder.CopyBuffer(*lightBlock.uploadAllocation, RenderBufferView(lightBlock.memory.lightUbo.get(), 0, lightBlock.lightCount * m_directionalLightSize));
				}
				
				for (auto& lightBlockContainer : { &m_shadowDirectionalLights, &m_shadowPointLights, &m_shadowSpotLights })
				{
					for (auto& lightBlock : *lightBlockContainer)
						builder.CopyBuffer(*lightBlock.uploadAllocation, RenderBufferView(lightBlock.memory.lightUbo.get(), 0, lightBlock.lightCount * m_directionalLightSize));
				}

				builder.MemoryBarrier(PipelineStage::Transfer, PipelineStage::VertexInput, MemoryAccess::TransferWrite, MemoryAccess::VertexBufferRead);
			}, QueueType::Transfer);

			m_rebuildCommandBuffer = true;
			m_rebuildLights = false;

			m_lastVisibilityHash = lightVisibilityHash;
		}
	}

	FramePass& LightingPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		std::size_t inputCount = m_gbufferBindingIndices.size();
		if (m_depthMapBindingIndex != MaxValue<UInt32>())
			inputCount++;

		if (inputOuputs.inputAttachments.size() != inputCount)
			throw std::runtime_error("at least one input expected");

		Graphics* graphics = Graphics::Instance();

		const auto& sampler = graphics->GetSamplerCache().Get({});

		FramePass& lightingPass = frameGraph.AddPass(m_passName);

		HybridVector<std::size_t, 8> inputAttachmentIndices;
		for (const auto& inputData : inputOuputs.inputAttachments)
		{
			lightingPass.AddInput(inputData.attachmentIndex);
			inputAttachmentIndices.push_back(inputData.attachmentIndex);
		}

		// We expect the last input to be the depth buffer, if one exists
		if (m_depthMapBindingIndex != MaxValue<UInt32>())
			lightingPass.SetInputAccess(inputOuputs.inputAttachments.size() - 1, TextureLayout::DepthReadOnlyStencilReadWrite, PipelineStage::FragmentShader, MemoryAccess::ShaderRead);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = lightingPass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					lightingPass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					lightingPass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			lightingPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					lightingPass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					lightingPass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		lightingPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		lightingPass.SetExecutionCallback([&]()
		{
			return (m_rebuildCommandBuffer) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		lightingPass.SetCommandCallback([this, &sampler, inputCount, inputAttachmentIndices](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			env.renderResources.PushForRelease(std::move(m_commonShaderBinding));

			const auto& viewerBuffer = m_viewer->GetViewerInstance().GetViewerBuffer();

			StackVector<ShaderBinding::Binding> bindings = NazaraStackVector(ShaderBinding::Binding, inputCount + 1);
			bindings.push_back({
				0,
				ShaderBinding::UniformBufferBinding {
					viewerBuffer.get(),
					0, viewerBuffer->GetSize()
				}
			});

			std::size_t gbufferInputCount = inputCount;
			if (m_depthMapBindingIndex != MaxValue<UInt32>())
				gbufferInputCount--;

			for (std::size_t i = 0; i < gbufferInputCount; ++i)
			{
				const auto& attachmentTexture = env.frameGraph.GetAttachmentTexture(inputAttachmentIndices[i]);

				bindings.push_back({
					m_gbufferBindingIndices[i],
					ShaderBinding::SampledTextureBinding {
						attachmentTexture.get(), sampler.get(),
					}
				});
			}

			if (m_depthMapBindingIndex != MaxValue<UInt32>())
			{
				const auto& attachmentTexture = env.frameGraph.GetAttachmentTexture(inputAttachmentIndices[gbufferInputCount]);

				bindings.push_back({
					m_depthMapBindingIndex,
					ShaderBinding::SampledTextureBinding {
						attachmentTexture.get(), sampler.get(), TextureLayout::DepthReadOnlyStencilReadWrite
					}
				});
			}

			m_commonShaderBinding = m_commonPipelineLayout->AllocateShaderBinding(0);
			m_commonShaderBinding->Update(bindings.data(), bindings.size());

			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			builder.BindRenderShaderBinding(0, *m_commonShaderBinding);

			if (!m_directionalLights.empty())
			{
				builder.BindRenderPipeline(*m_pipelines[BasicLightType::Directional].lightingPipeline);

				for (const auto& directionalLight : m_directionalLights)
				{
					for (std::size_t i = 0; i < directionalLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_directionalLightSize);

						builder.BindRenderShaderBinding(1, *directionalLight.memory.shaderBinding, std::span(&lightOffset, 1));
						builder.Draw(3);
					}
				}
			}

			if (!m_shadowDirectionalLights.empty())
			{
				builder.BindRenderPipeline(*m_pipelines[BasicLightType::Directional].lightingPipelineShadow);

				for (const auto& directionalLight : m_shadowDirectionalLights)
				{
					for (std::size_t i = 0; i < directionalLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_directionalLightSize);

						builder.BindRenderShaderBinding(1, *directionalLight.memory.shaderBindings[i], std::span(&lightOffset, 1));
						builder.Draw(3);
					}
				}
			}

			if (!m_pointLights.empty() || !m_shadowPointLights.empty())
			{
				builder.BindIndexBuffer(*m_pointLightMesh->GetIndexBuffer(0), IndexType::U16);
				builder.BindVertexBuffer(0, *m_pointLightMesh->GetVertexBuffer(0));
				UInt32 indexCount = m_pointLightMesh->GetIndexCount(0);

				for (const auto& pointLight : m_pointLights)
				{
					for (std::size_t i = 0; i < pointLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_pointLightSize);

						builder.BindRenderShaderBinding(1, *pointLight.memory.shaderBinding, std::span(&lightOffset, 1));

						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Point].stencilPipeline);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Point].lightingPipeline);
						builder.DrawIndexed(indexCount);
					}
				}

				for (const auto& pointLight : m_shadowPointLights)
				{
					for (std::size_t i = 0; i < pointLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_pointLightSize);

						builder.BindRenderShaderBinding(1, *pointLight.memory.shaderBindings[i], std::span(&lightOffset, 1));

						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Point].stencilPipelineShadow);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Point].lightingPipelineShadow);
						builder.DrawIndexed(indexCount);
					}
				}
			}

			if (!m_spotLights.empty() || !m_shadowSpotLights.empty())
			{
				builder.BindIndexBuffer(*m_spotLightMesh->GetIndexBuffer(0), IndexType::U16);
				builder.BindVertexBuffer(0, *m_spotLightMesh->GetVertexBuffer(0));
				UInt32 indexCount = m_spotLightMesh->GetIndexCount(0);

				for (const auto& spotLight : m_spotLights)
				{
					for (std::size_t i = 0; i < spotLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_spotLightSize);

						builder.BindRenderShaderBinding(1, *spotLight.memory.shaderBinding, std::span(&lightOffset, 1));

						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Spot].stencilPipeline);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Spot].lightingPipeline);
						builder.DrawIndexed(indexCount);
					}
				}

				for (const auto& spotLight : m_shadowSpotLights)
				{
					for (std::size_t i = 0; i < spotLight.lightCount; ++i)
					{
						UInt32 lightOffset = SafeCaster(i * m_spotLightSize);

						builder.BindRenderShaderBinding(1, *spotLight.memory.shaderBindings[i], std::span(&lightOffset, 1));

						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Spot].stencilPipelineShadow);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_pipelines[BasicLightType::Spot].lightingPipelineShadow);
						builder.DrawIndexed(indexCount);
					}
				}
			}

			m_rebuildCommandBuffer = false;
		});

		return lightingPass;
	}

	std::string LightingPipelinePass::GetShaderName(const ParameterList& parameters)
	{
		Result<std::string, ParameterList::Error> shaderResult = parameters.GetStringParameter("Shader");
		if (shaderResult.IsOk())
			return std::move(shaderResult).GetValue();
		// TODO: Log error if key is present but not of the right type

		throw std::runtime_error("LightingPipelinePass expect a Shader parameter");
	}

	void LightingPipelinePass::SetupMeshes()
	{
		MeshParams meshPrimitiveParams;
		meshPrimitiveParams.bufferFactory = &SoftwareBufferFactory;
		meshPrimitiveParams.vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ);

		// Point mesh
		{
			Mesh mesh;
			mesh.CreateStatic();
			mesh.BuildSubMesh(Primitive::IcoSphere(1.f), meshPrimitiveParams);
			mesh.SetMaterialCount(1);

			m_pointLightMesh = GraphicalMesh::BuildFromMesh(mesh);
		}

		// Spotlight mesh
		{
			Mesh mesh;
			mesh.CreateStatic();
			mesh.BuildSubMesh(Primitive::Cone(1.f, 1.f, 16, Matrix4f::Rotate(EulerAnglesf(90.f, 0.f, 0.f))), meshPrimitiveParams);
			mesh.SetMaterialCount(1);

			m_spotLightMesh = GraphicalMesh::BuildFromMesh(mesh);
		}
	}

	void LightingPipelinePass::SetupPipelineLayouts(RenderDevice& renderDevice, const std::string& shaderName)
	{
		using namespace nzsl::Ast::Literals;

		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics);

		nzsl::Ast::ModulePtr referenceModule = graphics->GetShaderModuleResolver()->Resolve(shaderName);

		nzsl::Ast::TransformerExecutor executor;
		executor.AddPass<nzsl::Ast::ResolveTransformer>({ .moduleResolver = graphics->GetShaderModuleResolver() });
		executor.AddPass<nzsl::Ast::BindingResolverTransformer>({ .forceAutoBindingResolve = true });
		executor.AddPass<nzsl::Ast::ValidationTransformer>();

		nzsl::Ast::TransformerContext context;
		context.partialCompilation = true;
		context.optionValues["Light"_opt] = Int32(0); //< just to avoid unresolved externals
		context.optionValues["EnableShadowMapping"_opt] = true;
		context.optionValues["MaxLightCount"_opt] = SafeCast<UInt32>(PredefinedLightData::MaxLightCount);
		context.optionValues["MaxLightCascadeCount"_opt] = SafeCast<UInt32>(PredefinedDirectionalLightData::MaxLightCascadeCount);
		context.optionValues["MaxJointCount"_opt] = SafeCast<UInt32>(PredefinedSkeletalData::MaxMatricesCount);

		nzsl::Ast::ModulePtr resolvedModule = nzsl::Ast::Clone(*referenceModule);
		executor.Transform(*resolvedModule, context);

		ShaderReflection reflection;
		reflection.Reflect(*resolvedModule);

		const auto* passDataExternalBlock = reflection.GetExternalBlockByTag("PassData");
		if (!passDataExternalBlock)
			throw std::runtime_error("failed to find external block with a PassData tag");

		if (auto viewerDataIt = passDataExternalBlock->uniformBlocks.find("ViewerData"); viewerDataIt != passDataExternalBlock->uniformBlocks.end())
		{
			const auto& externalBlock = viewerDataIt->second;
			if (externalBlock.bindingSet != 0)
				throw std::runtime_error("ViewerData uniform buffer should be in the binding set #0");

			m_viewerDataBindingIndex = externalBlock.bindingIndex;
		}
		else
			throw std::runtime_error("missing ViewerData tagged uniform buffer in PassData external block");

		for (std::size_t i = 0; m_gbufferBindingIndices.max_size(); ++i)
		{
			auto it = passDataExternalBlock->samplers.find(Format("GBuffer{}", i));
			if (it == passDataExternalBlock->samplers.end())
				break;

			if (it->second.bindingSet != 0)
				throw std::runtime_error(Format("GBuffer{} sampler should be in the binding set #0", i));

			m_gbufferBindingIndices.push_back(it->second.bindingIndex);
		}

		if (auto it = passDataExternalBlock->samplers.find("DepthBuffer"); it != passDataExternalBlock->samplers.end())
		{
			if (it->second.bindingSet != 0)
				throw std::runtime_error("DepthBuffer sampler should be in the binding set #0");

			m_depthMapBindingIndex = it->second.bindingIndex;
		}
		else
			m_depthMapBindingIndex = MaxValue();

		const auto* lightDataExternalBlock = reflection.GetExternalBlockByTag("LightData");
		if (!lightDataExternalBlock)
			throw std::runtime_error("failed to find external block with a LightData tag");

		if (auto lightDataIt = lightDataExternalBlock->uniformBlocks.find("LightData"); lightDataIt != lightDataExternalBlock->uniformBlocks.end())
		{
			const auto& uniformBlock = lightDataIt->second;
			if (uniformBlock.bindingSet != 1)
				throw std::runtime_error("LightData uniform buffer should be in the binding set #1");

			m_lightDataBindingIndex = uniformBlock.bindingIndex;
		}
		else
			throw std::runtime_error("missing LightData tagged uniform buffer in LightData external block");

		if (auto shadowMapIt = lightDataExternalBlock->samplers.find("ShadowMap"); shadowMapIt != lightDataExternalBlock->samplers.end())
		{
			const auto& sampler = shadowMapIt->second;
			if (sampler.bindingSet != 1)
				throw std::runtime_error("ShadowMap sampler should be in the binding set #1");

			m_lightShadowmapBindingIndex = sampler.bindingIndex;
		}
		else
			m_lightShadowmapBindingIndex = MaxValue();

		RenderPipelineLayoutInfo pipelineLayoutInfo = std::move(reflection).GetPipelineLayoutInfo();
		{
			auto it = std::find_if(pipelineLayoutInfo.bindings.begin(), pipelineLayoutInfo.bindings.end(), [&](const auto& binding) { return binding.setIndex == 1 && binding.bindingIndex == m_lightDataBindingIndex; });
			NazaraAssert(it != pipelineLayoutInfo.bindings.end());
			it->type = ShaderBindingType::UniformBufferDynamic;
		}

		if (m_lightShadowmapBindingIndex != MaxValue<UInt32>())
		{
			m_shadowPipelineLayout = renderDevice.InstantiateRenderPipelineLayout(pipelineLayoutInfo);

			auto it = std::find_if(pipelineLayoutInfo.bindings.begin(), pipelineLayoutInfo.bindings.end(), [&](const auto& binding) { return binding.setIndex == 1 && binding.bindingIndex == m_lightShadowmapBindingIndex; });
			NazaraAssert(it != pipelineLayoutInfo.bindings.end());
			pipelineLayoutInfo.bindings.erase(it);
		}

		m_commonPipelineLayout = renderDevice.InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));
	}

	void LightingPipelinePass::SetupPipelines(RenderDevice& renderDevice, std::string&& shaderName)
	{
		using namespace nzsl::Ast::Literals;

		m_fullscreenVertexShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Vertex, "Engine.FullscreenVertex");
		m_lightingShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment, shaderName);
		m_meshStencilShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Vertex, std::move(shaderName));

		UberShader::Config config;
		config.optionValues["EnableShadowMapping"_opt] = false;

		// Stencil pipeline
		for (BasicLightType lightType : { BasicLightType::Point, BasicLightType::Spot })
		{
			config.optionValues["Light"_opt] = static_cast<Int32>(lightType);

			RenderPipelineInfo pipelineInfo;
			pipelineInfo.primitiveMode = PrimitiveMode::TriangleList;
			pipelineInfo.pipelineLayout = m_commonPipelineLayout;
			pipelineInfo.vertexBuffers.push_back({
				0,
				VertexDeclaration::Get(VertexLayout::XYZ)
			});

			pipelineInfo.shaderModules.push_back(m_meshStencilShader->Get(config));

			pipelineInfo.colorWriteMask = ColorComponentMask{};
			pipelineInfo.depthBuffer = true;
			pipelineInfo.depthWrite = false;
			pipelineInfo.faceCulling = FaceCulling::Front;
			pipelineInfo.stencilTest = true;
			pipelineInfo.stencilFront.compare = RendererComparison::Always;
			pipelineInfo.stencilFront.depthFail = StencilOperation::Invert;
			pipelineInfo.stencilBack.compare = RendererComparison::Always;
			pipelineInfo.stencilBack.depthFail = StencilOperation::Invert;

			m_pipelines[lightType].stencilPipeline = renderDevice.InstantiateRenderPipeline(pipelineInfo);

			// Shadow pipeline
			pipelineInfo.pipelineLayout = m_shadowPipelineLayout;

			config.optionValues["EnableShadowMapping"_opt] = true;

			pipelineInfo.shaderModules.clear();
			pipelineInfo.shaderModules.push_back(m_meshStencilShader->Get(config));

			m_pipelines[lightType].stencilPipelineShadow = renderDevice.InstantiateRenderPipeline(pipelineInfo);
		}

		// Lighting pipeline
		for (BasicLightType lightType : { BasicLightType::Directional, BasicLightType::Point, BasicLightType::Spot })
		{
			RenderPipelineInfo pipelineInfo;
			pipelineInfo.pipelineLayout = m_commonPipelineLayout;
			pipelineInfo.primitiveMode = PrimitiveMode::TriangleList;

			pipelineInfo.blending = true;
			pipelineInfo.blend.dstColor = BlendFunc::One;
			pipelineInfo.blend.srcColor = BlendFunc::One;
			pipelineInfo.depthBuffer = false;
			pipelineInfo.stencilTest = true;
			pipelineInfo.stencilBack.compare = RendererComparison::NotEqual;
			pipelineInfo.stencilBack.fail = StencilOperation::Zero;
			pipelineInfo.stencilBack.depthFail = StencilOperation::Zero;
			pipelineInfo.stencilBack.pass = StencilOperation::Zero;

			if (lightType != BasicLightType::Directional)
			{
				pipelineInfo.faceCulling = FaceCulling::Front;
				pipelineInfo.vertexBuffers.push_back({
					0,
					VertexDeclaration::Get(VertexLayout::XYZ)
				});
			}

			config.optionValues["Light"_opt] = static_cast<Int32>(lightType);
			config.optionValues["EnableShadowMapping"_opt] = false;

			pipelineInfo.shaderModules.push_back((lightType == BasicLightType::Directional) ? m_fullscreenVertexShader->Get({}) : m_meshStencilShader->Get(config));
			pipelineInfo.shaderModules.push_back(m_lightingShader->Get(config));

			m_pipelines[lightType].lightingPipeline = renderDevice.InstantiateRenderPipeline(pipelineInfo);

			// Shadow pipeline
			pipelineInfo.pipelineLayout = m_shadowPipelineLayout;

			config.optionValues["EnableShadowMapping"_opt] = true;

			pipelineInfo.shaderModules.clear();
			pipelineInfo.shaderModules.push_back((lightType == BasicLightType::Directional) ? m_fullscreenVertexShader->Get({}) : m_meshStencilShader->Get(config));
			pipelineInfo.shaderModules.push_back(m_lightingShader->Get(config));

			m_pipelines[lightType].lightingPipelineShadow = renderDevice.InstantiateRenderPipeline(pipelineInfo);
		}
	}

	void* LightingPipelinePass::PushLightData(RenderDevice& renderDevice, UInt64 maxLight, std::vector<LightBlockMemory>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlock>& lights, UInt64 lightSize)
	{
		if (lights.empty() || lights.back().lightCount >= maxLight)
		{
			LightBlock& lightBlock = lights.emplace_back();

			if (!lightMemoryPool.empty())
			{
				lightBlock.memory = std::move(lightMemoryPool.back());
				lightMemoryPool.pop_back();
			}
			else
			{
				UInt64 maxUniformBufferSize = renderDevice.GetDeviceInfo().limits.maxUniformBufferSize;
				UInt64 maxLightCount = std::min(maxLight, maxUniformBufferSize / lightSize);

				// Allocate new light block
				lightBlock.memory.lightUbo = renderDevice.InstantiateBuffer(BufferType::Uniform, maxLightCount * lightSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write, nullptr);

				lightBlock.memory.shaderBinding = m_commonPipelineLayout->AllocateShaderBinding(1);
				lightBlock.memory.shaderBinding->Update({
					{
						m_lightDataBindingIndex,
						ShaderBinding::UniformBufferBinding {
							.buffer = lightBlock.memory.lightUbo.get(),
							.offset = 0, .range = lightSize, .dynamic = true
						}
					}
				});
			}

			lightBlock.uploadAllocation = &renderResources.GetUploadPool().Allocate(lightBlock.memory.lightUbo->GetSize());
		}

		LightBlock& lightBlock = lights.back();
		return AccessByOffset<void*>(lightBlock.uploadAllocation->mappedPtr, lightBlock.lightCount++ * lightSize);
	}

	void* LightingPipelinePass::PushLightData(RenderDevice& renderDevice, UInt64 maxLight, std::vector<LightBlockMemoryShadow>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlockShadow>& lights, UInt64 lightSize, const Texture* shadowMap, const TextureSampler* shadowMapSampler)
	{
		if (lights.empty() || lights.back().lightCount >= maxLight)
		{
			LightBlockShadow& lightBlock = lights.emplace_back();

			if (!lightMemoryPool.empty())
			{
				lightBlock.memory = std::move(lightMemoryPool.back());
				lightMemoryPool.pop_back();
			}
			else
			{
				UInt64 maxUniformBufferSize = renderDevice.GetDeviceInfo().limits.maxUniformBufferSize;
				UInt64 maxLightCount = std::min(maxLight, maxUniformBufferSize / lightSize);

				// Allocate new light block
				lightBlock.memory.lightUbo = renderDevice.InstantiateBuffer(BufferType::Uniform, maxLightCount * lightSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write, nullptr);
				lightBlock.memory.shaderBindings.clear();
			}

			lightBlock.uploadAllocation = &renderResources.GetUploadPool().Allocate(lightBlock.memory.lightUbo->GetSize());
		}

		LightBlockShadow& lightBlock = lights.back();
		lightBlock.memory.shaderBindings.push_back(m_shadowPipelineLayout->AllocateShaderBinding(1));
		lightBlock.memory.shaderBindings.back()->Update({
			{
				m_lightDataBindingIndex,
				ShaderBinding::UniformBufferBinding {
					.buffer = lightBlock.memory.lightUbo.get(),
					.offset = 0, .range = lightSize, .dynamic = true
				}
			},
			{
				m_lightShadowmapBindingIndex,
				ShaderBinding::SampledTextureBinding {
					.texture = shadowMap, .sampler = shadowMapSampler
				}
			}
		});

		return AccessByOffset<void*>(lightBlock.uploadAllocation->mappedPtr, lightBlock.lightCount++ * lightSize);
	}

	void LightingPipelinePass::ReleaseLights(std::vector<LightBlockMemory>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlock>& lights)
	{
		for (auto& lightBlock : lights)
		{
			renderResources.PushReleaseCallback([poolPtr = m_lightBufferPool, &lightMemoryPool, block = std::move(lightBlock.memory)]() mutable
			{
				lightMemoryPool.push_back(std::move(block));
			});
		}

		lights.clear();
	}

	void LightingPipelinePass::ReleaseLights(std::vector<LightBlockMemoryShadow>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlockShadow>& lights)
	{
		for (auto& lightBlock : lights)
		{
			renderResources.PushReleaseCallback([poolPtr = m_lightBufferPool, &lightMemoryPool, block = std::move(lightBlock.memory)]() mutable
			{
				lightMemoryPool.push_back(std::move(block));
			});
		}

		lights.clear();
	}
}

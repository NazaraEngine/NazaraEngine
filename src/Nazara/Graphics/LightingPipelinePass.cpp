// Copyright (C) 2024 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/LightingPipelinePass.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShaderTransfer.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <NZSL/Parser.hpp>

namespace Nz
{
	LightingPipelinePass::LightingPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	FramePipelinePass(0),
	m_lastVisibilityHash(0),
	m_viewer(passData.viewer),
	m_pipeline(passData.pipeline),
	m_rebuildCommandBuffer(false),
	m_rebuildLights(false)
	{
		Graphics* graphics = Graphics::Instance();
		const auto& renderDevice = graphics->GetRenderDevice();

		SetupMeshes();
		SetupPipelineLayouts(*renderDevice);
		SetupPipelines(*renderDevice);

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

			UInt64 uboAlignment = renderDevice->GetDeviceInfo().limits.minUniformBufferOffsetAlignment;

			for (std::size_t lightIndex : frameData.visibleLights->IterBits())
			{
				const Light* light = m_pipeline.RetrieveLight(lightIndex);

				constexpr UInt8 DirectionalLightType = static_cast<UInt8>(BasicLightType::Directional);
				constexpr UInt8 PointLightType = static_cast<UInt8>(BasicLightType::Point);
				constexpr UInt8 SpotLightType = static_cast<UInt8>(BasicLightType::Spot);

				switch (light->GetLightType())
				{
					case DirectionalLightType:
					{
						UInt64 lightSize = AlignPow2(PredefinedDirectionalLightOffsets.totalSize, uboAlignment);
						void* lightData = PushLightData(*renderDevice, 256, m_lightBufferPool->directionalLightPool, frameData.renderResources, m_directionalLights, lightSize);
						ShaderTransfer::WriteLight(SafeCast<const DirectionalLight*>(light), lightData);
						break;
					}

					case PointLightType:
					{
						UInt64 lightSize = AlignPow2(PredefinedPointLightOffsets.totalSize, uboAlignment);
						void* lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->pointLightPool, frameData.renderResources, m_pointLights, lightSize);
						ShaderTransfer::WriteLight(SafeCast<const PointLight*>(light), lightData);

						break;
					}

					case SpotLightType:
					{
						UInt64 lightSize = AlignPow2(PredefinedSpotLightOffsets.totalSize, uboAlignment);
						void* lightData = PushLightData(*renderDevice, 1024, m_lightBufferPool->spotLightPool, frameData.renderResources, m_spotLights, lightSize);
						ShaderTransfer::WriteLight(SafeCast<const SpotLight*>(light), lightData);
						break;
					}
				}
			}

			frameData.renderResources.Execute([&](CommandBufferBuilder& builder)
			{
				UInt64 directionalLightSize = AlignPow2(PredefinedDirectionalLightOffsets.totalSize, uboAlignment);
				UInt64 pointLightSize = AlignPow2(PredefinedPointLightOffsets.totalSize, uboAlignment);
				UInt64 spotLightSize = AlignPow2(PredefinedSpotLightOffsets.totalSize, uboAlignment);

				for (auto& lightBlock : m_directionalLights)
					builder.CopyBuffer(*lightBlock.uploadAllocation, RenderBufferView(lightBlock.memory.lightUbo.get(), 0, lightBlock.lightCount * directionalLightSize));

				for (auto& lightBlock : m_pointLights)
					builder.CopyBuffer(*lightBlock.uploadAllocation, RenderBufferView(lightBlock.memory.lightUbo.get(), 0, lightBlock.lightCount * pointLightSize));

				for (auto& lightBlock : m_spotLights)
					builder.CopyBuffer(*lightBlock.uploadAllocation, RenderBufferView(lightBlock.memory.lightUbo.get(), 0, lightBlock.lightCount * spotLightSize));

				builder.MemoryBarrier(PipelineStage::Transfer, PipelineStage::VertexInput, MemoryAccess::TransferWrite, MemoryAccess::VertexBufferRead);
			}, QueueType::Transfer);

			m_rebuildCommandBuffer = true;
			m_rebuildLights = false;

			m_lastVisibilityHash = lightVisibilityHash;
		}
	}

	FramePass& LightingPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() != 3)
			throw std::runtime_error("three inputs expected");

		Graphics* graphics = Graphics::Instance();

		const auto& sampler = graphics->GetSamplerCache().Get({});

		FramePass& lightingPass = frameGraph.AddPass("Lighting pass");

		std::size_t albedoAttachment = inputOuputs.inputAttachments[0].attachmentIndex;
		std::size_t normalAttachment = inputOuputs.inputAttachments[1].attachmentIndex;
		std::size_t depthAttachment = inputOuputs.inputAttachments[2].attachmentIndex;

		lightingPass.AddInput(albedoAttachment);
		lightingPass.AddInput(normalAttachment);
		std::size_t depthInput = lightingPass.AddInput(depthAttachment);

		lightingPass.SetInputAccess(depthInput, TextureLayout::DepthReadOnlyStencilReadWrite, PipelineStage::FragmentShader, MemoryAccess::ShaderRead);

		for (const PassOutputData& outputData : inputOuputs.outputAttachments)
			lightingPass.AddOutput(outputData.attachmentIndex);

		lightingPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		lightingPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		lightingPass.SetClearColor(0, Color::Black());

		lightingPass.SetExecutionCallback([&]()
		{
			return (m_rebuildCommandBuffer) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		lightingPass.SetCommandCallback([this, &sampler, albedoAttachment, normalAttachment, depthAttachment](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			env.renderResources.PushForRelease(std::move(m_commonShaderBinding));

			const auto& viewerBuffer = m_viewer->GetViewerInstance().GetViewerBuffer();
			const auto& albedoMap = env.frameGraph.GetAttachmentTexture(albedoAttachment);
			const auto& normalMap = env.frameGraph.GetAttachmentTexture(normalAttachment);
			const auto& depthMap = env.frameGraph.GetAttachmentTexture(depthAttachment);

			m_commonShaderBinding = m_commonPipelineLayout->AllocateShaderBinding(0);
			m_commonShaderBinding->Update({
				{
					0,
					ShaderBinding::UniformBufferBinding {
						viewerBuffer.get(),
						0, viewerBuffer->GetSize()
					}
				},
				{
					1,
					ShaderBinding::SampledTextureBinding {
						albedoMap.get(), sampler.get(),
					}
				},
				{
					2,
					ShaderBinding::SampledTextureBinding {
						normalMap.get(), sampler.get(),
					}
				},
				{
					3,
					ShaderBinding::SampledTextureBinding {
						depthMap.get(), sampler.get(), TextureLayout::DepthReadOnlyStencilReadWrite
					}
				}
			});

			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			builder.BindRenderShaderBinding(0, *m_commonShaderBinding);

			if (!m_directionalLights.empty())
			{
				builder.BindRenderPipeline(*m_lightingPipelines[BasicLightType::Directional]);

				for (const auto& directionalLight : m_directionalLights)
				{
					for (std::size_t i = 0; i < directionalLight.lightCount; ++i)
					{
						builder.BindRenderShaderBinding(1, *directionalLight.memory.shaderBindings[i]);
						builder.Draw(3);
					}
				}
			}

			if (!m_pointLights.empty())
			{
				builder.BindIndexBuffer(*m_pointLightMesh->GetIndexBuffer(0), IndexType::U16);
				builder.BindVertexBuffer(0, *m_pointLightMesh->GetVertexBuffer(0));
				UInt32 indexCount = m_pointLightMesh->GetIndexCount(0);

				for (const auto& pointLight : m_pointLights)
				{
					for (std::size_t i = 0; i < pointLight.lightCount; ++i)
					{
						builder.BindRenderShaderBinding(1, *pointLight.memory.shaderBindings[i]);

						builder.BindRenderPipeline(*m_stencilPipelines[BasicLightType::Point]);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_lightingPipelines[BasicLightType::Point]);
						builder.DrawIndexed(indexCount);
					}
				}
			}

			if (!m_spotLights.empty())
			{
				builder.BindIndexBuffer(*m_spotLightMesh->GetIndexBuffer(0), IndexType::U16);
				builder.BindVertexBuffer(0, *m_spotLightMesh->GetVertexBuffer(0));
				UInt32 indexCount = m_spotLightMesh->GetIndexCount(0);

				for (const auto& spotLight : m_spotLights)
				{
					for (std::size_t i = 0; i < spotLight.lightCount; ++i)
					{
						builder.BindRenderShaderBinding(1, *spotLight.memory.shaderBindings[i]);

						builder.BindRenderPipeline(*m_stencilPipelines[BasicLightType::Spot]);
						builder.DrawIndexed(indexCount);
						builder.BindRenderPipeline(*m_lightingPipelines[BasicLightType::Spot]);
						builder.DrawIndexed(indexCount);
					}
				}
			}

			m_rebuildCommandBuffer = false;
		});

		return lightingPass;
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

	void LightingPipelinePass::SetupPipelineLayouts(RenderDevice& renderDevice)
	{
		RenderPipelineLayoutInfo pipelineLayoutInfo;
		pipelineLayoutInfo.bindings.assign({
			{
				0, 0, 1,
				ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex
			},
			{
				0, 1, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			},
			{
				0, 2, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			},
			{
				0, 3, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			},
			{
				1, 0, 1,
				ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex
			}
		});

		m_commonPipelineLayout = renderDevice.InstantiateRenderPipelineLayout(pipelineLayoutInfo);
	}

	void LightingPipelinePass::SetupPipelines(RenderDevice& renderDevice)
	{
		m_fullscreenVertexShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Vertex, "Engine.FullscreenVertex");
		m_lightingShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment, "DeferredShading.PhongLighting");
		m_meshStencilShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Vertex, "DeferredShading.PhongLighting");

		// Stencil pipeline
		for (BasicLightType lightType : { BasicLightType::Point, BasicLightType::Spot })
		{
			UberShader::Config config;
			config.optionValues[nzsl::Ast::HashOption("LightType")] = static_cast<Int32>(lightType);

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

			m_stencilPipelines[lightType] = renderDevice.InstantiateRenderPipeline(pipelineInfo);
		}

		// Lighting pipeline
		for (BasicLightType lightType : { BasicLightType::Directional, BasicLightType::Point, BasicLightType::Spot })
		{
			UberShader::Config config;
			config.optionValues[nzsl::Ast::HashOption("LightType")] = static_cast<Int32>(lightType);
			
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

			pipelineInfo.shaderModules.push_back((lightType == BasicLightType::Directional) ? m_fullscreenVertexShader->Get({}) : m_meshStencilShader->Get(config));
			pipelineInfo.shaderModules.push_back(m_lightingShader->Get(config));

			m_lightingPipelines[lightType] = renderDevice.InstantiateRenderPipeline(pipelineInfo);
		}
	}

	void* LightingPipelinePass::PushLightData(RenderDevice& renderDevice, UInt64 maxLight, std::vector<LightBlockMemory>& lightMemoryPool, RenderResources& renderResources, std::vector<LightBlock>& lights, UInt64 lightSize)
	{
		if (!lights.empty())
		{
			LightBlock& lightBlock = lights.back();
			if (lightBlock.lightCount < lightBlock.memory.maxLightCount)
				return AccessByOffset<void*>(lightBlock.uploadAllocation->mappedPtr, lightBlock.lightCount++);
		}
		LightBlock& lightBlock = lights.emplace_back();
		lightBlock.lightCount = 1;

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

			// TODO: Add a way to allocate multiple shader bindings at once
			lightBlock.memory.shaderBindings.resize(maxLightCount);
			for (std::size_t i = 0; i < maxLightCount; ++i)
			{
				lightBlock.memory.shaderBindings[i] = m_commonPipelineLayout->AllocateShaderBinding(1);
				lightBlock.memory.shaderBindings[i]->Update({
					{
						0,
						ShaderBinding::UniformBufferBinding {
							lightBlock.memory.lightUbo.get(),
							i * lightSize, lightSize
						}
					}
				});
			}
		}

		lightBlock.uploadAllocation = &renderResources.GetUploadPool().Allocate(lightBlock.memory.lightUbo->GetSize());
		return lightBlock.uploadAllocation->mappedPtr;
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
}

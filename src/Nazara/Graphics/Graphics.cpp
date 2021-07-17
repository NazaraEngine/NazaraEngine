// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_blitShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/blit.nzsl.h>
		};
	}

	/*!
	* \ingroup graphics
	* \class Nz::Graphics
	* \brief Graphics class that represents the module initializer of Graphics
	*/
	Graphics::Graphics(Config config) :
	ModuleBase("Graphics", this),
	m_preferredDepthStencilFormat(PixelFormat::Undefined)
	{
		ECS::RegisterComponents();

		Renderer* renderer = Renderer::Instance();

		const std::vector<RenderDeviceInfo>& renderDeviceInfo = renderer->QueryRenderDevices();
		if (renderDeviceInfo.empty())
			throw std::runtime_error("no render device available");

		std::size_t bestRenderDeviceIndex = 0;
		for (std::size_t i = 0; i < renderDeviceInfo.size(); ++i)
		{
			const auto& deviceInfo = renderDeviceInfo[i];
			if (config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Dedicated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
			else if (!config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Integrated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
		}

		RenderDeviceFeatures enabledFeatures;
		enabledFeatures.anisotropicFiltering = !config.forceDisableFeatures.anisotropicFiltering && renderDeviceInfo[bestRenderDeviceIndex].features.anisotropicFiltering;
		enabledFeatures.depthClamping = !config.forceDisableFeatures.depthClamping && renderDeviceInfo[bestRenderDeviceIndex].features.depthClamping;
		enabledFeatures.nonSolidFaceFilling = !config.forceDisableFeatures.nonSolidFaceFilling && renderDeviceInfo[bestRenderDeviceIndex].features.nonSolidFaceFilling;

		m_renderDevice = renderer->InstanciateRenderDevice(bestRenderDeviceIndex, enabledFeatures);
		if (!m_renderDevice)
			throw std::runtime_error("failed to instantiate render device");

		m_renderPassCache.emplace(*m_renderDevice);
		m_samplerCache.emplace(m_renderDevice);

		MaterialPipeline::Initialize();

		RenderPipelineLayoutInfo referenceLayoutInfo;
		FillViewerPipelineLayout(referenceLayoutInfo);
		FillWorldPipelineLayout(referenceLayoutInfo);

		m_referencePipelineLayout = m_renderDevice->InstantiateRenderPipelineLayout(std::move(referenceLayoutInfo));

		BuildFullscreenVertexBuffer();
		BuildBlitPipeline();
		SelectDepthStencilFormats();
	}

	Graphics::~Graphics()
	{
		MaterialPipeline::Uninitialize();
		m_renderPassCache.reset();
		m_samplerCache.reset();
		m_fullscreenVertexBuffer.reset();
		m_fullscreenVertexDeclaration.reset();
		m_blitPipeline.reset();
		m_blitPipelineLayout.reset();
	}

	void Graphics::FillViewerPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::UniformBuffer,
			ShaderStageType_All
		});
	}

	void Graphics::FillWorldPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::UniformBuffer,
			ShaderStageType_All
		});
	}

	void Graphics::BuildBlitPipeline()
	{
		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0,
				ShaderBindingType::Texture,
				ShaderStageType::Fragment
			}
		});

		m_blitPipelineLayout = m_renderDevice->InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_blitPipelineLayout)
			throw std::runtime_error("failed to instantiate fullscreen renderpipeline layout");

		auto blitShader = m_renderDevice->InstantiateShaderModule(ShaderStageType::Fragment | ShaderStageType::Vertex, ShaderLanguage::NazaraShader, r_blitShader, sizeof(r_blitShader), {});
		if (!blitShader)
			throw std::runtime_error("failed to instantiate blit shader");

		RenderPipelineInfo pipelineInfo;
		pipelineInfo.pipelineLayout = m_blitPipelineLayout;
		pipelineInfo.shaderModules.push_back(std::move(blitShader));
		pipelineInfo.vertexBuffers.assign({
			{
				0,
				m_fullscreenVertexDeclaration
			}
		});

		m_blitPipeline = m_renderDevice->InstantiateRenderPipeline(std::move(pipelineInfo));
	}

	void Graphics::BuildFullscreenVertexBuffer()
	{
		m_fullscreenVertexDeclaration = VertexDeclaration::Get(VertexLayout::XY_UV);
		std::array<Nz::VertexStruct_XY_UV, 3> vertexData = {
			{
				{
					Nz::Vector2f(-1.f, 1.f),
					Nz::Vector2f(0.0f, 1.0f),
				},
				{
					Nz::Vector2f(-1.f, -3.f),
					Nz::Vector2f(0.0f, -1.0f),
				},
				{
					Nz::Vector2f(3.f, 1.f),
					Nz::Vector2f(2.0f, 1.0f),
				}
			}
		};

		m_fullscreenVertexBuffer = m_renderDevice->InstantiateBuffer(BufferType::Vertex);
		if (!m_fullscreenVertexBuffer->Initialize(m_fullscreenVertexDeclaration->GetStride() * vertexData.size(), BufferUsage::DeviceLocal))
			throw std::runtime_error("failed to initialize fullscreen vertex buffer");

		if (!m_fullscreenVertexBuffer->Fill(vertexData.data(), 0, m_fullscreenVertexDeclaration->GetStride() * vertexData.size()))
			throw std::runtime_error("failed to fill fullscreen vertex buffer");
	}

	void Graphics::SelectDepthStencilFormats()
	{
		for (PixelFormat depthStencilCandidate : { PixelFormat::Depth24Stencil8, PixelFormat::Depth32FStencil8, PixelFormat::Depth16Stencil8 })
		{
			if (m_renderDevice->IsTextureFormatSupported(depthStencilCandidate, TextureUsage::DepthStencilAttachment))
			{
				m_preferredDepthStencilFormat = depthStencilCandidate;
				break;
			}
		}

		if (m_preferredDepthStencilFormat == PixelFormat::Undefined)
			throw std::runtime_error("no supported depth-stencil format found");
	}

	Graphics* Graphics::s_instance = nullptr;
}

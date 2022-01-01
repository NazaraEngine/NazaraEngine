// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Utility/Font.hpp>
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

		BuildDefaultTextures();
		BuildFullscreenVertexBuffer();
		BuildBlitPipeline();
		RegisterMaterialPasses();
		SelectDepthStencilFormats();

		Font::SetDefaultAtlas(std::make_shared<GuillotineTextureAtlas>(*m_renderDevice));
	}

	Graphics::~Graphics()
	{
		// Free of atlas if it is ours
		std::shared_ptr<AbstractAtlas> defaultAtlas = Font::GetDefaultAtlas();
		if (defaultAtlas && defaultAtlas->GetStorage() == DataStorage::Hardware)
		{
			Font::SetDefaultAtlas(nullptr);

			// The default police can make live one hardware atlas after the free of a module (which could be problematic)
			// So, if the default police use a hardware atlas, we stole it.
			// I don't like this solution, but I don't have any better
			if (!defaultAtlas.unique())
			{
				// Still at least one police use the atlas
				const std::shared_ptr<Font>& defaultFont = Font::GetDefault();
				defaultFont->SetAtlas(nullptr);

				if (!defaultAtlas.unique())
				{
					// Still not the only one to own it ? Then crap.
					NazaraWarning("Default font atlas uses hardware storage and is still used");
				}
			}
		}

		defaultAtlas.reset();

		MaterialPipeline::Uninitialize();
		m_renderPassCache.reset();
		m_samplerCache.reset();
		m_fullscreenVertexBuffer.reset();
		m_fullscreenVertexDeclaration.reset();
		m_blitPipeline.reset();
		m_blitPipelineLayout.reset();
		m_defaultTextures.whiteTextures.fill(nullptr);
	}

	void Graphics::FillDrawDataPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		// TextureOverlay
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::Texture,
			ShaderStageType_All
		});
	}

	void Graphics::FillViewerPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		// ViewerData
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::UniformBuffer,
			ShaderStageType_All
		});
	}

	void Graphics::FillWorldPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		// InstanceData
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

		m_blitPipeline = m_renderDevice->InstantiateRenderPipeline(pipelineInfo);

		// Blending
		pipelineInfo.blending = true;
		pipelineInfo.blend.modeColor = BlendEquation::Add;
		pipelineInfo.blend.modeAlpha = BlendEquation::Add;
		pipelineInfo.blend.srcColor = BlendFunc::SrcAlpha;
		pipelineInfo.blend.dstColor = BlendFunc::InvSrcAlpha;
		pipelineInfo.blend.srcAlpha = BlendFunc::SrcAlpha;
		pipelineInfo.blend.dstAlpha = BlendFunc::InvSrcAlpha;

		m_blitPipelineTransparent = m_renderDevice->InstantiateRenderPipeline(std::move(pipelineInfo));
	}

	void Graphics::BuildDefaultTextures()
	{
		// White texture 2D
		{
			Nz::TextureInfo texInfo;
			texInfo.width = texInfo.height = texInfo.depth = texInfo.mipmapLevel = 1;
			texInfo.pixelFormat = PixelFormat::L8;

			std::array<UInt8, 6> whitePixels = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			for (std::size_t i = 0; i < ImageTypeCount; ++i)
			{
				texInfo.type = static_cast<ImageType>(i);

				m_defaultTextures.whiteTextures[i] = m_renderDevice->InstantiateTexture(texInfo);
				m_defaultTextures.whiteTextures[i]->Update(whitePixels.data());
			}
		}
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

	void Graphics::RegisterMaterialPasses()
	{
		m_materialPassRegistry.RegisterPass("ForwardPass");
		m_materialPassRegistry.RegisterPass("DepthPass");
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

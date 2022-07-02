// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Utility/Font.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <NZSL/Ast/Module.hpp>
#include <array>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_textureBlitShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/TextureBlit.nzslb.h>
		};

		const UInt8 r_basicMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/BasicMaterial.nzslb.h>
		};

		const UInt8 r_depthMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/DepthMaterial.nzslb.h>
		};

		const UInt8 r_fullscreenVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/FullscreenVertex.nzslb.h>
		};

		const UInt8 r_phongMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongMaterial.nzslb.h>
		};

		const UInt8 r_physicallyBasedMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhysicallyBasedMaterial.nzslb.h>
		};

		// Modules
		const UInt8 r_instanceDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/InstanceData.nzslb.h>
		};

		const UInt8 r_lightDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/LightData.nzslb.h>
		};

		const UInt8 r_viewerDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/ViewerData.nzslb.h>
		};

		const UInt8 r_mathConstantsModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/Constants.nzslb.h>
		};

		const UInt8 r_mathCookTorrancePBRModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/CookTorrancePBR.nzslb.h>
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

		BuildDefaultTextures();
		RegisterShaderModules();
		BuildBlitPipeline();
		RegisterMaterialPasses();
		SelectDepthStencilFormats();

		MaterialPipeline::Initialize();

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
		m_blitPipeline.reset();
		m_blitPipelineLayout.reset();
		m_defaultTextures.whiteTextures.fill(nullptr);
	}

	void Graphics::BuildBlitPipeline()
	{
		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0,
				ShaderBindingType::Texture,
				nzsl::ShaderStageType::Fragment
			}
		});

		m_blitPipelineLayout = m_renderDevice->InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_blitPipelineLayout)
			throw std::runtime_error("failed to instantiate fullscreen renderpipeline layout");

		nzsl::Ast::ModulePtr blitShaderModule = m_shaderModuleResolver->Resolve("TextureBlit");

		nzsl::ShaderWriter::States states;
		states.shaderModuleResolver = m_shaderModuleResolver;

		auto blitShader = m_renderDevice->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *blitShaderModule, states);
		if (!blitShader)
			throw std::runtime_error("failed to instantiate blit shader");

		RenderPipelineInfo pipelineInfo;

		pipelineInfo.pipelineLayout = m_blitPipelineLayout;
		pipelineInfo.shaderModules.push_back(std::move(blitShader));

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

	void Graphics::RegisterMaterialPasses()
	{
		m_materialPassRegistry.RegisterPass("ForwardPass");
		m_materialPassRegistry.RegisterPass("DepthPass");
	}

	void Graphics::RegisterShaderModules()
	{
		m_shaderModuleResolver = std::make_shared<nzsl::FilesystemModuleResolver>();
		RegisterEmbedShaderModule(r_basicMaterialShader);
		RegisterEmbedShaderModule(r_depthMaterialShader);
		RegisterEmbedShaderModule(r_fullscreenVertexShader);
		RegisterEmbedShaderModule(r_instanceDataModule);
		RegisterEmbedShaderModule(r_lightDataModule);
		RegisterEmbedShaderModule(r_mathConstantsModule);
		RegisterEmbedShaderModule(r_mathCookTorrancePBRModule);
		RegisterEmbedShaderModule(r_phongMaterialShader);
		RegisterEmbedShaderModule(r_physicallyBasedMaterialShader);
		RegisterEmbedShaderModule(r_textureBlitShader);
		RegisterEmbedShaderModule(r_viewerDataModule);

#ifdef NAZARA_DEBUG
		// Override embed files with dev files in debug
		if (std::filesystem::path modulePath = "../../src/Nazara/Graphics/Resources/Shaders"; std::filesystem::is_directory(modulePath))
			m_shaderModuleResolver->RegisterModuleDirectory(modulePath, true);
#endif

		// Let application register their own shaders
		if (std::filesystem::path shaderPath = "Shaders"; std::filesystem::is_directory(shaderPath))
			m_shaderModuleResolver->RegisterModuleDirectory(shaderPath);
	}

	template<std::size_t N>
	void Graphics::RegisterEmbedShaderModule(const UInt8(&content)[N])
	{
		nzsl::Unserializer unserializer(content, N);
		m_shaderModuleResolver->RegisterModule(nzsl::Ast::UnserializeShader(unserializer));
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

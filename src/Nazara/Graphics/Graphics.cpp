// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PredefinedMaterials.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/Formats/TextureLoader.hpp>
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

		const UInt8 r_skeletalDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkeletalData.nzslb.h>
		};

		const UInt8 r_skinningDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkinningData.nzslb.h>
		};

		const UInt8 r_skinningLinearModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkinningLinear.nzslb.h>
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
	* \class Graphics
	* \brief Graphics class that represents the module initializer of Graphics
	*/
	Graphics::Graphics(Config config) :
	ModuleBase("Graphics", this),
	m_preferredDepthFormat(PixelFormat::Undefined),
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
		BuildDefaultMaterials();

		Font::SetDefaultAtlas(std::make_shared<GuillotineTextureAtlas>(*m_renderDevice));

		m_materialInstanceLoader.RegisterLoader(Loaders::GetMaterialInstanceLoader_Texture()); // texture to material loader
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
		m_defaultMaterials = DefaultMaterials{};
		m_defaultTextures = DefaultTextures{};
	}

	void Graphics::BuildBlitPipeline()
	{
		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0, 1,
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

	void Graphics::BuildDefaultMaterials()
	{
		std::size_t depthPassIndex = m_materialPassRegistry.GetPassIndex("DepthPass");
		std::size_t shadowPassIndex = m_materialPassRegistry.GetPassIndex("ShadowPass");
		std::size_t forwardPassIndex = m_materialPassRegistry.GetPassIndex("ForwardPass");

		// BasicMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "BasicMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.states.faceCulling = FaceCulling::Front;
			settings.AddPass(shadowPassIndex, shadowPass);

			m_defaultMaterials.basicMaterial = std::make_shared<Material>(std::move(settings), "BasicMaterial");
		}

		// PbrMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);
			PredefinedMaterials::AddPbrSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhysicallyBasedMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.states.faceCulling = FaceCulling::Front;
			settings.AddPass(shadowPassIndex, shadowPass);

			m_defaultMaterials.pbrMaterial = std::make_shared<Material>(std::move(settings), "PhysicallyBasedMaterial");
		}

		// PhongMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);
			PredefinedMaterials::AddPhongSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhongMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.states.faceCulling = FaceCulling::Front;
			shadowPass.states.depthBias = true;
			shadowPass.states.depthBiasConstantFactor = 0.005f;
			shadowPass.states.depthBiasSlopeFactor = 0.05f;
			settings.AddPass(shadowPassIndex, shadowPass);

			m_defaultMaterials.phongMaterial = std::make_shared<Material>(std::move(settings), "PhongMaterial");
		}

		m_defaultMaterials.basicDefault = m_defaultMaterials.basicMaterial->GetDefaultInstance();

		m_defaultMaterials.basicNoDepth = m_defaultMaterials.basicMaterial->Instantiate();
		m_defaultMaterials.basicNoDepth->DisablePass(depthPassIndex);
		m_defaultMaterials.basicNoDepth->DisablePass(shadowPassIndex);
		m_defaultMaterials.basicNoDepth->UpdatePassStates(forwardPassIndex, [](RenderStates& states)
		{
			states.depthBuffer = false;
		});

		m_defaultMaterials.basicTransparent = m_defaultMaterials.basicMaterial->Instantiate();
		m_defaultMaterials.basicTransparent->DisablePass(depthPassIndex);
		m_defaultMaterials.basicTransparent->DisablePass(shadowPassIndex);
		m_defaultMaterials.basicTransparent->UpdatePassStates(forwardPassIndex, [](RenderStates& renderStates)
		{
			renderStates.depthWrite = false;
			renderStates.blending = true;
			renderStates.blend.modeColor = BlendEquation::Add;
			renderStates.blend.modeAlpha = BlendEquation::Add;
			renderStates.blend.srcColor = BlendFunc::SrcAlpha;
			renderStates.blend.dstColor = BlendFunc::InvSrcAlpha;
			renderStates.blend.srcAlpha = BlendFunc::One;
			renderStates.blend.dstAlpha = BlendFunc::One;
		});
	}

	void Graphics::BuildDefaultTextures()
	{
		// Depth textures (white but with a depth format)
		{
			PixelFormat depthFormat = PixelFormat::Undefined;
			for (PixelFormat depthStencilCandidate : { PixelFormat::Depth16, PixelFormat::Depth24, PixelFormat::Depth32F })
			{
				if (m_renderDevice->IsTextureFormatSupported(depthStencilCandidate, TextureUsage::ShaderSampling))
				{
					depthFormat = depthStencilCandidate;
					break;
				}
			}

			if (depthFormat == PixelFormat::Undefined)
				throw std::runtime_error("couldn't find a sampling-compatible depth pixel format");

			TextureInfo texInfo;
			texInfo.width = texInfo.height = texInfo.depth = texInfo.levelCount = 1;
			texInfo.pixelFormat = depthFormat;

			std::array<UInt8, 6> whitePixels = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			for (std::size_t i = 0; i < ImageTypeCount; ++i)
			{
				texInfo.type = static_cast<ImageType>(i);
				if (texInfo.type == ImageType::E3D)
					continue;

				texInfo.layerCount = (texInfo.type == ImageType::Cubemap) ? 6 : 1;

				m_defaultTextures.depthTextures[i] = m_renderDevice->InstantiateTexture(texInfo);
				m_defaultTextures.depthTextures[i]->Update(whitePixels.data());
			}
		}

		// White texture 2D
		{
			TextureInfo texInfo;
			texInfo.width = texInfo.height = texInfo.depth = texInfo.levelCount = 1;
			texInfo.pixelFormat = PixelFormat::L8;

			std::array<UInt8, 6> whitePixels = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			for (std::size_t i = 0; i < ImageTypeCount; ++i)
			{
				texInfo.type = static_cast<ImageType>(i);
				texInfo.layerCount = (texInfo.type == ImageType::Cubemap) ? 6 : 1;

				m_defaultTextures.whiteTextures[i] = m_renderDevice->InstantiateTexture(texInfo);
				m_defaultTextures.whiteTextures[i]->Update(whitePixels.data());
			}
		}
	}

	void Graphics::RegisterMaterialPasses()
	{
		m_materialPassRegistry.RegisterPass("ForwardPass");
		m_materialPassRegistry.RegisterPass("DepthPass");
		m_materialPassRegistry.RegisterPass("ShadowPass");
	}

	void Graphics::RegisterShaderModules()
	{
		m_shaderModuleResolver = std::make_shared<nzsl::FilesystemModuleResolver>();
		RegisterEmbedShaderModule(r_basicMaterialShader);
		RegisterEmbedShaderModule(r_fullscreenVertexShader);
		RegisterEmbedShaderModule(r_instanceDataModule);
		RegisterEmbedShaderModule(r_lightDataModule);
		RegisterEmbedShaderModule(r_mathConstantsModule);
		RegisterEmbedShaderModule(r_mathCookTorrancePBRModule);
		RegisterEmbedShaderModule(r_phongMaterialShader);
		RegisterEmbedShaderModule(r_physicallyBasedMaterialShader);
		RegisterEmbedShaderModule(r_skinningDataModule);
		RegisterEmbedShaderModule(r_skinningLinearModule);
		RegisterEmbedShaderModule(r_skeletalDataModule);
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
		for (PixelFormat depthStencilCandidate : { PixelFormat::Depth24, PixelFormat::Depth32F, PixelFormat::Depth16 })
		{
			if (m_renderDevice->IsTextureFormatSupported(depthStencilCandidate, TextureUsage::DepthStencilAttachment))
			{
				m_preferredDepthFormat = depthStencilCandidate;
				break;
			}
		}

		if (m_preferredDepthFormat == PixelFormat::Undefined)
			throw std::runtime_error("no supported depth format found");

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

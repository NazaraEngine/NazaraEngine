// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace Loaders
	{
		MaterialLoader::Entry GetMaterialLoader_Texture()
		{
			MaterialLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = [](std::string_view extension)
			{
				return Utility::Instance()->GetImageLoader().IsExtensionSupported(extension);
			};

			loaderEntry.streamLoader = [](Stream& stream, const MaterialParams& parameters) -> Result<std::shared_ptr<Material>, ResourceLoadingError>
			{
				TextureParams texParams;
				texParams.renderDevice = Graphics::Instance()->GetRenderDevice();

				std::shared_ptr<Texture> texture = Texture::LoadFromStream(stream, texParams);
				if (!texture)
					return Err(ResourceLoadingError::Unrecognized);

				//std::shared_ptr<Material> material = std::make_shared<Material>();

				bool hasAlphaTest = parameters.custom.GetBooleanParameter("EnableAlphaTest").GetValueOr(false);

				// ForwardPass
				/* {
					std::shared_ptr<MaterialPass> matPass;
					if (parameters.lightingType == MaterialLightingType::Phong)
						matPass = std::make_shared<MaterialPass>(PhongLightingMaterialPass::GetSettings());
					else if (parameters.lightingType == MaterialLightingType::PhysicallyBased)
						matPass = std::make_shared<MaterialPass>(PhysicallyBasedMaterialPass::GetSettings());
					else
						matPass = std::make_shared<MaterialPass>(BasicMaterialPass::GetSettings());

					matPass->EnableDepthBuffer(true);

					BasicMaterialPass forwardPass(*matPass);
					forwardPass.SetBaseColorMap(texture);

					if (hasAlphaTest && PixelFormatInfo::HasAlpha(texture->GetFormat()))
						forwardPass.EnableAlphaTest(true);

					material->AddPass("ForwardPass", std::move(matPass));
				}

				// DepthPass
				{
					std::shared_ptr<MaterialPass> matPass = std::make_shared<MaterialPass>(DepthMaterialPass::GetSettings());
					matPass->EnableDepthBuffer(true);

					if (hasAlphaTest && PixelFormatInfo::HasAlpha(texture->GetFormat()))
					{
						BasicMaterialPass depthPass(*matPass);
						depthPass.SetBaseColorMap(texture);
						depthPass.EnableAlphaTest(true);
					}

					material->AddPass("DepthPass", std::move(matPass));
				}*/

				//return material;
				return Err(ResourceLoadingError::Internal);
			};

			loaderEntry.parameterFilter = [](const MaterialParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipNativeTextureLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}

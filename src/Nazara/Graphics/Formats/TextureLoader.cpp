// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>

namespace Nz
{
	namespace Loaders
	{
		MaterialInstanceLoader::Entry GetMaterialInstanceLoader_Texture()
		{
			MaterialInstanceLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = [](std::string_view extension)
			{
				return Core::Instance()->GetImageLoader().IsExtensionSupported(extension);
			};

			loaderEntry.streamLoader = [](Stream& stream, const MaterialInstanceParams& parameters) -> Result<std::shared_ptr<MaterialInstance>, ResourceLoadingError>
			{
				ImageParams imageParams;
				if (parameters.custom.GetBooleanParameter("sRGB").GetValueOr(false))
					imageParams.loadFormat = PixelFormat::RGBA8_SRGB;

				std::shared_ptr<Image> image = Image::LoadFromStream(stream, imageParams);
				if (!image)
					return Err(ResourceLoadingError::Unrecognized);

				std::shared_ptr<TextureAsset> texture = TextureAsset::CreateFromImage(*image/*, texParams*/);
				if (!texture)
					return Err(ResourceLoadingError::Internal);

				bool enableAlphaTest = parameters.custom.GetBooleanParameter("EnableAlphaTest").GetValueOr(false);
				bool enableAlphaBlending = parameters.custom.GetBooleanParameter("EnableAlphaBlending").GetValueOr(false);

				bool hasAlpha;
				if (enableAlphaTest || enableAlphaBlending)
					hasAlpha = image->HasAlpha();
				else
					hasAlpha = false;

				// Delete image to free memory
				image.reset();

				std::shared_ptr<MaterialInstance> materialInstance = MaterialInstance::Instantiate(parameters.materialType);

				if (enableAlphaTest && hasAlpha)
					materialInstance->SetValueProperty("AlphaTest", true);

				if (enableAlphaBlending && hasAlpha)
				{
					materialInstance->DisablePass("DepthPass");
					materialInstance->UpdatePassStates("ForwardPass", [](RenderStates& renderStates)
					{
						renderStates.depthBuffer = true;
						renderStates.depthWrite = false;
						renderStates.blending = true;
						renderStates.blend.modeColor = BlendEquation::Add;
						renderStates.blend.modeAlpha = BlendEquation::Add;
						renderStates.blend.srcColor = BlendFunc::SrcAlpha;
						renderStates.blend.dstColor = BlendFunc::InvSrcAlpha;
						renderStates.blend.srcAlpha = BlendFunc::One;
						renderStates.blend.dstAlpha = BlendFunc::One;

						return true;
					});
				}

				materialInstance->SetTextureProperty("BaseColorMap", std::move(texture));

				return materialInstance;
			};

			loaderEntry.parameterFilter = [](const MaterialInstanceParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipNativeTextureLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}

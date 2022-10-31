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
		MaterialInstanceLoader::Entry GetMaterialInstanceLoader_Texture()
		{
			MaterialInstanceLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = [](std::string_view extension)
			{
				return Utility::Instance()->GetImageLoader().IsExtensionSupported(extension);
			};

			loaderEntry.streamLoader = [](Stream& stream, const MaterialInstanceParams& parameters) -> Result<std::shared_ptr<MaterialInstance>, ResourceLoadingError>
			{
				TextureParams texParams;
				texParams.renderDevice = Graphics::Instance()->GetRenderDevice();

				std::shared_ptr<Texture> texture = Texture::LoadFromStream(stream, texParams);
				if (!texture)
					return Err(ResourceLoadingError::Unrecognized);

				bool hasAlphaTest = parameters.custom.GetBooleanParameter("EnableAlphaTest").GetValueOr(false);

				std::shared_ptr<MaterialInstance> materialInstance;
				switch (parameters.lightingType)
				{
					case MaterialLightingType::Phong:
						materialInstance = Graphics::Instance()->GetDefaultMaterials().phongMaterial->CreateInstance();
						break;

					case MaterialLightingType::PhysicallyBased:
						materialInstance = Graphics::Instance()->GetDefaultMaterials().pbrMaterial->CreateInstance();
						break;

					case MaterialLightingType::None:
						break;
				}

				if (!materialInstance)
					materialInstance = Graphics::Instance()->GetDefaultMaterials().basicMaterial->CreateInstance();

				if (hasAlphaTest && PixelFormatInfo::HasAlpha(texture->GetFormat()))
					materialInstance->SetValueProperty("AlphaTest", true);

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

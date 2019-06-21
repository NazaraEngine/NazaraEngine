// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		Ternary Check(Stream& stream, const MaterialParams& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeTextureLoader", &skip) && skip)
				return Ternary_False;

			return Ternary_Unknown;
		}

		MaterialRef Load(Stream& stream, const MaterialParams& parameters)
		{
			NazaraUnused(parameters);

			TextureRef texture = Texture::LoadFromStream(stream);
			if (!texture)
			{
				NazaraError("Failed to load diffuse map");
				return nullptr;
			}

			MaterialRef material = Material::New(PhongLightingMaterial::GetSettings());
			material->SetShader(parameters.shaderName);

			PhongLightingMaterial phongMaterial(material);
			phongMaterial.SetDiffuseMap(texture);

			return material;
		}
	}

	namespace Loaders
	{
		void RegisterTexture()
		{
			MaterialLoader::RegisterLoader(ImageLoader::IsExtensionSupported, Check, Load);
		}

		void UnregisterTexture()
		{
			MaterialLoader::UnregisterLoader(ImageLoader::IsExtensionSupported, Check, Load);
		}
	}
}

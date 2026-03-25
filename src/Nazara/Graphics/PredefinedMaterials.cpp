// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PredefinedMaterials.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PropertyHandler/OptionValuePropertyHandler.hpp>
#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/PropertyHandler/UniformValuePropertyHandler.hpp>

namespace Nz
{
	void PredefinedMaterials::AddBasicSettings(MaterialSettings& settings)
	{
		settings.AddValueProperty<Color>("BaseColor", Color::White());
		settings.AddValueProperty<bool>("AlphaTest", false);
		settings.AddValueProperty<bool>("Billboard", false);
		settings.AddValueProperty<float>("AlphaTestThreshold", 0.2f);
		settings.AddValueProperty<float>("ShadowMapNormalOffset", 0.f);
		settings.AddValueProperty<float>("ShadowPosScale", 1.f - 0.0025f);
		settings.AddTextureProperty("BaseColorMap", ImageType::E2D);
		settings.AddTextureProperty("AlphaMap", ImageType::E2D);
		settings.AddPropertyHandler<OptionValuePropertyHandler>("AlphaTest", "AlphaTest");
		settings.AddPropertyHandler<OptionValuePropertyHandler>("Billboard", "Billboard");
		settings.AddPropertyHandler<TexturePropertyHandler>("BaseColorMap", "HasBaseColorTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("AlphaMap", "HasAlphaTexture");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("BaseColor");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("AlphaTestThreshold");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("ShadowMapNormalOffset");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("ShadowPosScale");
	}

	void PredefinedMaterials::AddPbrSettings(MaterialSettings& settings)
	{
		settings.AddValueProperty<float>("MetallicFactor", 1.f);
		settings.AddValueProperty<float>("RoughnessFactor", 1.f);
		settings.AddTextureProperty("EmissiveMap", ImageType::E2D);
		settings.AddTextureProperty("HeightMap", ImageType::E2D);
		settings.AddTextureProperty("MetallicMap", ImageType::E2D);
		settings.AddTextureProperty("NormalMap", ImageType::E2D);
		settings.AddTextureProperty("RoughnessMap", ImageType::E2D);
		settings.AddTextureProperty("SpecularMap", ImageType::E2D);
		settings.AddPropertyHandler<UniformValuePropertyHandler>("MetallicFactor");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("RoughnessFactor");
		settings.AddPropertyHandler<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("HeightMap", "HasHeightTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("MetallicMap", "HasMetallicTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("NormalMap", "HasNormalTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("RoughnessMap", "HasRoughnessTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("SpecularMap", "HasSpecularTexture");
	}

	void PredefinedMaterials::AddPhongSettings(MaterialSettings& settings)
	{
		settings.AddValueProperty<Color>("AmbientColor", Color::White());
		settings.AddValueProperty<Color>("SpecularColor", Color::White());
		settings.AddValueProperty<float>("Shininess", 2.f);
#ifndef NAZARA_PLATFORM_WEB
		settings.AddValueProperty<bool>("ShadowMapping", true);
#else
		// FIXME: Shadowmapping is currently broken on web because WebGL doesn't support non-constant array indexing
		settings.AddValueProperty<bool>("ShadowMapping", false);
#endif
		settings.AddTextureProperty("EmissiveMap", ImageType::E2D);
		settings.AddTextureProperty("HeightMap", ImageType::E2D);
		settings.AddTextureProperty("NormalMap", ImageType::E2D);
		settings.AddTextureProperty("SpecularMap", ImageType::E2D);
		settings.AddPropertyHandler<OptionValuePropertyHandler>("ShadowMapping", "EnableShadowMapping");
		settings.AddPropertyHandler<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("HeightMap", "HasHeightTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("NormalMap", "HasNormalTexture");
		settings.AddPropertyHandler<TexturePropertyHandler>("SpecularMap", "HasSpecularTexture");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("AmbientColor");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("SpecularColor");
		settings.AddPropertyHandler<UniformValuePropertyHandler>("Shininess");
	}
}

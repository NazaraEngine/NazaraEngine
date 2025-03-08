// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
		settings.AddPropertyHandler(std::make_unique<OptionValuePropertyHandler>("AlphaTest", "AlphaTest"));
		settings.AddPropertyHandler(std::make_unique<OptionValuePropertyHandler>("Billboard", "Billboard"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("BaseColorMap", "HasBaseColorTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("AlphaMap", "HasAlphaTexture"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("BaseColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("AlphaTestThreshold"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("ShadowMapNormalOffset"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("ShadowPosScale"));
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
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("MetallicFactor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("RoughnessFactor"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("HeightMap", "HasHeightTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("MetallicMap", "HasMetallicTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("NormalMap", "HasNormalTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("RoughnessMap", "HasRoughnessTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("SpecularMap", "HasSpecularTexture"));
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
		settings.AddPropertyHandler(std::make_unique<OptionValuePropertyHandler>("ShadowMapping", "EnableShadowMapping"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("HeightMap", "HasHeightTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("NormalMap", "HasNormalTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("SpecularMap", "HasSpecularTexture"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("AmbientColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("SpecularColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("Shininess"));
	}
}

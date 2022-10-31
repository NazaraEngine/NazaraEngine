// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PredefinedMaterials.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PropertyHandler/OptionValuePropertyHandler.hpp>
#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/PropertyHandler/UniformValuePropertyHandler.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void PredefinedMaterials::AddBasicSettings(MaterialSettings& settings)
	{
		settings.AddValueProperty<Color>("BaseColor", Color::White);
		settings.AddValueProperty<bool>("AlphaTest", false);
		settings.AddValueProperty<float>("AlphaTestThreshold", 0.2f);
		settings.AddTextureProperty("BaseColorMap", ImageType::E2D);
		settings.AddTextureProperty("AlphaMap", ImageType::E2D);
		settings.AddPropertyHandler(std::make_unique<OptionValuePropertyHandler>("AlphaTest", "AlphaTest"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("BaseColorMap", "HasBaseColorTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("AlphaMap", "HasAlphaTexture"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("BaseColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("AlphaTestThreshold"));
	}

	void PredefinedMaterials::AddPbrSettings(MaterialSettings& settings)
	{
		settings.AddTextureProperty("EmissiveMap", ImageType::E2D);
		settings.AddTextureProperty("HeightMap", ImageType::E2D);
		settings.AddTextureProperty("MetallicMap", ImageType::E2D);
		settings.AddTextureProperty("NormalMap", ImageType::E2D);
		settings.AddTextureProperty("RoughnessMap", ImageType::E2D);
		settings.AddTextureProperty("SpecularMap", ImageType::E2D);
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("HeightMap", "HasHeightTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("MetallicMap", "HasMetallicTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("NormalMap", "HasNormalTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("RoughnessMap", "HasRoughnessTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("SpecularMap", "HasSpecularTexture"));
	}

	void PredefinedMaterials::AddPhongSettings(MaterialSettings& settings)
	{
		settings.AddValueProperty<Color>("AmbientColor", Color::Black);
		settings.AddValueProperty<Color>("SpecularColor", Color::White);
		settings.AddValueProperty<float>("Shininess", 2.f);
		settings.AddTextureProperty("EmissiveMap", ImageType::E2D);
		settings.AddTextureProperty("HeightMap", ImageType::E2D);
		settings.AddTextureProperty("NormalMap", ImageType::E2D);
		settings.AddTextureProperty("SpecularMap", ImageType::E2D);
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("EmissiveMap", "HasEmissiveTexture"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("HeightMap", "HasHeightMap"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("NormalMap", "HasNormalMap"));
		settings.AddPropertyHandler(std::make_unique<TexturePropertyHandler>("SpecularMap", "HasSpecularMap"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("AmbientColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("SpecularColor"));
		settings.AddPropertyHandler(std::make_unique<UniformValuePropertyHandler>("Shininess"));
	}
}

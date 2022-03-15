// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthMaterial.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_depthMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/depth_material.nzsl.h>
		};
	}

	std::vector<std::shared_ptr<UberShader>> DepthMaterial::BuildShaders()
	{
		ShaderAst::ModulePtr shaderModule;

#ifdef NAZARA_DEBUG
		std::filesystem::path shaderPath = "../../src/Nazara/Graphics/Resources/Shaders/depth_material.nzsl";
		if (std::filesystem::exists(shaderPath))
		{
			try
			{
				shaderModule = ShaderLang::ParseFromFile(shaderPath);
			}
			catch (const std::exception& e)
			{
				NazaraError(std::string("failed to load shader from engine folder: ") + e.what());
			}
		}
#endif

		if (!shaderModule)
			shaderModule = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(r_depthMaterialShader), sizeof(r_depthMaterialShader)));

		auto shader = std::make_shared<UberShader>(ShaderStageType::Fragment | ShaderStageType::Vertex, std::move(shaderModule));

		return { std::move(shader) };
	}

	bool DepthMaterial::Initialize()
	{
		BasicUniformOffsets offsets;
		std::tie(offsets, std::ignore) = BuildUniformOffsets();

		BasicBuildOptions options;
		options.defaultValues.resize(offsets.totalSize);
		options.shaders = BuildShaders();

		options.basicOffsets = s_basicUniformOffsets;
		options.basicOptionIndexes = &s_basicOptionIndexes;
		options.basicTextureIndexes = &s_basicTextureIndexes;

		s_depthMaterialSettings = std::make_shared<MaterialSettings>(Build(options));

		return true;
	}

	void DepthMaterial::Uninitialize()
	{
		s_depthMaterialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> DepthMaterial::s_depthMaterialSettings;
}

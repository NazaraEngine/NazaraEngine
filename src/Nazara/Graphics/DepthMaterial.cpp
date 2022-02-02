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
		const UInt8 r_shader[] = {
			#include <Nazara/Graphics/Resources/Shaders/depth_material.nzsl.h>
		};
	}

	std::vector<std::shared_ptr<UberShader>> DepthMaterial::BuildShaders()
	{
		ShaderAst::StatementPtr shaderAst = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(r_shader), sizeof(r_shader)));
		auto shader = std::make_shared<UberShader>(ShaderStageType::Fragment | ShaderStageType::Vertex, shaderAst);

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

		s_basicMaterialSettings = std::make_shared<MaterialSettings>(Build(options));

		return true;
	}

	void DepthMaterial::Uninitialize()
	{
		s_basicMaterialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> DepthMaterial::s_basicMaterialSettings;
}

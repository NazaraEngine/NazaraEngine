// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthMaterial.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	std::vector<std::shared_ptr<UberShader>> DepthMaterial::BuildShaders()
	{
		ShaderAst::ModulePtr shaderModule = Graphics::Instance()->GetShaderModuleResolver()->Resolve("DepthMaterial");
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

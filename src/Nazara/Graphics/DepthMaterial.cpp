// Copyright (C) 2017 Jérôme Leclercq
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
		UniformOffsets offsets;
		std::tie(offsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(offsets.totalSize);
		s_materialSettings = std::make_shared<MaterialSettings>(Build(offsets, std::move(defaultValues), BuildShaders()));

		return true;
	}

	void DepthMaterial::Uninitialize()
	{
		s_materialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> DepthMaterial::s_materialSettings;
}

// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERMODULERESOLVER_HPP
#define NAZARA_SHADER_SHADERMODULERESOLVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Nz
{
	namespace ShaderAst
	{
		using ModulePtr = std::shared_ptr<class Module>;
	}

	class NAZARA_SHADER_API ShaderModuleResolver
	{
		public:
			ShaderModuleResolver() = default;
			ShaderModuleResolver(const ShaderModuleResolver&) = default;
			ShaderModuleResolver(ShaderModuleResolver&&) = default;
			virtual ~ShaderModuleResolver();

			virtual ShaderAst::ModulePtr Resolve(const std::string& /*moduleName*/) = 0;

			ShaderModuleResolver& operator=(const ShaderModuleResolver&) = default;
			ShaderModuleResolver& operator=(ShaderModuleResolver&&) = default;
	};
}

#include <Nazara/Shader/ShaderModuleResolver.inl>

#endif // NAZARA_SHADER_SHADERMODULERESOLVER_HPP

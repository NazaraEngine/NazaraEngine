// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline ModulePtr SanitizeVisitor::Sanitize(const Module& module, std::string* error)
	{
		return Sanitize(module, {}, error);
	}

	inline ModulePtr Sanitize(const Module& module, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(module, error);
	}

	inline ModulePtr Sanitize(const Module& module, const SanitizeVisitor::Options& options, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(module, options, error);
	}
}

#include <Nazara/Shader/DebugOff.hpp>

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERWRITER_HPP
#define NAZARA_SHADERWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderVisitor.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderWriter : public ShaderVisitor
	{
		public:
			ShaderWriter() = default;
			ShaderWriter(const ShaderWriter&) = delete;
			ShaderWriter(ShaderWriter&&) = delete;
			~ShaderWriter() = default;

			virtual Nz::String Generate(const ShaderAst::StatementPtr& node) = 0;

			virtual void RegisterFunction(const String& name, ShaderAst::StatementPtr node, std::initializer_list<ShaderAst::NamedVariablePtr> parameters, ShaderAst::ExpressionType ret) = 0;
			virtual void RegisterVariable(ShaderAst::VariableType kind, const String& name, ShaderAst::ExpressionType type) = 0;
	};
}

#endif // NAZARA_SHADERWRITER_HPP

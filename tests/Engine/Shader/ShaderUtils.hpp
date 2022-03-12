#pragma once

#ifndef NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP
#define NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP

#include <Nazara/Shader/Ast/Module.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <string>

void ExpectGLSL(const Nz::ShaderAst::Module& shader, std::string_view expectedOutput);
void ExpectNZSL(const Nz::ShaderAst::Module& shader, std::string_view expectedOutput);
void ExpectSPIRV(const Nz::ShaderAst::Module& shader, std::string_view expectedOutput, bool outputParameter = false);

Nz::ShaderAst::ModulePtr SanitizeModule(const Nz::ShaderAst::Module& module);
Nz::ShaderAst::ModulePtr SanitizeModule(const Nz::ShaderAst::Module& module, const Nz::ShaderAst::SanitizeVisitor::Options& options);

#endif

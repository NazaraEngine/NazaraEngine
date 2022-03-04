#pragma once

#ifndef NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP
#define NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP

#include <Nazara/Shader/Ast/Module.hpp>
#include <string>

void ExpectGLSL(Nz::ShaderAst::Module& shader, std::string_view expectedOutput);
void ExpectNZSL(Nz::ShaderAst::Module& shader, std::string_view expectedOutput);
void ExpectSPIRV(Nz::ShaderAst::Module& shader, std::string_view expectedOutput, bool outputParameter = false);

#endif

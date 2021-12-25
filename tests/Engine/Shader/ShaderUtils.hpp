#pragma once

#ifndef NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP
#define NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP

#include <Nazara/Shader/Ast/Nodes.hpp>
#include <string>

void ExpectGLSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput);
void ExpectNZSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput);
void ExpectSpirV(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput, bool outputParameter = false);

#endif

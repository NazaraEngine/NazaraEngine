#pragma once

#ifndef NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP
#define NAZARA_UNITTESTS_SHADER_SHADERUTILS_HPP

#include <Nazara/Shader/Ast/Nodes.hpp>
#include <string>

void ExpectingGLSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput);
void ExpectingNZSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput);
void ExpectingSpirV(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput);

#endif

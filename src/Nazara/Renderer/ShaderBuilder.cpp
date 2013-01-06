// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <map>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	std::map<nzUInt32, NzShader*> s_shaders;

	NzString BuildFragmentShaderSource(nzUInt32 flags)
	{
		bool glsl140 = (NzOpenGL::GetVersion() >= 310);
		bool useMRT = (glsl140 && NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets));

		NzString inKW = (glsl140) ? "in" : "varying";
		NzString fragmentColorKW = (glsl140) ? "RenderTarget0" : "gl_FragColor";

		NzString sourceCode;
		sourceCode.Reserve(256); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

		/********************Version de GLSL********************/
		sourceCode = "#version ";
		if (glsl140)
			sourceCode += "140\n";
		else
			sourceCode += "110\n";

		sourceCode += '\n';

		/********************Uniformes********************/
		if ((flags & nzShaderBuilder_DiffuseMapping) == 0 || flags & nzShaderBuilder_Lighting)
			sourceCode += "uniform vec3 MaterialDiffuse;\n";

		if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += "uniform sampler2D MaterialDiffuseMap;\n";

		sourceCode += '\n';

		/********************Entrant********************/
		if (flags & nzShaderBuilder_DiffuseMapping)
		{
			sourceCode += inKW;
			sourceCode += " vec2 vTexCoord;\n";
		}

		sourceCode += '\n';

		/********************Sortant********************/
		if (useMRT)
			sourceCode += "out vec4 RenderTarget0;\n";

		sourceCode += '\n';

		/********************Code********************/
		sourceCode += "void main()\n{\n";

		sourceCode += '\t';
		sourceCode += fragmentColorKW;
		if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += " = texture2D(MaterialDiffuseMap, vTexCoord);\n";
		else
			sourceCode += " = vec4(MaterialDiffuse, 1.0);\n";

		sourceCode += '}';

		sourceCode += '\n';

		return sourceCode;
	}

	NzString BuildVertexShaderSource(nzUInt32 flags)
	{
		bool glsl140 = (NzOpenGL::GetVersion() >= 300);

		NzString inKW = (glsl140) ? "in" : "attribute";
		NzString outKW = (glsl140) ? "out" : "varying";

		NzString sourceCode;
		sourceCode.Reserve(256); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

		/********************Version de GLSL********************/
		sourceCode = "#version ";
		if (glsl140)
			sourceCode += "140\n";
		else
			sourceCode += "110\n";

		sourceCode += '\n';

		/********************Uniformes********************/
		sourceCode += "uniform mat4 WorldViewProjMatrix;\n";

		sourceCode += '\n';

		/********************Entrant********************/
		sourceCode += inKW;
		sourceCode += " vec3 VertexPosition;\n";

		if (flags & nzShaderBuilder_Lighting || flags & nzShaderBuilder_NormalMapping || flags & nzShaderBuilder_ParallaxMapping)
		{
			sourceCode += inKW;
			sourceCode += " vec3 VertexNormal;\n";
		}

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += inKW;
			sourceCode += " vec3 VertexTangent;\n";
		}

		if (flags & nzShaderBuilder_DiffuseMapping || flags & nzShaderBuilder_Lighting || flags & nzShaderBuilder_NormalMapping || flags & nzShaderBuilder_ParallaxMapping)
		{
			sourceCode += inKW;
			sourceCode += " vec2 VertexTexCoord0;\n";
		}

		sourceCode += '\n';

		/********************Sortant********************/
		if (flags & nzShaderBuilder_DiffuseMapping)
		{
			sourceCode += outKW;
			sourceCode += " vec2 vTexCoord;\n";
		}

		sourceCode += '\n';

		/********************Code********************/
		sourceCode += "void main()\n{\n";

		sourceCode += "\tgl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n";

		if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += "\tvTexCoord = VertexTexCoord0;\n";

		sourceCode += '}';

		sourceCode += '\n';

		return sourceCode;
	}
}

const NzShader* NzShaderBuilder::Get(nzUInt32 flags)
{
	auto it = s_shaders.find(flags);
	if (it == s_shaders.end())
	{
		// Alors nous créons le shader
		NzShader* shader = new NzShader;
		if (!shader->Create(nzShaderLanguage_GLSL))
		{
			NazaraError("Failed to create shader");
			return nullptr;
		}

		if (!shader->Load(nzShaderType_Fragment, BuildFragmentShaderSource(flags)))
		{
			NazaraError("Failed to load fragment shader: " + shader->GetLog());
			return nullptr;
		}

		if (!shader->Load(nzShaderType_Vertex, BuildVertexShaderSource(flags)))
		{
			NazaraError("Failed to load vertex shader: " + shader->GetLog());
			return nullptr;
		}

		if (!shader->Compile())
		{
			NazaraError("Failed to compile shader: " + shader->GetLog());
			return nullptr;
		}

		s_shaders[flags] = shader;

		return shader;
	}
	else
		return it->second;
}

bool NzShaderBuilder::Initialize()
{
	return true;
}

void NzShaderBuilder::Uninitialize()
{
	for (auto it : s_shaders)
		delete it.second;

	s_shaders.clear();
}

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <memory>
#include <unordered_map>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	std::unordered_map<nzUInt32, NzShader*> s_shaders;

	NzString BuildFragmentShaderSource(nzUInt32 flags)
	{
		bool glsl140 = (NzOpenGL::GetVersion() >= 310);
		//bool useMRT = (glsl140 && NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets));

		NzString inKW = (glsl140) ? "in" : "varying";
		NzString fragmentColorKW = (glsl140) ? "RenderTarget0" : "gl_FragColor";

		NzString sourceCode;
		sourceCode.Reserve(1024); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

		/********************Préprocesseur********************/
		sourceCode = "#version ";
		if (glsl140)
			sourceCode += "140\n";
		else
			sourceCode += "110\n";

		sourceCode += '\n';

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += "#define LIGHT_DIRECTIONAL 0\n"
			              "#define LIGHT_POINT 1\n"
			              "#define LIGHT_SPOT 2\n"
			              "#define MAX_LIGHTS 8\n"
			              "\n";
		}

		sourceCode += '\n';

		/********************Uniformes********************/
		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += "struct Light\n"
			              "{\n"
			              "int type;\n"
			              "vec4 ambient;\n"
			              "vec4 diffuse;\n"
			              "vec4 specular;\n"
			              "\n"
			              "vec4 parameters1;\n"
			              "vec4 parameters2;\n"
			              "vec2 parameters3;\n"
			              "};\n"
			              "\n";
		}

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += "uniform vec3 CameraPosition;\n"
			              "uniform int LightCount;\n"
			              "uniform Light Lights[MAX_LIGHTS];\n"
			              "uniform vec4 MaterialAmbient;\n";
		}

		if ((flags & nzShaderBuilder_DiffuseMapping) == 0 || flags & nzShaderBuilder_Lighting)
			sourceCode += "uniform vec4 MaterialDiffuse;\n";

		if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += "uniform sampler2D MaterialDiffuseMap;\n";

		if (flags & nzShaderBuilder_Lighting)
		{
			if (flags & nzShaderBuilder_NormalMapping)
				sourceCode += "uniform sampler2D MaterialNormalMap;\n";

			sourceCode += "uniform float MaterialShininess;\n"
			              "uniform vec4 MaterialSpecular;\n";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += "uniform sampler2D MaterialSpecularMap;\n";

			sourceCode += "uniform vec4 SceneAmbient;\n";
		}

		sourceCode += '\n';

		/********************Entrant********************/
		if (flags & nzShaderBuilder_Lighting)
		{
			if (flags & nzShaderBuilder_NormalMapping)
				sourceCode += inKW + " mat3 vLightToWorld;\n";
			else
				sourceCode += inKW + " vec3 vNormal;\n";
		}

		if (flags & nzShaderBuilder_DiffuseMapping || flags & nzShaderBuilder_NormalMapping)
			sourceCode += inKW + " vec2 vTexCoord;\n";

		if (flags & nzShaderBuilder_Lighting)
			sourceCode += inKW + " vec3 vWorldPos;\n";

		sourceCode += '\n';

		/********************Sortant********************/
		if (glsl140)
			sourceCode += "out vec4 RenderTarget0;\n";

		sourceCode += '\n';

		/********************Fonctions********************/
		sourceCode += "void main()\n"
		              "{\n";

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += "vec3 light = vec3(0.0, 0.0, 0.0);\n";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += "vec3 si = vec3(0.0, 0.0, 0.0);\n";

			if (flags & nzShaderBuilder_NormalMapping)
				sourceCode += "vec3 normal = normalize(vLightToWorld * (2.0 * vec3(texture2D(MaterialNormalMap, vTexCoord)) - 1.0));\n";
			else
				sourceCode += "vec3 normal = normalize(vNormal);\n";

			sourceCode += "\n"
			              "for (int i = 0; i < LightCount; ++i)\n"
			              "{\n";

			if (glsl140)
			{
				sourceCode += "switch (Lights[i].type)\n"
				              "{\n"
				              "case LIGHT_DIRECTIONAL:\n";
			}
			else // Le GLSL 110 n'a pas d'instruction switch
				sourceCode += "if (Lights[i].type == LIGHT_DIRECTIONAL)\n";

			// Directional Light
			sourceCode += "{\n"
			              "vec3 lightDir = normalize(-Lights[i].parameters1.xyz);\n"
			              "light += Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);\n"
			              "\n"
			              "float lambert = max(dot(normal, lightDir), 0.0);\n"
			              "light += lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;\n"
			              "\n"
			              "if (MaterialShininess > 0.0)\n"
			              "{\n"
			              "vec3 eyeVec = normalize(CameraPosition - vWorldPos);\n"
			              "vec3 reflection = reflect(-lightDir, normal);\n"
			              "\n"
			              "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);\n";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += "si";
			else
				sourceCode += "light";

			sourceCode += " += specular * Lights[i].specular.rgb * MaterialSpecular.rgb;\n"
			              "}\n";

			if (glsl140)
			{
				sourceCode += "break;\n"
				              "}\n"
				              "\n"
				              "case LIGHT_POINT:\n";
			}
			else
				sourceCode += "}\n"
				              "else if (Lights[i].type == LIGHT_POINT)\n";

			// Point Light
			sourceCode += "{\n"
			              "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;\n"
			              "\n"
			              "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);\n"
			              "light += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);\n"
			              "\n"
			              "lightDir = normalize(lightDir);\n"
			              "float lambert = max(dot(normal, lightDir), 0.0);\n"
			              "light += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;\n"
			              "\n"
			              "if (MaterialShininess > 0.0)\n"
			              "{\n"
			              "vec3 eyeVec = normalize(CameraPosition - vWorldPos);\n"
			              "vec3 reflection = reflect(-lightDir, normal);\n"
			              "\n"
			              "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);\n";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += "si";
			else
				sourceCode += "light";

			sourceCode += " += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;\n"
			              "}\n";

			if (glsl140)
			{
				sourceCode += "break;\n"
				              "}\n"
				              "\n"
				              "case LIGHT_SPOT:\n";
			}
			else
			{
				sourceCode += "}\n"
				              "else if (Lights[i].type == LIGHT_SPOT)\n";
			}

			// Spot Light
			sourceCode += "{\n"
			              "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;\n"
			              "\n"
			              "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*length(lightDir), 0.0);\n"
			              "light += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);\n"
			              "\n"
			              "lightDir = normalize(lightDir);\n"
			              "\n"
			              "float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);\n"
			              "float outerAngle = Lights[i].parameters3.y;\n"
			              "float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;\n"
			              "float lambert = max(dot(normal, lightDir), 0.0);\n"
			              "att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);\n"
			              "light += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;\n"
			              "\n"
			              "if (MaterialShininess > 0.0)\n"
			              "{\n"
			              "vec3 eyeVec = normalize(CameraPosition - vWorldPos);\n"
			              "vec3 reflection = reflect(-lightDir, normal);\n"
			              "\n"
			              "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);\n";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += "si";
			else
				sourceCode += "light";

			sourceCode += " += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;\n"
			              "}\n";

			if (glsl140)
			{
				sourceCode += "break;\n"
				              "}\n"
				              "default:\n"
				              "break;\n"
				              "}\n";
			}
			else
				sourceCode += "}\n";

			sourceCode += "}\n"
			              "\n";

			sourceCode += fragmentColorKW + " = vec4(light, MaterialDiffuse.a)";

			if (flags & nzShaderBuilder_DiffuseMapping)
				sourceCode += "*texture2D(MaterialDiffuseMap, vTexCoord)";

			if (flags & nzShaderBuilder_SpecularMapping)
				sourceCode += " + vec4(si, MaterialDiffuse.a)*texture2D(MaterialSpecularMap, vTexCoord)"; // Utiliser l'alpha de MaterialSpecular n'aurait aucun sens

			sourceCode += ";\n";
		}
		else if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += fragmentColorKW + " = texture2D(MaterialDiffuseMap, vTexCoord);\n";
		else
			sourceCode += fragmentColorKW + " = MaterialDiffuse;\n";

		sourceCode += "}\n";

		return sourceCode;
	}

	NzString BuildVertexShaderSource(nzUInt32 flags)
	{
		bool glsl140 = (NzOpenGL::GetVersion() >= 310);

		NzString inKW = (glsl140) ? "in" : "attribute";
		NzString outKW = (glsl140) ? "out" : "varying";

		NzString sourceCode;
		sourceCode.Reserve(512); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

		/********************Version de GLSL********************/
		sourceCode = "#version ";
		if (glsl140)
			sourceCode += "140\n";
		else
			sourceCode += "110\n";

		sourceCode += '\n';

		/********************Uniformes********************/
		if (flags & nzShaderBuilder_Lighting)
			sourceCode += "uniform mat4 WorldMatrix;\n";

		sourceCode += "uniform mat4 WorldViewProjMatrix;\n";

		sourceCode += '\n';

		/********************Entrant********************/
		sourceCode += inKW + " vec3 VertexPosition;\n";

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += inKW + " vec3 VertexNormal;\n";
			sourceCode += inKW + " vec3 VertexTangent;\n";
		}

		if (flags & nzShaderBuilder_DiffuseMapping)
			sourceCode += inKW + " vec2 VertexTexCoord0;\n";

		sourceCode += '\n';

		/********************Sortant********************/
		if (flags & nzShaderBuilder_Lighting)
		{
			if (flags & nzShaderBuilder_NormalMapping)
				sourceCode += outKW + " mat3 vLightToWorld;\n";
			else
				sourceCode += outKW + " vec3 vNormal;\n";
		}

		if (flags & nzShaderBuilder_DiffuseMapping || flags & nzShaderBuilder_NormalMapping)
			sourceCode += outKW + " vec2 vTexCoord;\n";

		if (flags & nzShaderBuilder_Lighting)
			sourceCode += outKW + " vec3 vWorldPos;\n";

		sourceCode += '\n';

		/********************Code********************/
		sourceCode += "void main()\n"
		              "{\n"
		              "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n";

		if (flags & nzShaderBuilder_Lighting)
		{
			sourceCode += "mat3 RotationMatrix = mat3(WorldMatrix);\n";

			if (flags & nzShaderBuilder_NormalMapping)
			{
				sourceCode += "\n"
				              "vec3 binormal = cross(VertexNormal, VertexTangent);\n"
				              "vLightToWorld[0] = normalize(VertexTangent * RotationMatrix);\n"
				              "vLightToWorld[1] = normalize(binormal * RotationMatrix);\n"
				              "vLightToWorld[2] = normalize(VertexNormal * RotationMatrix);\n"
				              "\n";
			}
			else
				sourceCode += "vNormal = normalize(RotationMatrix * VertexNormal);\n";
		}

		if (flags & nzShaderBuilder_DiffuseMapping || flags & nzShaderBuilder_NormalMapping)
			sourceCode += "vTexCoord = VertexTexCoord0;\n";

		if (flags & nzShaderBuilder_Lighting)
			sourceCode += "vWorldPos = vec3(WorldMatrix * vec4(VertexPosition, 1.0));\n";

		sourceCode += "}\n";

		return sourceCode;
	}

	NzShader* BuildShader(nzUInt32 flags)
	{
		std::unique_ptr<NzShader> shader(new NzShader);
		if (!shader->Create(nzShaderLanguage_GLSL))
		{
			NazaraError("Failed to create shader");
			return nullptr;
		}

		NzString fragmentSource = BuildFragmentShaderSource(flags);
		if (!shader->Load(nzShaderType_Fragment, fragmentSource))
		{
			NazaraError("Failed to load fragment shader: " + shader->GetLog());
			NazaraNotice("Fragment shader source: ");
			NazaraNotice(fragmentSource);
			return nullptr;
		}

		NzString vertexSource = BuildVertexShaderSource(flags);
		if (!shader->Load(nzShaderType_Vertex, vertexSource))
		{
			NazaraError("Failed to load vertex shader: " + shader->GetLog());
			NazaraNotice("Vertex shader source: ");
			NazaraNotice(vertexSource);
			return nullptr;
		}

		#ifdef NAZARA_DEBUG
		NazaraNotice("Fragment shader source: ");
		NazaraNotice(fragmentSource);
		NazaraNotice("Vertex shader source: ");
		NazaraNotice(vertexSource);
		#endif

		if (!shader->Compile())
		{
			NazaraError("Failed to compile shader: " + shader->GetLog());
			return nullptr;
		}

		return shader.release();
	}
}

const NzShader* NzShaderBuilder::Get(nzUInt32 flags)
{
	auto it = s_shaders.find(flags);
	if (it == s_shaders.end())
	{
		// Alors nous créons le shader
		NzShader* shader = BuildShader(flags);
		if (!shader) ///TODO: Ajouter une erreur en mode Once
			return s_shaders[0]; // Shader par défaut

		s_shaders[flags] = shader;
		///TODO: emplace

		return shader;
	}
	else
		return it->second;
}

bool NzShaderBuilder::Initialize()
{
	NzShader* shader = BuildShader(0);
	if (!shader)
	{
		NazaraInternalError("Failed to build default shader");
		return false;
	}

	s_shaders[0] = shader;

	return true;
}

void NzShaderBuilder::Uninitialize()
{
	for (auto it : s_shaders)
		delete it.second;

	s_shaders.clear();
}

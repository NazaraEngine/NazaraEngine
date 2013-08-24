// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	struct ParamsHash
	{
		std::size_t operator()(const NzShaderProgramManagerParams& params) const
		{
			static_assert(nzShaderTarget_Max < 0x4, "Maximum shader target takes more than 2 bits");

			std::size_t h = (params.target << 0) | // 2 bits
			                (params.flags  << 2);  // 8 bits

			switch (params.target)
			{
				case nzShaderTarget_FullscreenQuad:
					h |= (params.fullscreenQuad.alphaMapping   << 10) | // 1 bit
					     (params.fullscreenQuad.alphaTest      << 11) | // 1 bit
					     (params.fullscreenQuad.diffuseMapping << 12);  // 1 bit
					break;

				case nzShaderTarget_Model:
					h |= (params.model.alphaMapping    << 10) | // 1 bit
					     (params.model.alphaTest       << 11) | // 1 bit
					     (params.model.diffuseMapping  << 12) | // 1 bit
					     (params.model.emissiveMapping << 13) | // 1 bit
					     (params.model.lighting        << 14) | // 1 bit
					     (params.model.normalMapping   << 15) | // 1 bit
					     (params.model.parallaxMapping << 16) | // 1 bit
					     (params.model.specularMapping << 17);  // 1 bit
					break;

				case nzShaderTarget_None:
					break;

				case nzShaderTarget_Sprite:
					h |= (params.sprite.alphaMapping   << 10) | // 1 bit
					     (params.sprite.alphaTest      << 11) | // 1 bit
					     (params.sprite.diffuseMapping << 12);  // 1 bit
					break;
			}

			return h;
		}
	};

	struct ParamsEquality
	{
		bool operator()(const NzShaderProgramManagerParams& first, const NzShaderProgramManagerParams& second) const
		{
			if (first.target != second.target || first.flags != second.flags)
				return false;

			switch (first.target)
			{
				case nzShaderTarget_FullscreenQuad:
					return std::memcmp(&first.fullscreenQuad, &second.fullscreenQuad, sizeof(NzShaderProgramManagerParams::FullscreenQuad)) == 0;

				case nzShaderTarget_Model:
					return std::memcmp(&first.model, &second.model, sizeof(NzShaderProgramManagerParams::Model)) == 0;

				case nzShaderTarget_None:
					return true;

				case nzShaderTarget_Sprite:
					return std::memcmp(&first.sprite, &second.sprite, sizeof(NzShaderProgramManagerParams::Sprite)) == 0;
			}

			return false;
		}
	};

	std::unordered_map<NzShaderProgramManagerParams, NzShaderProgramRef, ParamsHash, ParamsEquality> s_programs;
	NzString s_cacheDirectory("shaderCache");
	NzString s_inKW;
	NzString s_outKW;
	NzString s_fragmentColorKW;
	NzString s_textureLookupKW;
	bool s_cacheEnabled = false;
	bool s_earlyFragmentTest;
	bool s_glsl140;
	//bool s_loadCachedPrograms = true;
	unsigned int s_glslVersion;
}

const NzShaderProgram* NzShaderProgramManager::Get(const NzShaderProgramManagerParams& params)
{
	auto it = s_programs.find(params);
	if (it == s_programs.end())
	{
		// Alors nous générons le programme
		std::unique_ptr<NzShaderProgram> program;

		if (s_cacheEnabled)
		{
			NzString programFileName = NzNumberToString(ParamsHash()(params), 36) + ".nsb"; // Nazara Shader Binary, très original, je sais
			NazaraDebug("Checking cache for program file \"" + programFileName + "\"...");

			NzFile shaderFile(s_cacheDirectory + NAZARA_DIRECTORY_SEPARATOR + programFileName);
			if (shaderFile.Open(NzFile::ReadOnly))
			{
				NazaraDebug("File found");

				unsigned int size = shaderFile.GetSize();

				NzByteArray binary;
				binary.Resize(size);

				if (shaderFile.Read(&binary[0], size) != size)
				{
					NazaraError("Failed to read program binary");
					return false;
				}

				shaderFile.Close();

				program.reset(new NzShaderProgram);
				if (!program->LoadFromBinary(binary))
				{
					NazaraWarning("Program \"" + programFileName + "\" loading failed, this is mostly due to a driver/video card "
					              "update or a file corruption, regenerating program...");

					program.reset(GenerateProgram(params));
				}
			}
			else
			{
				if (shaderFile.Exists())
					NazaraWarning("Program file exists but couldn't be opened");

				program.reset(GenerateProgram(params));
				if (program)
				{
					if (program->IsBinaryRetrievable())
					{
						NazaraDebug("Program \"" + programFileName + "\" (re)generated, saving it into program cache directory...");

						NzByteArray programBinary = program->GetBinary();
						if (!programBinary.IsEmpty())
						{
							if (!shaderFile.Open(NzFile::Truncate | NzFile::WriteOnly) || !shaderFile.Write(programBinary))
								NazaraWarning("Failed to save program binary to file \"" + programFileName + "\": " + NzGetLastSystemError());
						}
						else
							NazaraWarning("Failed to retrieve shader program binary");
					}
				}
			}
		}
		else
			program.reset(GenerateProgram(params));

		if (!program)
		{
			NazaraWarning("Failed to build program, using default one...");

			NzShaderProgramManagerParams defaultParams;
			defaultParams.flags = params.flags;
			defaultParams.target = nzShaderTarget_None;

			program.reset(s_programs[defaultParams]); // Shader par défaut
		}

		return program.release();
	}
	else
		return it->second;
}

NzString NzShaderProgramManager::BuildFragmentCode(const NzShaderProgramManagerParams& params)
{
	#ifdef NAZARA_DEBUG
	if (params.target > nzShaderTarget_Max)
	{
		NazaraError("Shader target out of enum");
		return NzString();
	}
	#endif

	NzString source;
	source.Reserve(2048); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

	/********************Header********************/
	source += "#version ";
	source += NzString::Number(s_glslVersion);
	source += "\n\n";

	switch (params.target)
	{
		case nzShaderTarget_FullscreenQuad:
		{
			// "discard" ne s'entend pas bien avec les early fragment tests
			if (s_earlyFragmentTest && !params.fullscreenQuad.alphaMapping)
				source += "layout(early_fragment_tests) in;" "\n\n";

			/********************Entrant********************/
			if (params.fullscreenQuad.alphaMapping || params.fullscreenQuad.diffuseMapping)
				source += s_inKW + " vec2 vTexCoord;" "\n\n";

			/********************Sortant********************/
			if (s_glsl140)
				source += "out vec4 RenderTarget0;" "\n\n";

			/********************Uniformes********************/
			if (params.fullscreenQuad.alphaMapping)
				source += "uniform sampler2D MaterialAlphaMap;" "\n";

			if (params.fullscreenQuad.alphaTest)
				source += "uniform float MaterialAlphaThreshold;" "\n";

			source += "uniform vec4 MaterialDiffuse;" "\n";

			if (params.fullscreenQuad.diffuseMapping)
				source += "uniform sampler2D MaterialDiffuseMap;" "\n";

			source += '\n';

			/********************Fonctions********************/
			source += "void main()" "\n"
			          "{" "\n";

			source += "\t" "vec4 fragmentColor = MaterialDiffuse";
			if (params.fullscreenQuad.diffuseMapping)
				source += '*' + s_textureLookupKW + "(MaterialDiffuseMap, vTexCoord)";

			source += ";" "\n";

			if (params.fullscreenQuad.alphaMapping)
				source += "fragmentColor.a *= " + s_textureLookupKW + "(MaterialAlphaMap, vTexCoord).r";

			source += ";" "\n";

			if (params.fullscreenQuad.alphaMapping)
			{
				source += "\t" "if (fragmentColor.a < MaterialAlphaThreshold)" "\n"
						  "\t\t" "discard;" "\n";
			}

			source += "\t" + s_fragmentColorKW + " = fragmentColor;" "\n"
					  "}" "\n";
			break;
		}

		case nzShaderTarget_Model:
		{
			//bool parallaxMapping = (params.model.lighting && params.model.parallaxMapping);
			bool uvMapping = (params.model.alphaMapping || params.model.diffuseMapping || params.model.normalMapping || params.model.specularMapping);

			// "discard" ne s'entend pas bien avec les early fragment tests
			if (s_earlyFragmentTest && !params.model.alphaMapping)
				source += "layout(early_fragment_tests) in;" "\n\n";

			if (params.model.lighting)
			{
				source += "#define LIGHT_DIRECTIONAL 0" "\n"
				          "#define LIGHT_POINT 1" "\n"
				          "#define LIGHT_SPOT 2" "\n\n";
			}

			/********************Entrant********************/
			if (params.model.lighting)
			{
				if (params.model.normalMapping)
					source += s_inKW + " mat3 vLightToWorld;" "\n";
				else
					source += s_inKW + " vec3 vNormal;" "\n";
			}

			if (uvMapping)
				source += s_inKW + " vec2 vTexCoord;" "\n";

			if (params.model.lighting)
				source += s_inKW + " vec3 vWorldPos;" "\n";

			if (params.model.lighting || uvMapping)
				source += '\n';

			/********************Sortant********************/
			if (s_glsl140)
				source += "out vec4 RenderTarget0;" "\n\n";

			/********************Uniformes********************/
			if (params.model.lighting)
			{
				source += "struct Light" "\n"
				          "{" "\n"
				          "\t" "int type;" "\n"
				          "\t" "vec4 ambient;" "\n"
				          "\t" "vec4 diffuse;" "\n"
				          "\t" "vec4 specular;" "\n\n"

				          "\t" "vec4 parameters1;" "\n"
				          "\t" "vec4 parameters2;" "\n"
				          "\t" "vec2 parameters3;" "\n"
				          "};" "\n\n"

				          "uniform vec3 EyePosition;" "\n"
				          "uniform Light Lights[3];" "\n"
				          "uniform vec4 MaterialAmbient;" "\n";
			}

			if (params.model.alphaMapping)
				source += "uniform sampler2D MaterialAlphaMap;" "\n";

			if (params.model.alphaTest)
				source += "uniform float MaterialAlphaThreshold;" "\n";

			source += "uniform vec4 MaterialDiffuse;" "\n";

			if (params.model.diffuseMapping)
				source += "uniform sampler2D MaterialDiffuseMap;" "\n";

			if (params.model.emissiveMapping)
				source += "uniform sampler2D MaterialEmissiveMap;" "\n";

			if (params.model.lighting)
			{
				if (params.model.normalMapping)
					source += "uniform sampler2D MaterialNormalMap;" "\n";

				source += "uniform float MaterialShininess;" "\n"
				          "uniform vec4 MaterialSpecular;" "\n";

				if (params.model.specularMapping)
					source += "uniform sampler2D MaterialSpecularMap;" "\n";

				source += "uniform vec4 SceneAmbient;" "\n";
			}

			source += '\n';

			/********************Fonctions********************/
			source += "void main()" "\n"
			          "{" "\n";

			/*if (!parallaxMapping)
			{*/
				source += "\t" "vec4 fragmentColor = MaterialDiffuse";
				if (params.model.diffuseMapping)
					source += '*' + s_textureLookupKW + "(MaterialDiffuseMap, vTexCoord)";

				source += ";" "\n";
			//}

			if (params.model.alphaMapping)
			{
				source += "\t" "fragmentColor.a *= ";

				if (params.model.alphaMapping)
					source += s_textureLookupKW + "(MaterialAlphaMap, vTexCoord).r";

				/*if (parallaxMapping && params.model.diffuseMapping)
					source += '*' + s_textureLookupKW + "(MaterialDiffuseMap, vTexCoord).a";*/

				source += ";" "\n";
			}

			if (params.model.alphaTest)
			{
				source += "\t" "if (fragmentColor.a < MaterialAlphaThreshold)" "\n"
						  "\t\t" "discard;" "\n";
			}

			if (params.model.lighting)
			{
				source += "\t" "vec3 lightColor = vec3(0.0);" "\n";

				if (params.model.specularMapping)
					source += "\t" "vec3 specularColor = vec3(0.0);" "\n";

				if (params.model.normalMapping)
					source += "\t" "vec3 normal = normalize(vLightToWorld * (2.0 * vec3(" + s_textureLookupKW + "(MaterialNormalMap, vTexCoord)) - 1.0));" "\n";
				else
					source += "\t" "vec3 normal = normalize(vNormal);" "\n";

				source += '\n';

				source += "\t" "if (MaterialShininess > 0.0)" "\n"
				          "\t" "{" "\n"
				          "\t\t" "vec3 eyeVec = normalize(EyePosition - vWorldPos);" "\n\n"

				          "\t\t" "for (int i = 0; i < 3; ++i)" "\n"
				          "\t\t" "{" "\n";

				if (s_glsl140)
				{
					source += "\t\t\t" "switch (Lights[i].type)" "\n"
					          "\t\t\t" "{" "\n"
					          "\t\t\t\t" "case LIGHT_DIRECTIONAL:" "\n";
				}
				else // Le GLSL 110 n'a pas d'instruction switch
					source += "\t\t\t" "if (Lights[i].type == LIGHT_DIRECTIONAL)" "\n";

				// Directional Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = normalize(-Lights[i].parameters1.xyz);" "\n"
				          "\t\t\t\t\t" "lightColor += Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n\n"

				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n"

				          "\t\t\t\t\t" "vec3 reflection = reflect(-lightDir, normal);" "\n"
				          "\t\t\t\t\t" "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);" "\n";

				if (params.model.specularMapping)
					source += "\t\t\t\t\t" "specularColor";
				else
					source += "\t\t\t\t\t" "lightColor";

				source += " += specular * Lights[i].specular.rgb * MaterialSpecular.rgb;" "\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"

					          "\t\t\t\t" "case LIGHT_POINT:" "\n";
				}
				else
					source += "\t\t\t" "}" "\n"
					          "\t\t\t" "else if (Lights[i].type == LIGHT_POINT)" "\n";

				// Point Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;" "\n\n"

				          "\t\t\t\t\t" "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n"

				          "\t\t\t\t\t" "lightDir = normalize(lightDir);" "\n"
				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n"

				          "\t\t\t\t\t" "vec3 reflection = reflect(-lightDir, normal);" "\n"
				          "\t\t\t\t\t" "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);" "\n";

				if (params.model.specularMapping)
					source += "\t\t\t\t\t" "specularColor";
				else
					source += "\t\t\t\t\t" "lightColor";

				source += " += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;" "\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"

					          "\t\t\t\t" "case LIGHT_SPOT:" "\n";
				}
				else
				{
					source += "\t\t\t" "}" "\n"
					          "\t\t\t" "else if (Lights[i].type == LIGHT_SPOT)" "\n";
				}

				// Spot Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;" "\n\n"

				          "\t\t\t\t\t" "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*length(lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n\n"

				          "\t\t\t\t\t" "lightDir = normalize(lightDir);" "\n\n"

				          "\t\t\t\t\t" "float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);" "\n"
				          "\t\t\t\t\t" "float outerAngle = Lights[i].parameters3.y;" "\n"
				          "\t\t\t\t\t" "float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;" "\n"
				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n"

				          "\t\t\t\t\t" "vec3 reflection = reflect(-lightDir, normal);" "\n"
				          "\t\t\t\t\t" "float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);" "\n";

				if (params.model.specularMapping)
					source += "\t\t\t\t\t" "specularColor";
				else
					source += "\t\t\t\t\t" "lightColor";

				source += " += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"
					          "\t\t\t\t" "default:" "\n"
					          "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t" "}" "\n";
				}
				else
					source += "\t\t\t" "}" "\n";

				source += "\t\t" "}" "\n"
				          "\t" "}" "\n"
				          "\t" "else" "\n"
				          "\t" "{" "\n";

				source += "\t\t" "for (int i = 0; i < 3; ++i)" "\n"
				          "\t\t" "{" "\n";

				if (s_glsl140)
				{
					source += "\t\t\t" "switch (Lights[i].type)" "\n"
					          "\t\t\t" "{" "\n"
					          "\t\t\t\t" "case LIGHT_DIRECTIONAL:" "\n";
				}
				else // Le GLSL 110 n'a pas d'instruction switch
					source += "\t\t\t" "if (Lights[i].type == LIGHT_DIRECTIONAL)" "\n";

				// Directional Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = normalize(-Lights[i].parameters1.xyz);" "\n"
				          "\t\t\t\t\t" "lightColor += Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n\n"

				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"

					          "\t\t\t\t" "case LIGHT_POINT:" "\n";
				}
				else
					source += "\t\t\t" "}" "\n"
					          "\t\t\t" "else if (Lights[i].type == LIGHT_POINT)" "\n";

				// Point Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;" "\n\n"

				          "\t\t\t\t\t" "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n"

				          "\t\t\t\t\t" "lightDir = normalize(lightDir);" "\n"
				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"

					          "\t\t\t\t" "case LIGHT_SPOT:" "\n";
				}
				else
				{
					source += "\t\t\t" "}" "\n"
					          "\t\t\t" "else if (Lights[i].type == LIGHT_SPOT)" "\n";
				}

				// Spot Light
				source += "\t\t\t\t" "{" "\n"
				          "\t\t\t\t\t" "vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;" "\n\n"

				          "\t\t\t\t\t" "float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*length(lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);" "\n\n"

				          "\t\t\t\t\t" "lightDir = normalize(lightDir);" "\n\n"

				          "\t\t\t\t\t" "float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);" "\n"
				          "\t\t\t\t\t" "float outerAngle = Lights[i].parameters3.y;" "\n"
				          "\t\t\t\t\t" "float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;" "\n"
				          "\t\t\t\t\t" "float lambert = max(dot(normal, lightDir), 0.0);" "\n"
				          "\t\t\t\t\t" "att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);" "\n"
				          "\t\t\t\t\t" "lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;" "\n\n";

				if (s_glsl140)
				{
					source += "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t\t" "}" "\n\n"
					          "\t\t\t\t" "default:" "\n"
					          "\t\t\t\t\t" "break;" "\n"
					          "\t\t\t" "}" "\n";
				}
				else
					source += "\t\t\t" "}" "\n";

				source += "\t\t" "}" "\n"
				          "\t" "}" "\n\n"

				          "\t" "fragmentColor *= vec4(lightColor";

				if (params.model.specularMapping)
					source += "+ specularColor*" + s_textureLookupKW + "(MaterialSpecularMap, vTexCoord).rgb"; // Utiliser l'alpha de MaterialSpecular n'aurait aucun sens

				source += ", 1.0);" "\n";

				if (params.model.emissiveMapping)
				{
					if (params.model.specularMapping)
						source += "\t" "float lightIntensity = dot(lightColor + specularColor, vec3(0.3, 0.59, 0.11));" "\n";
					else
						source += "\t" "float lightIntensity = dot(lightColor, vec3(0.3, 0.59, 0.11));" "\n";

					source += "\t" "vec3 emissionColor = MaterialDiffuse.rgb*" + s_textureLookupKW + "(MaterialEmissiveMap, vTexCoord).rgb;" "\n"
					          "\t" + s_fragmentColorKW + " = vec4(mix(fragmentColor.rgb, emissionColor, clamp(1.0 - 3.0*lightIntensity, 0.0, 1.0)), fragmentColor.a);" "\n";
				}
				else
					source += '\t' + s_fragmentColorKW + " = fragmentColor;" "\n";
			}
			else
				source += '\t' + s_fragmentColorKW + " = fragmentColor;" "\n";

			source += "}" "\n";
			break;
		}

		case nzShaderTarget_None:
		{
			if (s_earlyFragmentTest)
				source += "layout(early_fragment_tests) in;" "\n\n";

			/********************Sortant********************/
			if (s_glsl140)
				source += "out vec4 RenderTarget0;" "\n\n";

			/********************Uniformes********************/
			source += "uniform vec4 MaterialDiffuse;" "\n\n";

			/********************Fonctions********************/
			source += "void main()" "\n"
			          "{" "\n";
			source += '\t' + s_fragmentColorKW + " = MaterialDiffuse;" "\n";
			source += "}" "\n";
			break;
		}

		case nzShaderTarget_Sprite:
		{
			// "discard" ne s'entend pas bien avec les early fragment tests
			if (s_earlyFragmentTest && !params.sprite.alphaMapping)
				source += "layout(early_fragment_tests) in;" "\n\n";

			/********************Entrant********************/
			if (params.sprite.alphaMapping || params.sprite.diffuseMapping)
				source += s_inKW + " vec2 vTexCoord;" "\n\n";

			/********************Sortant********************/
			if (s_glsl140)
				source += "out vec4 RenderTarget0;" "\n\n";

			/********************Uniformes********************/
			if (params.sprite.alphaMapping)
				source += "uniform sampler2D MaterialAlphaMap;" "\n";

			if (params.sprite.alphaTest)
				source += "uniform float MaterialAlphaThreshold;" "\n";

			source += "uniform vec4 MaterialDiffuse;" "\n";

			if (params.sprite.diffuseMapping)
				source += "uniform sampler2D MaterialDiffuseMap;" "\n";

			source += '\n';

			/********************Fonctions********************/
			source += "void main()" "\n"
			          "{" "\n";

			source += "\t" "vec4 fragmentColor = MaterialDiffuse";
			if (params.sprite.diffuseMapping)
				source += '*' + s_textureLookupKW + "(MaterialDiffuseMap, vTexCoord)";

			source += ";" "\n";

			if (params.sprite.alphaMapping)
				source += "fragmentColor.a *= " + s_textureLookupKW + "(MaterialAlphaMap, vTexCoord).r";

			source += ";" "\n";

			if (params.sprite.alphaMapping)
			{
				source += "\t" "if (fragmentColor.a < MaterialAlphaThreshold)" "\n"
						  "\t\t" "discard;" "\n";
			}

			source += "\t" + s_fragmentColorKW + " = fragmentColor;" "\n"
					  "}" "\n";
			break;
		}
	}

	return source;
}

NzString NzShaderProgramManager::BuildVertexCode(const NzShaderProgramManagerParams& params)
{
	#ifdef NAZARA_DEBUG
	if (params.target > nzShaderTarget_Max)
	{
		NazaraError("Shader target out of enum");
		return NzString();
	}
	#endif

	NzString source;
	source.Reserve(2048); // Le shader peut faire plus, mais cela évite déjà beaucoup de petites allocations

	/********************Header********************/
	source += "#version ";
	source += NzString::Number(s_glslVersion);
	source += "\n\n";

	switch (params.target)
	{
		case nzShaderTarget_FullscreenQuad:
		{
			bool uvMapping = (params.fullscreenQuad.alphaMapping || params.fullscreenQuad.diffuseMapping);

			/********************Entrant********************/
			source += s_inKW + " vec2 VertexPosition;" "\n\n";

			/********************Sortant********************/
			if (uvMapping)
				source += s_outKW + " vec2 vTexCoord;" "\n\n";

			/********************Code********************/
			source += "void main()" "\n"
			          "{" "\n"
			          "\t" "gl_Position = vec4(VertexPosition, 0.0, 1.0);" "\n";

			if (uvMapping)
			{
				if (params.flags & nzShaderFlags_FlipUVs)
					source += "\t" "vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, 0.5 - VertexPosition.y*0.5;" "\n";
				else
					source += "\t" "vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, (VertexPosition.y + 1.0)*0.5);" "\n";
			}

			source += "}" "\n";
			break;
		}

		case nzShaderTarget_Model:
		{
			bool uvMapping = (params.model.diffuseMapping || params.model.normalMapping || params.model.specularMapping);

			/********************Entrant********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += s_inKW + " mat4 InstanceData0;" "\n";

			source += s_inKW + " vec3 VertexPosition;" "\n";

			if (params.model.lighting)
			{
				source += s_inKW + " vec3 VertexNormal;" "\n";
				source += s_inKW + " vec3 VertexTangent;" "\n";
			}

			if (uvMapping)
				source += s_inKW + " vec2 VertexTexCoord;" "\n";

			source += '\n';

			/********************Sortant********************/
			if (params.model.lighting)
			{
				if (params.model.normalMapping)
					source += s_outKW + " mat3 vLightToWorld;" "\n";
				else
					source += s_outKW + " vec3 vNormal;" "\n";
			}

			if (uvMapping)
				source += s_outKW + " vec2 vTexCoord;" "\n";

			if (params.model.lighting)
				source += s_outKW + " vec3 vWorldPos;" "\n";

			if (params.model.lighting || uvMapping)
				source += '\n';

			/********************Uniformes********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += "uniform mat4 ViewProjMatrix;" "\n";
			else
			{
				if (params.model.lighting)
					source += "uniform mat4 WorldMatrix;" "\n";

				source += "uniform mat4 WorldViewProjMatrix;" "\n";
			}

			source += '\n';

			/********************Code********************/
			source += "void main()" "\n"
			          "{" "\n";

			if (params.flags & nzShaderFlags_Instancing)
				source += "\t" "gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);" "\n";
			else
				source += "\t" "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);" "\n";

			if (params.model.lighting)
			{
				if (params.flags & nzShaderFlags_Instancing)
				{
					if (s_glsl140)
						source += "\t" "mat3 rotationMatrix = mat3(InstanceData0);" "\n";
					else
						source += "\t" "mat3 rotationMatrix = mat3(InstanceData0[0].xyz, InstanceData0[1].xyz, InstanceData0[2].xyz);" "\n";
				}
				else
				{
					if (s_glsl140)
						source += "\t" "mat3 rotationMatrix = mat3(WorldMatrix);" "\n";
					else
						source += "\t" "mat3 rotationMatrix = mat3(WorldMatrix[0].xyz, WorldMatrix[1].xyz, WorldMatrix[2].xyz);" "\n";
				}

				if (params.model.normalMapping)
				{
					source += "\n"
					          "\t" "vec3 binormal = cross(VertexNormal, VertexTangent);" "\n"
					          "\t" "vLightToWorld[0] = normalize(rotationMatrix * VertexTangent);" "\n"
					          "\t" "vLightToWorld[1] = normalize(rotationMatrix * binormal);" "\n"
					          "\t" "vLightToWorld[2] = normalize(rotationMatrix * VertexNormal);" "\n\n";
				}
				else
					source += "\t" "vNormal = normalize(rotationMatrix * VertexNormal);" "\n";
			}

			if (uvMapping)
			{
				if (params.flags & nzShaderFlags_FlipUVs)
					source += "\t" "vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);" "\n";
				else
					source += "\t" "vTexCoord = VertexTexCoord;" "\n";
			}

			if (params.model.lighting)
			{
				if (params.flags & nzShaderFlags_Instancing)
					source += "\t" "vWorldPos = vec3(InstanceData0 * vec4(VertexPosition, 1.0));" "\n";
				else
					source += "\t" "vWorldPos = vec3(WorldMatrix * vec4(VertexPosition, 1.0));" "\n";
			}

			source += "}" "\n";
			break;
		}

		case nzShaderTarget_None:
		{
			/********************Entrant********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += s_inKW + " mat4 InstanceData0;" "\n";

			source += s_inKW + " vec3 VertexPosition;" "\n\n";

			/********************Uniformes********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += "uniform mat4 ViewProjMatrix;" "\n\n";
			else
				source += "uniform mat4 WorldViewProjMatrix;" "\n\n";

			/********************Code********************/
			source += "void main()" "\n"
			          "{" "\n";

			if (params.flags & nzShaderFlags_Instancing)
				source += "\t" "gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);" "\n";
			else
				source += "\t" "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);" "\n";

			source += "}" "\n";
			break;
		}

		case nzShaderTarget_Sprite:
		{
			bool uvMapping = (params.fullscreenQuad.alphaMapping || params.fullscreenQuad.diffuseMapping);

			/********************Entrant********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += s_inKW + " mat4 InstanceData0;" "\n";

			source += s_inKW + " vec3 VertexPosition;" "\n";

			if (uvMapping)
				source += s_inKW + " vec2 VertexTexCoord;" "\n";

			source += '\n';

			/********************Sortant********************/
			if (uvMapping)
				source += s_outKW + " vec2 vTexCoord;" "\n\n";

			/********************Uniformes********************/
			if (params.flags & nzShaderFlags_Instancing)
				source += "uniform mat4 ViewProjMatrix;" "\n";
			else
				source += "uniform mat4 WorldViewProjMatrix;" "\n";

			source += '\n';

			/********************Code********************/
			source += "void main()" "\n"
			          "{" "\n";

			if (params.flags & nzShaderFlags_Instancing)
				source += "\t" "gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);" "\n";
			else
				source += "\t" "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);" "\n";

			if (uvMapping)
			{
				if (params.flags & nzShaderFlags_FlipUVs)
					source += "\t" "vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);" "\n";
				else
					source += "\t" "vTexCoord = VertexTexCoord;" "\n";
			}

			source += "}" "\n";
			break;
		}
	}

	return source;
}

NzShaderProgram* NzShaderProgramManager::GenerateProgram(const NzShaderProgramManagerParams& params)
{
	std::unique_ptr<NzShaderProgram> program(new NzShaderProgram);
	program->SetPersistent(false);

	if (!program->Create(nzShaderLanguage_GLSL))
	{
		NazaraError("Failed to create program");
		return nullptr;
	}

	NzString fragmentSource = BuildFragmentCode(params);
	NazaraDebug("Fragment shader source:\n" + fragmentSource);
	if (!program->LoadShader(nzShaderType_Fragment, fragmentSource))
	{
		NazaraError("Failed to load fragment shader: " + program->GetLog());
		NazaraNotice("Source:\n" + fragmentSource);
		return nullptr;
	}

	NzString vertexSource = BuildVertexCode(params);
	NazaraDebug("Vertex shader source:\n" + vertexSource);
	if (!program->LoadShader(nzShaderType_Vertex, vertexSource))
	{
		NazaraError("Failed to load vertex shader: " + program->GetLog());
		NazaraNotice("Source:\n" + vertexSource);
		return nullptr;
	}

	if (!program->Compile())
	{
		NazaraError("Failed to compile program: " + program->GetLog());
		return nullptr;
	}

	return program.release();
}

bool NzShaderProgramManager::Initialize()
{
	s_glslVersion = NzOpenGL::GetGLSLVersion();
	s_earlyFragmentTest = (s_glslVersion >= 420 || NzOpenGL::IsSupported(nzOpenGLExtension_Shader_ImageLoadStore));
	s_glsl140 = (s_glslVersion >= 140);

	s_fragmentColorKW = (s_glsl140) ? "RenderTarget0" : "gl_FragColor";
	s_inKW = (s_glsl140) ? "in" : "varying";
	s_outKW = (s_glsl140) ? "out" : "varying";
	s_textureLookupKW = (s_glsl140) ? "texture" : "texture2D";

	NzShaderProgramManagerParams params;
	params.target = nzShaderTarget_None;
	for (unsigned int i = 0; i <= nzShaderFlags_Max; ++i)
	{
		params.flags = i;

		NzShaderProgram* program = GenerateProgram(params);
		if (!program)
		{
			NazaraError("Failed to generate default program (flags: 0x" + NzString::Number(i, 16) + ')');
			Uninitialize();
			return false;
		}

		s_programs[params] = program;
	}

	/*if (s_loadCachedPrograms)
	{
		NzDirectory cacheDirectory(s_cacheDirectory);
		cacheDirectory.SetPattern("*.nsb");

		if (cacheDirectory.Open())
		{
			while (cacheDirectory.NextResult(true))
			{
				long long hash;
				if (cacheDirectory.GetResultName().SubStringTo(".nsb", -1, true, false).ToInteger(&hash, 32))
				{
					std::size_t hashCode = static_cast<std::size_t>(hash);

					if (s_programs.find(hashCode) == s_programs.end())
					{
						NzFile shaderFile(cacheDirectory.GetResultPath());
						if (shaderFile.Open(NzFile::ReadOnly))
						{
							unsigned int size = cacheDirectory.GetResultSize();

							NzByteArray binary;
							binary.Resize(size);

							if (shaderFile.Read(&binary[0], size) != size)
							{
								NazaraError("Failed to read program binary");
								return false;
							}

							shaderFile.Close();

							std::unique_ptr<NzShaderProgram> program(new NzShaderProgram);
							if (program->LoadFromBinary(binary))
								s_programs[hashCode] = binary.release();
							else
								NazaraWarning("Program binary \"" + cacheDirectory.GetResultName() + "\" loading failed, this is mostly due to a driver/video card "
											  "update or a file corruption, regenerating program...");							}
					}
				}
				else
					NazaraWarning("Failed to parse program file name (" + cacheDirectory.GetResultName() + ')');
			}
		}
		else if (cacheDirectory.Exists())
			NazaraWarning("Failed to open shader cache directory");
	}*/

	return true;
}

void NzShaderProgramManager::Uninitialize()
{
	s_programs.clear();
	s_fragmentColorKW.Clear(false);
	s_inKW.Clear(false);
	s_outKW.Clear(false);
	s_textureLookupKW.Clear(false);
}

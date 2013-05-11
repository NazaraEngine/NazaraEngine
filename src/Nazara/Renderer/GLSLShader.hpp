// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLSHADER_HPP
#define NAZARA_GLSLSHADER_HPP

#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <map>

class NzResource;

class NzGLSLShader : public NzShaderImpl, NzResourceListener
{
	public:
		NzGLSLShader(NzShader* parent);
		~NzGLSLShader() = default;

		bool Bind();
		bool BindTextures();

		bool Compile();

		bool Create();
		void Destroy();

		NzString GetLog() const;
		nzShaderLanguage GetLanguage() const;
		NzString GetSourceCode(nzShaderType type) const;
		int GetUniformLocation(const NzString& name) const;
		int GetUniformLocation(nzShaderUniform uniform) const;

		bool IsLoaded(nzShaderType type) const;

		bool Load(nzShaderType type, const NzString& source);

		bool SendBoolean(int location, bool value);
		bool SendColor(int location, const NzColor& color);
		bool SendDouble(int location, double value);
		bool SendFloat(int location, float value);
		bool SendInteger(int location, int value);
		bool SendMatrix(int location, const NzMatrix4d& matrix);
		bool SendMatrix(int location, const NzMatrix4f& matrix);
		bool SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit = nullptr);
		bool SendVector(int location, const NzVector2d& vector);
		bool SendVector(int location, const NzVector2f& vector);
		bool SendVector(int location, const NzVector3d& vector);
		bool SendVector(int location, const NzVector3f& vector);
		bool SendVector(int location, const NzVector4d& vector);
		bool SendVector(int location, const NzVector4f& vector);

	private:
		void OnResourceCreated(const NzResource* resource, int index) override;
		void OnResourceDestroy(const NzResource* resource, int index) override;
		void OnResourceReleased(const NzResource* resource, int index) override;

		struct TextureSlot
		{
			bool enabled;
			bool updated = false;
			nzUInt8 unit;
			const NzTexture* texture;
		};

		mutable std::map<NzString, GLint> m_idCache; ///FIXME: unordered_map
		std::map<GLint, TextureSlot> m_textures; ///FIXME: unordered_map
		GLuint m_program;
		GLuint m_shaders[nzShaderType_Max+1];
		NzShader* m_parent;
		NzString m_log;
		int m_uniformLocations[nzShaderUniform_Max+1];
};

#endif // NAZARA_GLSLSHADER_HPPs

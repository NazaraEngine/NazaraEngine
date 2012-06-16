// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLSHADER_HPP
#define NAZARA_GLSLSHADER_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <map>

class NzGLSLShader : public NzShaderImpl
{
	public:
		NzGLSLShader(NzShader* parent);
		~NzGLSLShader();

		bool Bind();

		bool Compile();
		bool Create();

		void Destroy();

		NzString GetLog() const;
		nzShaderLanguage GetLanguage() const;
		NzString GetSourceCode(nzShaderType type) const;
		GLint GetUniformLocation(const NzString& name) const;

		bool IsLoaded(nzShaderType type) const;

		bool Load(nzShaderType type, const NzString& source);
		bool Lock();

		bool SendBoolean(const NzString& name, bool value);
		bool SendDouble(const NzString& name, double value);
		bool SendFloat(const NzString& name, float value);
		bool SendInteger(const NzString& name, int value);
		bool SendMatrix(const NzString& name, const NzMatrix4d& matrix);
		bool SendMatrix(const NzString& name, const NzMatrix4f& matrix);
		bool SendVector(const NzString& name, const NzVector2d& vector);
		bool SendVector(const NzString& name, const NzVector2f& vector);
		bool SendVector(const NzString& name, const NzVector3d& vector);
		bool SendVector(const NzString& name, const NzVector3f& vector);
		bool SendVector(const NzString& name, const NzVector4d& vector);
		bool SendVector(const NzString& name, const NzVector4f& vector);
		bool SendTexture(const NzString& name, NzTexture* texture);

		void Unbind();
		void Unlock();

	private:
		struct TextureSlot
		{
			nzUInt8 unit;
			NzTexture* texture;
		};

		mutable std::map<NzString, GLint> m_idCache;
		std::map<GLint, TextureSlot> m_textures;
		GLuint m_program;
		GLuint m_shaders[nzShaderType_Count];
		NzShader* m_parent;
		NzString m_log;
};

#endif // NAZARA_GLSLSHADER_HPPs

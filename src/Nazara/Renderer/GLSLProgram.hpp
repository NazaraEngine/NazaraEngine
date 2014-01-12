// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLPROGRAM_HPP
#define NAZARA_GLSLPROGRAM_HPP

#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/AbstractShaderProgram.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <map>
#include <unordered_map>

class NzResource;

class NzGLSLProgram : public NzAbstractShaderProgram, NzResourceListener
{
	public:
		NzGLSLProgram(NzShaderProgram* parent);
		~NzGLSLProgram() = default;

		bool Bind();
		bool BindTextures();

		bool Compile();

		bool Create();
		void Destroy();

		NzByteArray GetBinary() const;
		NzString GetLog() const;
		nzShaderLanguage GetLanguage() const;
		NzString GetSourceCode(nzShaderType type) const;
		int GetUniformLocation(const NzString& name) const;
		int GetUniformLocation(nzShaderUniform uniform) const;

		bool IsBinaryRetrievable() const;
		bool IsLoaded(nzShaderType type) const;

		bool LoadFromBinary(const void* buffer, unsigned int size);
		bool LoadShader(nzShaderType type, const NzString& source);

		bool SendBoolean(int location, bool value);
		bool SendColor(int location, const NzColor& color);
		bool SendDouble(int location, double value);
		bool SendDoubleArray(int location, const double* values, unsigned int count);
		bool SendFloat(int location, float value);
		bool SendFloatArray(int location, const float* values, unsigned int count);
		bool SendInteger(int location, int value);
		bool SendIntegerArray(int location, const int* values, unsigned int count);
		bool SendMatrix(int location, const NzMatrix4d& matrix);
		bool SendMatrix(int location, const NzMatrix4f& matrix);
		bool SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit = nullptr);
		bool SendVector(int location, const NzVector2d& vector);
		bool SendVector(int location, const NzVector2f& vector);
		bool SendVector(int location, const NzVector2i& vector);
		bool SendVector(int location, const NzVector3d& vector);
		bool SendVector(int location, const NzVector3f& vector);
		bool SendVector(int location, const NzVector3i& vector);
		bool SendVector(int location, const NzVector4d& vector);
		bool SendVector(int location, const NzVector4f& vector);
		bool SendVector(int location, const NzVector4i& vector);
		bool SendVectorArray(int location, const NzVector2d* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector2f* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector2i* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector3d* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector3f* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector3i* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector4d* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector4f* vectors, unsigned int count);
		bool SendVectorArray(int location, const NzVector4i* vectors, unsigned int count);

	private:
		bool OnResourceCreated(const NzResource* resource, int index) override;
		bool OnResourceDestroy(const NzResource* resource, int index) override;
		void OnResourceReleased(const NzResource* resource, int index) override;
		void PreLinkage();
		bool PostLinkage();

		struct TextureSlot
		{
			bool enabled;
			bool updated = false;
			nzUInt8 unit;
			const NzTexture* texture;
		};

		mutable std::unordered_map<NzString, GLint> m_idCache;
		std::map<GLint, TextureSlot> m_textures;
		GLuint m_program;
		GLuint m_shaders[nzShaderType_Max+1];
		NzShaderProgram* m_parent;
		NzString m_log;
		int m_uniformLocations[nzShaderUniform_Max+1];
};

#endif // NAZARA_GLSLPROGRAM_HPP

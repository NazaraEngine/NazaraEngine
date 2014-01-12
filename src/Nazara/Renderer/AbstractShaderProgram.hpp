// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTSHADERPROGRAM_HPP
#define NAZARA_ABSTRACTSHADERPROGRAM_HPP

#include <Nazara/Renderer/ShaderProgram.hpp>

class NzAbstractShaderProgram
{
	friend class NzRenderer;

	public:
		NzAbstractShaderProgram() = default;
		virtual ~NzAbstractShaderProgram();

		virtual bool Bind() = 0;
		virtual	bool BindTextures() = 0;

		virtual bool Compile() = 0;
		virtual bool Create() = 0;

		virtual void Destroy() = 0;

		virtual NzByteArray GetBinary() const = 0;
		virtual NzString GetLog() const = 0;
		virtual nzShaderLanguage GetLanguage() const = 0;
		virtual NzString GetSourceCode(nzShaderType type) const = 0;
		virtual int GetUniformLocation(const NzString& name) const = 0;
		virtual int GetUniformLocation(nzShaderUniform uniform) const = 0;

		virtual bool IsBinaryRetrievable() const = 0;
		virtual bool IsLoaded(nzShaderType type) const = 0;

		virtual bool LoadFromBinary(const void* buffer, unsigned int size) = 0;
		virtual bool LoadShader(nzShaderType type, const NzString& source) = 0;

		virtual bool SendBoolean(int location, bool value) = 0;
		virtual bool SendColor(int location, const NzColor& color) = 0;
		virtual bool SendDouble(int location, double value) = 0;
		virtual bool SendDoubleArray(int location, const double* values, unsigned int count) = 0;
		virtual bool SendFloat(int location, float value) = 0;
		virtual bool SendFloatArray(int location, const float* values, unsigned int count) = 0;
		virtual bool SendInteger(int location, int value) = 0;
		virtual bool SendIntegerArray(int location, const int* value, unsigned int count) = 0;
		virtual bool SendMatrix(int location, const NzMatrix4d& matrix) = 0;
		virtual bool SendMatrix(int location, const NzMatrix4f& matrix) = 0;
		virtual bool SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit = nullptr) = 0;
		virtual bool SendVector(int location, const NzVector2d& vector) = 0;
		virtual bool SendVector(int location, const NzVector2f& vector) = 0;
		virtual bool SendVector(int location, const NzVector2i& vector) = 0;
		virtual bool SendVector(int location, const NzVector3d& vector) = 0;
		virtual bool SendVector(int location, const NzVector3f& vector) = 0;
		virtual bool SendVector(int location, const NzVector3i& vector) = 0;
		virtual bool SendVector(int location, const NzVector4d& vector) = 0;
		virtual bool SendVector(int location, const NzVector4f& vector) = 0;
		virtual bool SendVector(int location, const NzVector4i& vector) = 0;
		virtual bool SendVectorArray(int location, const NzVector2d* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector2f* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector2i* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector3d* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector3f* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector3i* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector4d* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector4f* vectors, unsigned int count) = 0;
		virtual bool SendVectorArray(int location, const NzVector4i* vectors, unsigned int count) = 0;
};

#endif // NAZARA_ABSTRACTSHADERPROGRAM_HPP

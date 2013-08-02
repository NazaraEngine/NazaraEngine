// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERPROGRAM_HPP
#define NAZARA_SHADERPROGRAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Enums.hpp>

class NzShaderProgram;

using NzShaderProgramConstRef = NzResourceRef<const NzShaderProgram>;
using NzShaderProgramRef = NzResourceRef<NzShaderProgram>;

class NzAbstractShaderProgram;
class NzTexture;

class NAZARA_API NzShaderProgram : public NzResource, NzNonCopyable
{
	friend class NzRenderer;

	public:
		NzShaderProgram();
		NzShaderProgram(nzShaderLanguage language);
		NzShaderProgram(NzShaderProgram&& shader);
		~NzShaderProgram();

		bool Create(nzShaderLanguage language);
		bool Compile();

		void Destroy();

		NzByteArray GetBinary() const;
		nzUInt32 GetFlags() const;
		NzString GetLog() const;
		nzShaderLanguage GetLanguage() const;
		NzString GetSourceCode(nzShaderType type) const;
		int GetUniformLocation(const NzString& name) const;
		int GetUniformLocation(nzShaderUniform uniform) const;

		bool HasUniform(const NzString& name) const;

		bool IsBinaryRetrievable() const;
		bool IsCompiled() const;
		bool IsLoaded(nzShaderType type) const;
		bool IsValid() const;

		bool LoadShader(nzShaderType type, const NzString& source);
		bool LoadShaderFromFile(nzShaderType type, const NzString& source);

		bool SendBoolean(int location, bool value) const;
		bool SendColor(int location, const NzColor& color) const;
		bool SendDouble(int location, double value) const;
		bool SendFloat(int location, float value) const;
		bool SendInteger(int location, int value) const;
		bool SendMatrix(int location, const NzMatrix4d& matrix) const;
		bool SendMatrix(int location, const NzMatrix4f& matrix) const;
		bool SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit = nullptr) const;
		bool SendVector(int location, const NzVector2d& vector) const;
		bool SendVector(int location, const NzVector2f& vector) const;
		bool SendVector(int location, const NzVector3d& vector) const;
		bool SendVector(int location, const NzVector3f& vector) const;
		bool SendVector(int location, const NzVector4d& vector) const;
		bool SendVector(int location, const NzVector4f& vector) const;

		void SetFlags(nzUInt32 flags);

		NzShaderProgram& operator=(NzShaderProgram&& shader);

		static bool IsLanguageSupported(nzShaderLanguage language);
		static bool IsShaderTypeSupported(nzShaderType type);

	private:
		nzUInt32 m_flags;
		NzAbstractShaderProgram* m_impl;
		bool m_compiled;
};

#endif // NAZARA_SHADERPROGRAM_HPP

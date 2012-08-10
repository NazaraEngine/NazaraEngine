// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERIMPL_HPP
#define NAZARA_SHADERIMPL_HPP

#include <Nazara/Renderer/Shader.hpp>

class NzRenderer;
class NzTexture;
class NzVertexBuffer;
class NzVertexDeclaration;

class NzShaderImpl
{
	friend class NzRenderer;

	public:
		NzShaderImpl() = default;
		virtual ~NzShaderImpl();

		virtual bool Bind() = 0;
		virtual	bool BindTextures() = 0;

		virtual bool Compile() = 0;
		virtual bool Create() = 0;

		virtual void Destroy() = 0;

		virtual NzString GetLog() const = 0;
		virtual nzShaderLanguage GetLanguage() const = 0;
		virtual NzString GetSourceCode(nzShaderType type) const = 0;
		virtual int GetUniformLocation(const NzString& name) const = 0;

		virtual bool IsLoaded(nzShaderType type) const = 0;

		virtual bool Load(nzShaderType type, const NzString& source) = 0;

		virtual bool Lock() = 0;

		virtual bool SendBoolean(int location, bool value) = 0;
		virtual bool SendDouble(int location, double value) = 0;
		virtual bool SendFloat(int location, float value) = 0;
		virtual bool SendInteger(int location, int value) = 0;
		virtual bool SendMatrix(int location, const NzMatrix4d& matrix) = 0;
		virtual bool SendMatrix(int location, const NzMatrix4f& matrix) = 0;
		virtual bool SendTexture(int location, const NzTexture* texture) = 0;
		virtual bool SendVector(int location, const NzVector2d& vector) = 0;
		virtual bool SendVector(int location, const NzVector2f& vector) = 0;
		virtual bool SendVector(int location, const NzVector3d& vector) = 0;
		virtual bool SendVector(int location, const NzVector3f& vector) = 0;
		virtual bool SendVector(int location, const NzVector4d& vector) = 0;
		virtual bool SendVector(int location, const NzVector4f& vector) = 0;

		virtual void Unbind() = 0;
		virtual void Unlock() = 0;
};

#endif // NAZARA_SHADERIMPL_HPP

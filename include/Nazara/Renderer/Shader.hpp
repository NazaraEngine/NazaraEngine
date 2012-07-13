// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Resource.hpp>

class NzRenderer;
class NzShaderImpl;
class NzTexture;

class NAZARA_API NzShader : public NzResource, NzNonCopyable
{
	friend class NzRenderer;

	public:
		NzShader();
		NzShader(nzShaderLanguage language);
		~NzShader();

		bool Create(nzShaderLanguage language);
		bool Compile();

		void Destroy();

		NzString GetLog() const;
		nzShaderLanguage GetLanguage() const;
		NzString GetSourceCode(nzShaderType type) const;

		bool IsCompiled() const;
		bool IsLoaded(nzShaderType type) const;

		bool Load(nzShaderType type, const NzString& source);
		bool LoadFromFile(nzShaderType type, const NzString& source);

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

		void Unlock();

		static bool IsLanguageSupported(nzShaderLanguage language);
		static bool IsTypeSupported(nzShaderType type);

	private:
		NzShaderImpl* m_impl;
		bool m_compiled;
};

#endif // NAZARA_SHADER_HPP

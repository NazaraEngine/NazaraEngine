// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>

enum nzShaderLanguage
{
	nzShaderLanguage_Unknown,

	nzShaderLanguage_Cg,
	nzShaderLanguage_GLSL
};

enum nzShaderType
{
	nzShaderType_Fragment,
	nzShaderType_Geometry,
	nzShaderType_Vertex,

	nzShaderType_Count
};

class NzRenderer;
class NzShaderImpl;

class NAZARA_API NzShader
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

		void Unlock();

		static bool IsLanguageSupported(nzShaderLanguage language);
		static bool IsTypeSupported(nzShaderType type);

	private:
		NzShaderImpl* m_impl;
		bool m_compiled;
};

#endif // NAZARA_SHADER_HPP

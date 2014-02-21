// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERSTAGE_HPP
#define NAZARA_SHADERSTAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Enums.hpp>

class NAZARA_API NzShaderStage : NzNonCopyable
{
	public:
		NzShaderStage();
		NzShaderStage(nzShaderStage stage);
		NzShaderStage(NzShaderStage&& stage);
		~NzShaderStage();

		bool Compile();

		bool Create(nzShaderStage stage);
		void Destroy();

		NzString GetLog() const;
		NzString GetSource() const;

		bool IsCompiled() const;
		bool IsValid() const;

		void SetSource(const char* source, unsigned int length);
		void SetSource(const NzString& source);
		bool SetSourceFromFile(const NzString& filePath);

		NzShaderStage& operator=(NzShaderStage&& shader);

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;

		static bool IsSupported(nzShaderStage stage);

	private:
		nzShaderStage m_stage;
		bool m_compiled;
		unsigned int m_id;
};

#endif // NAZARA_SHADERSTAGE_HPP

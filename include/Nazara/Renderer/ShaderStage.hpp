// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERSTAGE_HPP
#define NAZARA_SHADERSTAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderStage
	{
		public:
			ShaderStage();
			ShaderStage(ShaderStageType stage);
			ShaderStage(const ShaderStage&) = delete;
			ShaderStage(ShaderStage&& stage);
			~ShaderStage();

			bool Compile();

			bool Create(ShaderStageType stage);
			void Destroy();

			String GetLog() const;
			String GetSource() const;

			bool IsCompiled() const;
			bool IsValid() const;

			void SetSource(const char* source, unsigned int length);
			void SetSource(const String& source);
			bool SetSourceFromFile(const String& filePath);

			ShaderStage& operator=(const ShaderStage&) = delete;
			ShaderStage& operator=(ShaderStage&& shader);

			// Fonctions OpenGL
			unsigned int GetOpenGLID() const;

			static bool IsSupported(ShaderStageType stage);

		private:
			ShaderStageType m_stage;
			bool m_compiled;
			unsigned int m_id;
	};
}

#endif // NAZARA_SHADERSTAGE_HPP

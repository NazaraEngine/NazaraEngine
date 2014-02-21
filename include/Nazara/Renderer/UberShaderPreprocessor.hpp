// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERPREPROCESSOR_HPP
#define NAZARA_UBERSHADERPREPROCESSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/UberShaderInstancePreprocessor.hpp>
#include <unordered_map>

class NAZARA_API NzUberShaderPreprocessor : public NzUberShader
{
	public:
		NzUberShaderPreprocessor() = default;
		~NzUberShaderPreprocessor() = default;

		NzUberShaderInstance* Get(const NzParameterList& parameters) const;

		void SetShader(nzShaderStage stage, const NzString& source, const NzString& flags);
		bool SetShaderFromFile(nzShaderStage stage, const NzString& filePath, const NzString& flags);

		static bool IsSupported();

	private:
		struct Shader
		{
			mutable std::unordered_map<nzUInt32, NzShaderStage> cache;
			std::unordered_map<NzString, nzUInt32> flags;
			NzString source;
			bool present = false;
		};

		mutable std::unordered_map<nzUInt32, NzUberShaderInstancePreprocessor> m_cache;
		std::unordered_map<NzString, nzUInt32> m_flags;
		Shader m_shaders[nzShaderStage_Max+1];
};

#endif // NAZARA_UBERSHADERPREPROCESSOR_HPP

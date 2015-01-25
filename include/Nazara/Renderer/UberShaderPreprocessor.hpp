// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERPREPROCESSOR_HPP
#define NAZARA_UBERSHADERPREPROCESSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/UberShaderInstancePreprocessor.hpp>
#include <unordered_map>

class NzUberShaderPreprocessor;

using NzUberShaderPreprocessorConstListener = NzObjectListenerWrapper<const NzUberShaderPreprocessor>;
using NzUberShaderPreprocessorConstRef = NzObjectRef<const NzUberShaderPreprocessor>;
using NzUberShaderPreprocessorListener = NzObjectListenerWrapper<NzUberShaderPreprocessor>;
using NzUberShaderPreprocessorRef = NzObjectRef<NzUberShaderPreprocessor>;

class NAZARA_API NzUberShaderPreprocessor : public NzUberShader
{
	public:
		NzUberShaderPreprocessor() = default;
		~NzUberShaderPreprocessor() = default;

		NzUberShaderInstance* Get(const NzParameterList& parameters) const;

		void SetShader(nzShaderStage stage, const NzString& source, const NzString& shaderFlags, const NzString& requiredFlags = NzString());
		bool SetShaderFromFile(nzShaderStage stage, const NzString& filePath, const NzString& shaderFlags, const NzString& requiredFlags = NzString());

		static bool IsSupported();
		template<typename... Args> static NzUberShaderPreprocessorRef New(Args&&... args);

	private:
		struct Shader
		{
			mutable std::unordered_map<nzUInt32, NzShaderStage> cache;
			std::unordered_map<NzString, nzUInt32> flags;
			nzUInt32 requiredFlags;
			NzString source;
			bool present = false;
		};

		mutable std::unordered_map<nzUInt32, NzUberShaderInstancePreprocessor> m_cache;
		std::unordered_map<NzString, nzUInt32> m_flags;
		Shader m_shaders[nzShaderStage_Max+1];
};

#include <Nazara/Renderer/UberShaderPreprocessor.inl>

#endif // NAZARA_UBERSHADERPREPROCESSOR_HPP

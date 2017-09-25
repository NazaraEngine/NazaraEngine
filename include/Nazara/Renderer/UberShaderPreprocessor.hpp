// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERPREPROCESSOR_HPP
#define NAZARA_UBERSHADERPREPROCESSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/UberShaderInstancePreprocessor.hpp>
#include <unordered_map>

namespace Nz
{
	class UberShaderInstance;
	class UberShaderPreprocessor;

	using UberShaderPreprocessorConstRef = ObjectRef<const UberShaderPreprocessor>;
	using UberShaderPreprocessorRef = ObjectRef<UberShaderPreprocessor>;

	class NAZARA_RENDERER_API UberShaderPreprocessor : public UberShader
	{
		public:
			UberShaderPreprocessor() = default;
			~UberShaderPreprocessor();

			UberShaderInstance* Get(const ParameterList& parameters) const override;

			void SetShader(ShaderStageType stage, const String& source, const String& shaderFlags, const String& requiredFlags = String());
			bool SetShaderFromFile(ShaderStageType stage, const String& filePath, const String& shaderFlags, const String& requiredFlags = String());

			static bool IsSupported();
			template<typename... Args> static UberShaderPreprocessorRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnUberShaderPreprocessorRelease, const UberShaderPreprocessor* /*uberShaderPreprocessor*/);

		private:
			struct CachedShader
			{
				mutable std::unordered_map<UInt32, ShaderStage> cache;
				std::unordered_map<String, UInt32> flags;
				UInt32 requiredFlags;
				String source;
				bool present = false;
			};

			mutable std::unordered_map<UInt32, UberShaderInstancePreprocessor> m_cache;
			std::unordered_map<String, UInt32> m_flags;
			CachedShader m_shaders[ShaderStageType_Max+1];
	};
}

#include <Nazara/Renderer/UberShaderPreprocessor.inl>

#endif // NAZARA_UBERSHADERPREPROCESSOR_HPP

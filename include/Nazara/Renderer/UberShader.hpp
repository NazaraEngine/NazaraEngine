// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADER_HPP
#define NAZARA_UBERSHADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	class ParameterList;
	class UberShader;
	class UberShaderInstance;

	using UberShaderConstRef = ObjectRef<const UberShader>;
	using UberShaderLibrary = ObjectLibrary<UberShader>;
	using UberShaderRef = ObjectRef<UberShader>;

	class NAZARA_RENDERER_API UberShader : public RefCounted
	{
		friend UberShaderLibrary;
		friend class Renderer;

		public:
			UberShader() = default;
			UberShader(const UberShader&) = delete;
			UberShader(UberShader&&) = delete;
			virtual ~UberShader();

			virtual UberShaderInstance* Get(const ParameterList& parameters) const = 0;

			UberShader& operator=(const UberShader&) = delete;
			UberShader& operator=(UberShader&&) = delete;

			// Signals:
			NazaraSignal(OnUberShaderRelease, const UberShader* /*uberShader*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			static UberShaderLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_UBERSHADER_HPP

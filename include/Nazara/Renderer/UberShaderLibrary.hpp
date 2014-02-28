// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERLIBRARY_HPP
#define NAZARA_UBERSHADERLIBRARY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <unordered_map>

class NAZARA_API NzUberShaderLibrary
{
	public:
		NzUberShaderLibrary() = delete;
		~NzUberShaderLibrary() = delete;

		static NzUberShader* Get(const NzString& name);
		static bool Has(const NzString& name);

		static bool Initialize();
		static void Register(const NzString& name, NzUberShader* uberShader);
		static void Uninitialize();
		static void Unregister(const NzString& name);

	private:
		static std::unordered_map<NzString, NzUberShaderRef> s_library;
};

#endif // NAZARA_UBERSHADERLIBRARY_HPP

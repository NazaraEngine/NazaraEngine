// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERLIBRARY_HPP
#define NAZARA_SHADERLIBRARY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <unordered_map>

class NAZARA_API NzShaderLibrary
{
	public:
		NzShaderLibrary() = delete;
		~NzShaderLibrary() = delete;

		static NzShader* Get(const NzString& name);
		static bool Has(const NzString& name);

		static bool Initialize();
		static void Register(const NzString& name, NzShader* shader);
		static void Uninitialize();
		static void Unregister(const NzString& name);

	private:
		static std::unordered_map<NzString, NzShaderRef> s_library;
};

#endif // NAZARA_SHADERLIBRARY_HPP

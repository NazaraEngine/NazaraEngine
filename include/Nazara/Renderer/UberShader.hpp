// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADER_HPP
#define NAZARA_UBERSHADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Renderer/UberShaderInstance.hpp>
#include <unordered_map>

class NzUberShader;

using NzUberShaderConstListener = NzObjectListenerWrapper<const NzUberShader>;
using NzUberShaderConstRef = NzObjectRef<const NzUberShader>;
using NzUberShaderLibrary = NzObjectLibrary<NzUberShader>;
using NzUberShaderListener = NzObjectListenerWrapper<NzUberShader>;
using NzUberShaderRef = NzObjectRef<NzUberShader>;

class NAZARA_API NzUberShader : public NzRefCounted
{
	friend NzUberShaderLibrary;
	friend class NzRenderer;

	public:
		NzUberShader() = default;
		virtual ~NzUberShader();

		virtual NzUberShaderInstance* Get(const NzParameterList& parameters) const = 0;

	private:
		static bool Initialize();
		static void Uninitialize();

		static NzUberShaderLibrary::LibraryMap s_library;
};

#endif // NAZARA_UBERSHADER_HPP

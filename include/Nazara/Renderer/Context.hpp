// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXT_HPP
#define NAZARA_CONTEXT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <memory>
#include <vector>

class NzContext;

using NzContextConstListener = NzObjectListenerWrapper<const NzContext>;
using NzContextConstRef = NzObjectRef<const NzContext>;
using NzContextLibrary = NzObjectLibrary<NzContext>;
using NzContextListener = NzObjectListenerWrapper<NzContext>;
using NzContextRef = NzObjectRef<NzContext>;

class NzContextImpl;

class NAZARA_API NzContext : public NzRefCounted
{
	friend NzContextImpl;
	friend NzContextLibrary;
	friend class NzOpenGL;

	public:
		NzContext() = default;
		~NzContext();

		bool Create(const NzContextParameters& parameters = NzContextParameters());
		void Destroy();

		const NzContextParameters& GetParameters() const;
		bool IsActive() const;
		bool SetActive(bool active) const;
		void SwapBuffers();

		static bool EnsureContext();
		static const NzContext* GetCurrent();
		static const NzContext* GetReference();
		static const NzContext* GetThreadContext();

	private:
		static bool Initialize();
		static void Uninitialize();

		NzContextParameters m_parameters;
		NzContextImpl* m_impl = nullptr;

		static std::unique_ptr<NzContext> s_reference;
		static std::vector<std::unique_ptr<NzContext>> s_contexts;
		static NzContextLibrary::LibraryMap s_library;
};

#endif // NAZARA_CONTEXT_HPP

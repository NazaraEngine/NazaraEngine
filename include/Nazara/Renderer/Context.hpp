// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXT_HPP
#define NAZARA_CONTEXT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class Context;

	using ContextConstRef = ObjectRef<const Context>;
	using ContextLibrary = ObjectLibrary<Context>;
	using ContextRef = ObjectRef<Context>;

	class ContextImpl;

	class NAZARA_RENDERER_API Context : public RefCounted
	{
		friend ContextImpl;
		friend ContextLibrary;
		friend class OpenGL;

		public:
			Context() = default;
			Context(const Context&) = delete;
			Context(Context&&) = delete;
			~Context();

			bool Create(const ContextParameters& parameters = ContextParameters());

			void Destroy();

			void EnableVerticalSync(bool enabled);

			const ContextParameters& GetParameters() const;

			bool IsActive() const;

			bool SetActive(bool active) const;
			void SwapBuffers();

			Context& operator=(const Context&) = delete;
			Context& operator=(Context&&) = delete;

			static bool EnsureContext();

			static const Context* GetCurrent();
			static const Context* GetReference();
			static const Context* GetThreadContext();

			// Signals:
			NazaraSignal(OnContextDestroy, const Context* /*context*/);
			NazaraSignal(OnContextRelease, const Context* /*context*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			ContextParameters m_parameters;
			ContextImpl* m_impl = nullptr;

			static std::unique_ptr<Context> s_reference;
			static std::vector<std::unique_ptr<Context>> s_contexts;
			static ContextLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_CONTEXT_HPP

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifdef NAZARA_RENDERER_COMMON
#error This file is not part of the common renderer interface, you must undefine NAZARA_RENDERER_COMMON to use it
#endif

#pragma once

#ifndef NAZARA_CONTEXT_HPP
#define NAZARA_CONTEXT_HPP

#include <Nazara/Renderer/ContextParameters.hpp>

class NzContextImpl;

class NAZARA_API NzContext
{
	friend NzContextImpl;

	public:
		NzContext();
		~NzContext();

		bool Create(const NzContextParameters& parameters = NzContextParameters());
		void Destroy();
		const NzContextParameters& GetParameters() const;
		bool IsActive() const;
		bool SetActive(bool active);
		void SwapBuffers();

		static bool EnsureContext();
		static NzContext* GetCurrent();
		static const NzContext* GetReference();
		static NzContext* GetThreadContext();
		static bool Initialize();
		static void Uninitialize();

	private:
		NzContextParameters m_parameters;
		NzContextImpl* m_impl;

		static NzContext* s_reference;
};

#endif // NAZARA_CONTEXT_HPP

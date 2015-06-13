// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTargetParameters.hpp>
#include <unordered_map>

class NzRenderer;

class NAZARA_RENDERER_API NzRenderTarget
{
	friend class NzRenderer;

	public:
		NzRenderTarget() = default;
		virtual ~NzRenderTarget();

		virtual unsigned int GetHeight() const = 0;
		virtual NzRenderTargetParameters GetParameters() const = 0;
		virtual unsigned int GetWidth() const = 0;

		bool IsActive() const;
		virtual bool IsRenderable() const = 0;

		bool SetActive(bool active);

		// Fonctions OpenGL
		virtual bool HasContext() const = 0;

		// Signals
		NazaraSignal(OnRenderTargetParametersChange, const NzRenderTarget*); //< Args: me
		NazaraSignal(OnRenderTargetRelease, const NzRenderTarget*); //< Args: me
		NazaraSignal(OnRenderTargetSizeChange, const NzRenderTarget*); //< Args: me

	protected:
		virtual bool Activate() const = 0;
		virtual void Desactivate() const;
		virtual void EnsureTargetUpdated() const = 0;
};

#endif // NAZARA_RENDERTARGET_HPP

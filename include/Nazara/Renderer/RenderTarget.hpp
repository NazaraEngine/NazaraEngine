// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTargetParameters.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API RenderTarget
	{
		friend class Renderer;

		public:
			RenderTarget() = default;
			RenderTarget(const RenderTarget&) = delete;
			RenderTarget(RenderTarget&&) noexcept = default;
			virtual ~RenderTarget();

			virtual unsigned int GetHeight() const = 0;
			virtual RenderTargetParameters GetParameters() const = 0;
			virtual unsigned int GetWidth() const = 0;

			bool IsActive() const;
			virtual bool IsRenderable() const = 0;

			bool SetActive(bool active);

			// Fonctions OpenGL
			virtual bool HasContext() const = 0;

			RenderTarget& operator=(const RenderTarget&) = delete;
			RenderTarget& operator=(RenderTarget&&) noexcept = default;

			// Signals:
			NazaraSignal(OnRenderTargetParametersChange, const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetRelease,	const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetSizeChange, const RenderTarget* /*renderTarget*/);

		protected:
			virtual bool Activate() const = 0;
			virtual void Desactivate() const;
			virtual void EnsureTargetUpdated() const = 0;
	};
}

#endif // NAZARA_RENDERTARGET_HPP

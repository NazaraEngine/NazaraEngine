// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTargetParameters.hpp>

class NzRenderer;

class NAZARA_API NzRenderTarget
{
	friend class NzRenderer;

	public:
		NzRenderTarget() = default;
		virtual ~NzRenderTarget();

		#ifndef NAZARA_RENDERER_COMMON
		virtual bool HasContext() const = 0;
		#endif

		virtual unsigned int GetHeight() const = 0;
		virtual NzRenderTargetParameters GetParameters() const = 0;
		virtual unsigned int GetWidth() const = 0;

		bool IsActive() const;
		virtual bool IsValid() const = 0;

		bool SetActive(bool active);

	protected:
		virtual bool Activate() = 0;
		virtual void Desactivate();
};

#endif // NAZARA_RENDERTARGET_HPP

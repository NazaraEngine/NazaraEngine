// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

		virtual bool CanActivate() const = 0;

		virtual NzRenderTargetParameters GetRenderTargetParameters() const = 0;

		#ifdef NAZARA_RENDERER_OPENGL
		virtual bool HasContext() const = 0;
		#endif

		bool IsActive() const;

		bool SetActive(bool active);

	protected:
		virtual bool Activate() = 0;
		virtual void Desactivate();
};

#endif // NAZARA_RENDERTARGET_HPP

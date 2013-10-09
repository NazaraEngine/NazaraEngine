// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTargetParameters.hpp>
#include <unordered_map>

class NzRenderer;

class NAZARA_API NzRenderTarget
{
	friend class NzRenderer;

	public:
		class Listener;

		NzRenderTarget();
		virtual ~NzRenderTarget();

		void AddListener(Listener* listener, void* userdata = nullptr) const;

		virtual unsigned int GetHeight() const = 0;
		virtual NzRenderTargetParameters GetParameters() const = 0;
		virtual unsigned int GetWidth() const = 0;

		bool IsActive() const;
		virtual bool IsRenderable() const = 0;

		void RemoveListener(Listener* listener) const;

		bool SetActive(bool active);

		// Fonctions OpenGL
		virtual bool HasContext() const = 0;

		class NAZARA_API Listener
		{
			public:
				Listener() = default;
				~Listener();

				virtual bool OnRenderTargetParametersChange(const NzRenderTarget* renderTarget, void* userdata);
				virtual void OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata);
				virtual bool OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata);
		};

	protected:
		virtual bool Activate() const = 0;
		virtual void Desactivate() const;
		virtual void EnsureTargetUpdated() const = 0;

		void NotifyParametersChange();
		void NotifySizeChange();

	private:
		mutable std::unordered_map<Listener*, void*> m_listeners;
		        bool m_listenersLocked;
};

#endif // NAZARA_RENDERTARGET_HPP

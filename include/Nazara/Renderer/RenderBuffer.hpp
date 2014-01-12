// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERBUFFER_HPP
#define NAZARA_RENDERBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzRenderBuffer;

using NzRenderBufferConstRef = NzResourceRef<const NzRenderBuffer>;
using NzRenderBufferRef = NzResourceRef<NzRenderBuffer>;

class NAZARA_API NzRenderBuffer : public NzResource, NzNonCopyable
{
	public:
		NzRenderBuffer();
		~NzRenderBuffer();

		bool Create(nzPixelFormat format, unsigned int width, unsigned int height);
		void Destroy();

		unsigned int GetHeight() const;
		nzPixelFormat GetFormat() const;
		unsigned int GetWidth() const;

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;

		bool IsValid() const;

		static bool IsSupported();

	private:
		nzPixelFormat m_pixelFormat;
		unsigned int m_height;
		unsigned int m_id;
		unsigned int m_width;
};

#endif // NAZARA_RENDERBUFFER_HPP

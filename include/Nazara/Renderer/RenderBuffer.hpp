// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERBUFFER_HPP
#define NAZARA_RENDERBUFFER_HPP

#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API RenderBuffer : public AbstractBuffer
	{
		public:
			RenderBuffer() = default;
			~RenderBuffer() = default;

			virtual bool Fill(const void* data, UInt32 offset, UInt32 size) = 0;

			bool Initialize(UInt32 size, BufferUsageFlags usage) override;

			DataStorage GetStorage() const override;

			virtual void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) = 0;
			virtual bool Unmap() = 0;

		private:
			SoftwareBuffer m_softwareBuffer;
	};

}

#include <Nazara/Renderer/RenderBuffer.inl>

#endif // NAZARA_RENDERBUFFER_HPP

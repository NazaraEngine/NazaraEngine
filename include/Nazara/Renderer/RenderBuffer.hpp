// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERBUFFER_HPP
#define NAZARA_RENDERER_RENDERBUFFER_HPP

#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	class RenderDevice;

	class NAZARA_RENDERER_API RenderBuffer : public AbstractBuffer
	{
		public:
			inline RenderBuffer(Buffer* parent, BufferType type);
			RenderBuffer(const RenderBuffer&) = delete;
			RenderBuffer(RenderBuffer&&) = default;
			~RenderBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size) final;

			bool Initialize(UInt64 size, BufferUsageFlags usage) override;

			AbstractBuffer* GetHardwareBuffer(RenderDevice* device);
			UInt64 GetSize() const override;
			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt64 offset = 0, UInt64 size = 0) final;
			bool Unmap() final;

			RenderBuffer& operator=(const RenderBuffer&) = delete;
			RenderBuffer& operator=(RenderBuffer&&) = default;

		public: //< temp
			bool Synchronize(RenderDevice* device);

		private:
			struct HardwareBuffer;

			HardwareBuffer* GetHardwareBufferData(RenderDevice* device);

			struct HardwareBuffer
			{
				std::shared_ptr<AbstractBuffer> buffer;
				bool synchronized = false;
			};

			BufferUsageFlags m_usage;
			SoftwareBuffer m_softwareBuffer;
			Buffer* m_parent;
			BufferType m_type;
			std::size_t m_size;
			std::unordered_map<RenderDevice*, HardwareBuffer> m_hardwareBuffers;
	};
}

#include <Nazara/Renderer/RenderBuffer.inl>

#endif // NAZARA_RENDERER_RENDERBUFFER_HPP

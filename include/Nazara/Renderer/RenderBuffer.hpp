// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERBUFFER_HPP
#define NAZARA_RENDERBUFFER_HPP

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

			bool Fill(const void* data, UInt32 offset, UInt32 size) override final;

			bool Initialize(UInt32 size, BufferUsageFlags usage) override;

			AbstractBuffer* GetHardwareBuffer(RenderDevice* device);
			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) override final;
			bool Unmap() override final;

			RenderBuffer& operator=(const RenderBuffer&) = delete;
			RenderBuffer& operator=(RenderBuffer&&) = default;

		public: //< temp
			bool Synchronize(RenderDevice* device);

		private:
			struct HardwareBuffer
			{
				std::unique_ptr<AbstractBuffer> buffer;
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

#endif // NAZARA_RENDERBUFFER_HPP

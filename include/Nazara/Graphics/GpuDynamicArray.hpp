// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GPUDYNAMICARRAY_HPP
#define NAZARA_GRAPHICS_GPUDYNAMICARRAY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Nz
{
	class RenderBuffer;
	class RenderDevice;

	class NAZARA_GRAPHICS_API GpuDynamicArray : public TransferInterface
	{
		public:
			GpuDynamicArray(RenderDevice& renderDevice, UInt32 entrySize, UInt32 initialCapacity = 512, UInt32 headerSize = 0, BufferUsageFlags bufferUsageFlags = BufferUsage::DeviceLocal | BufferUsage::StorageBuffer);
			GpuDynamicArray(const GpuDynamicArray&) = delete;
			GpuDynamicArray(GpuDynamicArray&&) = delete;
			~GpuDynamicArray() = default;

			UInt8* AccessEntry(UInt32 entryIndex);
			UInt8* AccessHeader();

			inline const std::shared_ptr<RenderBuffer>& GetBuffer() const;
			inline UInt32 GetCapacity() const;
			inline UInt32 GetEntrySize() const;
			inline UInt32 GetHeaderSize() const;
			inline UInt32 GetSize() const;

			UInt32 Push();

			void Pop();

			void UpdateDebugName(std::string debugName);

			GpuDynamicArray& operator=(const GpuDynamicArray&) = delete;
			GpuDynamicArray& operator=(GpuDynamicArray&&) = delete;

			NazaraSignal(OnBufferInvalidated, GpuDynamicArray* /*emitter*/);

		private:
			inline UInt64 ComputeBufferSize(UInt32 entryCount);
			void GrowBuffer();
			inline void ResetInvalidationRanges();
			void OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder) override;

			struct InvalidatedRange
			{
				UInt64 start = 0;
				UInt64 end = 0;
			};

			std::shared_ptr<RenderBuffer> m_gpuBuffer;
			std::string m_debugName;
			std::vector<UInt8> m_memory;
			BufferUsageFlags m_bufferUsageFlags;
			InvalidatedRange m_invalidatedRange;
			UInt32 m_capacity;
			UInt32 m_entrySize;
			UInt32 m_headerSize;
			UInt32 m_size;
			RenderDevice& m_renderDevice;
	};
}

#include <Nazara/Graphics/GpuDynamicArray.inl>

#endif // NAZARA_GRAPHICS_GPUDYNAMICARRAY_HPP

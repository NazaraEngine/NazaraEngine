// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline AbstractBuffer* Buffer::GetImpl() const
	{
		return m_impl.get();
	}

	inline UInt32 Buffer::GetSize() const
	{
		return m_size;
	}

	inline DataStorage Buffer::GetStorage() const
	{
		return m_impl->GetStorage();
	}

	inline BufferType Buffer::GetType() const
	{
		return m_type;
	}

	inline BufferUsageFlags Buffer::GetUsage() const
	{
		return m_usage;
	}

	inline bool Buffer::HasStorage(DataStorage storage) const
	{
		return GetStorage() == storage;
	}

	inline bool Buffer::IsValid() const
	{
		return m_impl != nullptr;
	}

	template<typename... Args>
	BufferRef Buffer::New(Args&&... args)
	{
		std::unique_ptr<Buffer> object(new Buffer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>

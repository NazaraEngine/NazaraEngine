// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Icon::Icon() :
	m_impl(nullptr)
	{
	}

	inline Icon::Icon(const Image& icon)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(icon);
	}

	Icon::~Icon()
	{
		Destroy();
	}

	bool Icon::IsValid() const
	{
		return m_impl != nullptr;
	}

	template<typename... Args>
	IconRef Icon::New(Args&&... args)
	{
		std::unique_ptr<Icon> object(new Icon(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASECOMPONENT_HPP
#define NDK_BASECOMPONENT_HPP

#include <NDK/Prerequesites.hpp>

namespace Ndk
{
	class NDK_API BaseComponent
	{
		public:
			BaseComponent(nzUInt32 componentId);
			virtual ~BaseComponent();

			virtual BaseComponent* Clone() const = 0;

			nzUInt32 GetId() const;

			static nzUInt32 GetNextId();

		protected:
			nzUInt32 m_componentId;

		private:
			static nzUInt32 s_nextId;
	};
}

#include <NDK/BaseComponent.inl>

#endif // NDK_BASECOMPONENT_HPP

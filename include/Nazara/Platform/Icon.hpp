// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICON_HPP
#define NAZARA_ICON_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Platform/Config.hpp>

namespace Nz
{
	class Image;
	class IconImpl;

	class Icon;

	using IconRef = ObjectRef<Icon>;

	class NAZARA_PLATFORM_API Icon : public RefCounted
	{
		friend class WindowImpl;

		public:
			inline Icon();
			inline explicit Icon(const Image& icon);
			inline ~Icon();

			bool Create(const Image& icon);
			void Destroy();

			inline bool IsValid() const;

			template<typename... Args> static IconRef New(Args&&... args);

		private:
			IconImpl* m_impl;
	};
}

#include <Nazara/Platform/Icon.inl>

#endif // NAZARA_ICON_HPP

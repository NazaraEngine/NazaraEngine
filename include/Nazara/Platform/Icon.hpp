// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICON_HPP
#define NAZARA_ICON_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>
#include <memory>

namespace Nz
{
	class Image;
	class IconImpl;

	class NAZARA_PLATFORM_API Icon
	{
		friend class WindowImpl;

		public:
			Icon();
			explicit Icon(const Image& icon);
			Icon(const Icon&) = delete;
			Icon(Icon&&) noexcept;
			~Icon();

			bool Create(const Image& icon);
			void Destroy();

			inline bool IsValid() const;

			Icon& operator=(const Icon&) = delete;
			Icon& operator=(Icon&&) noexcept;

		private:
			std::unique_ptr<IconImpl> m_impl;
	};
}

#include <Nazara/Platform/Icon.inl>

#endif // NAZARA_ICON_HPP

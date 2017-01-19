// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICON_HPP
#define NAZARA_ICON_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class Image;
	class IconImpl;

	class Icon;

	using IconRef = ObjectRef<Icon>;

	class NAZARA_UTILITY_API Icon : public RefCounted
	{
		friend class WindowImpl;

		public:
			inline Icon();
			inline explicit Icon(const Image& icon);
			inline ~Icon();

			bool Create(const Image& icon);
			void Destroy();

			inline bool IsValid() const;

		private:
			IconImpl* m_impl;
	};
}

#include <Nazara/Utility/Icon.inl>

#endif // NAZARA_ICON_HPP

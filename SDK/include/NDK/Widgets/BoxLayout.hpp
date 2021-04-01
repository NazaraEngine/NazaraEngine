// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_BOXLAYOUT_HPP
#define NDK_WIDGETS_BOXLAYOUT_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/Widgets/Enums.hpp>
#include <memory>

namespace Ndk
{
	class NDK_CLIENT_API BoxLayout : public BaseWidget
	{
		public:
			BoxLayout(BaseWidget* parent, BoxLayoutOrientation orientation);
			BoxLayout(const BoxLayout&) = delete;
			BoxLayout(BoxLayout&&) = delete;
			~BoxLayout();

			void Layout() override;

			BoxLayout& operator=(const BoxLayout&) = delete;
			BoxLayout& operator=(BoxLayout&&) = delete;

		private:
			struct State;

			std::unique_ptr<State> m_state;
			BoxLayoutOrientation m_orientation;
			float m_spacing;
	};
}

#include <NDK/Widgets/BoxLayout.inl>

#endif // NDK_WIDGETS_BOXLAYOUT_HPP

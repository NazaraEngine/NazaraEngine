// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_BOXLAYOUT_HPP
#define NDK_WIDGETS_BOXLAYOUT_HPP

#include <NazaraSDK/Prerequisites.hpp>
#include <NazaraSDK/BaseWidget.hpp>
#include <NazaraSDK/Widgets/Enums.hpp>
#include <vector>

namespace Ndk
{
	class NDK_API BoxLayout : public BaseWidget
	{
		public:
			inline BoxLayout(BaseWidget* parent, BoxLayoutOrientation orientation);
			BoxLayout(const BoxLayout&) = delete;
			BoxLayout(BoxLayout&&) = default;
			~BoxLayout() = default;

			void Layout() override;

			BoxLayout& operator=(const BoxLayout&) = delete;
			BoxLayout& operator=(BoxLayout&&) = default;

		private:
			struct ChildInfo
			{
				BaseWidget* widget;
				bool isConstrained;
				float maximumSize;
				float minimumSize;
				float size;
			};

			std::vector<ChildInfo> m_childInfos;
			BoxLayoutOrientation m_orientation;
			float m_spacing;
	};
}

#include <NazaraSDK/Widgets/BoxLayout.inl>

#endif // NDK_WIDGETS_BOXLAYOUT_HPP

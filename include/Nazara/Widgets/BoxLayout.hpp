// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_BOXLAYOUT_HPP
#define NAZARA_WIDGETS_BOXLAYOUT_HPP

#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/Enums.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_WIDGETS_API BoxLayout : public BaseWidget
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

#include <Nazara/Widgets/BoxLayout.inl>

#endif // NAZARA_WIDGETS_BOXLAYOUT_HPP

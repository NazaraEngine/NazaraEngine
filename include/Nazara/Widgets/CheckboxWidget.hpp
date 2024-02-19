// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_CHECKBOXWIDGET_HPP
#define NAZARA_WIDGETS_CHECKBOXWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/Enums.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API CheckboxWidget : public BaseWidget
	{
		public:
			CheckboxWidget(BaseWidget* parent);
			CheckboxWidget(const CheckboxWidget&) = delete;
			CheckboxWidget(CheckboxWidget&&) = delete;
			~CheckboxWidget() = default;

			inline void EnableTristate(bool enabled);

			inline CheckboxState GetState() const;
			inline bool IsTristateEnabled() const;

			inline void SetState(bool checkboxState);
			void SetState(CheckboxState checkboxState);

			inline void SwitchToNextState();

			CheckboxWidget& operator=(const CheckboxWidget&) = delete;
			CheckboxWidget& operator=(CheckboxWidget&&) = delete;

			NazaraSignal(OnCheckboxStateUpdate, const CheckboxWidget* /*button*/, CheckboxState /*newState*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			bool OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			bool OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			void OnMouseExit() override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			std::unique_ptr<CheckboxWidgetStyle> m_style;
			CheckboxState m_state;
			bool m_isTristateEnabled;
	};
}

#include <Nazara/Widgets/CheckboxWidget.inl>

#endif // NAZARA_WIDGETS_CHECKBOXWIDGET_HPP

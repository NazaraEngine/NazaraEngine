// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_CHECKBOXWIDGET_HPP
#define NDK_WIDGETS_CHECKBOXWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/Enums.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Rect.hpp>

namespace Ndk
{
	class World;

	class NDK_API CheckboxWidget : public BaseWidget
	{
		public:
			CheckboxWidget(BaseWidget* parent = nullptr);
			CheckboxWidget(const CheckboxWidget&) = delete;
			CheckboxWidget(CheckboxWidget&&) = default;
			~CheckboxWidget() = default;

			//virtual CheckboxWidget* Clone() const = 0;

			void ResizeToContent() override;
			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);

			inline void SetTextMargin(float margin);
			inline float GetTextMargin() const;
			inline void EnableAdaptativeMargin(bool enable = true);
			inline bool IsMarginAdaptative() const;

			inline void SetCheckboxSize(const Nz::Vector2f& size);
			inline const Nz::Vector2f& GetCheckboxSize() const;
			inline Nz::Vector2f GetCheckboxBorderSize() const;

			inline void EnableCheckbox(bool enable = true);
			inline bool IsCheckboxEnabled() const;
			inline void EnableTristate(bool enable = true);
			inline bool IsTristateEnabled() const;

			inline CheckboxState GetState() const;
			void SetState(CheckboxState state);
			CheckboxState SetNextState();

			CheckboxWidget& operator=(const CheckboxWidget&) = delete;
			CheckboxWidget& operator=(CheckboxWidget&&) = default;

			NazaraSignal(OnStateChanged, const CheckboxWidget* /*checkbox*/);

		private:
			void Layout() override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			inline bool ContainsCheckbox(int x, int y) const;

			void UpdateCheckboxSprite();
			void CreateCheckboxTextures();


			EntityHandle m_checkboxEntity;
			EntityHandle m_checkboxContentEntity;
			EntityHandle m_textEntity;

			Nz::TextureRef m_checkbox;
			Nz::TextureRef m_disabledCheckbox;

			Nz::TextureRef m_checkboxContentChecked;
			Nz::TextureRef m_checkboxContentTristate;

			Nz::SpriteRef m_checkboxContentSprite;
			Nz::SpriteRef m_checkboxSprite;
			Nz::TextSpriteRef m_textSprite;

			Nz::Color m_backgroundColor;
			Nz::Color m_disabledBackgroundColor;
			Nz::Color m_disabledMainColor;
			Nz::Color m_mainColor;

			bool m_adaptativeMargin;
			bool m_checkboxEnabled;
			bool m_tristateEnabled;

			float m_borderScale;
			float m_textMargin;
			CheckboxState m_state;
	};
}

#include <NDK/Widgets/CheckboxWidget.inl>

#endif // NDK_WIDGETS_CHECKBOXWIDGET_HPP

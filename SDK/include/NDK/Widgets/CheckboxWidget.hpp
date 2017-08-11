// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_CHECKBOXWIDGET_HPP
#define NDK_WIDGETS_CHECKBOXWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Ndk
{
	class World;

	enum CheckboxState
	{
		CheckboxState_Unchecked,
		CheckboxState_Checked,
		CheckboxState_Tristate,

		CheckboxState_Max = CheckboxState_Tristate
	};

	class NDK_API CheckboxWidget : public BaseWidget /// TODO: enlever possibilité de modifier la bordure, faire checkbox images au compiletime
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

			CheckboxState m_state;
			bool m_tristateEnabled;
			bool m_checkboxEnabled;

			bool m_adaptativeMargin;
			float m_textMargin;
			float m_borderScale;

			Nz::Color m_mainColor;
			Nz::Color m_backgroundColor;
			Nz::Color m_disabledMainColor;
			Nz::Color m_disabledBackgroundColor;

			Nz::TextureRef m_checkbox;
			Nz::TextureRef m_disabledCheckbox;

			EntityHandle m_checkboxEntity;
			EntityHandle m_checkboxTextEntity;
			EntityHandle m_textEntity;

			Nz::SpriteRef m_checkboxSprite;
			Nz::TextSpriteRef m_checkboxTextSprite;
			Nz::TextSpriteRef m_textSprite;
	};
}

#include <NDK/Widgets/CheckboxWidget.inl>

#endif // NDK_WIDGETS_CHECKBOXWIDGET_HPP

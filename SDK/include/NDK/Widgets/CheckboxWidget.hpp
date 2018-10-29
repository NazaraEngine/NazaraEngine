// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_CHECKBOXWIDGET_HPP
#define NDK_WIDGETS_CHECKBOXWIDGET_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Widgets/Enums.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	class AbstractTextDrawer;
}

namespace Ndk
{
	class NDK_API CheckboxWidget : public BaseWidget
	{
		friend class Sdk;

		public:
			CheckboxWidget(BaseWidget* parent);
			CheckboxWidget(const CheckboxWidget&) = delete;
			CheckboxWidget(CheckboxWidget&&) = default;
			~CheckboxWidget() = default;

			//virtual CheckboxWidget* Clone() const = 0;

			inline void EnableAdaptativeMargin(bool enable = true);
			inline void EnableCheckbox(bool enable = true);
			inline void EnableTristate(bool enable = true);

			inline bool IsCheckboxEnabled() const;
			inline bool IsMarginAdaptative() const;
			inline bool IsTristateEnabled() const;

			inline const Nz::Vector2f& GetCheckboxSize() const;
			inline Nz::Vector2f GetCheckboxBorderSize() const;
			inline CheckboxState GetState() const;
			inline float GetTextMargin() const;

			inline void SetCheckboxSize(const Nz::Vector2f& size);
			CheckboxState SwitchToNextState();
			void SetState(CheckboxState state);
			inline void SetTextMargin(float margin);

			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);


			CheckboxWidget& operator=(const CheckboxWidget&) = delete;
			CheckboxWidget& operator=(CheckboxWidget&&) = default;

			NazaraSignal(OnStateChanged, const CheckboxWidget* /*checkbox*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			void Layout() override;
			void UpdateCheckbox();
			void UpdateSize();

			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			inline bool ContainsCheckbox(int x, int y) const;


			EntityHandle m_checkboxBorderEntity;
			EntityHandle m_checkboxBackgroundEntity;
			EntityHandle m_checkboxContentEntity;
			EntityHandle m_textEntity;

			Nz::TextureRef m_checkMark;

			Nz::SpriteRef m_checkboxContentSprite;
			Nz::SpriteRef m_checkboxBorderSprite;
			Nz::SpriteRef m_checkboxBackgroundSprite;
			Nz::TextSpriteRef m_textSprite;

			static Nz::Color s_backgroundColor;
			static Nz::Color s_disabledBackgroundColor;
			static Nz::Color s_disabledBorderColor;
			static Nz::Color s_borderColor;

			bool m_adaptativeMargin;
			bool m_checkboxEnabled;
			bool m_tristateEnabled;

			static float s_borderScale;
			float m_textMargin;
			CheckboxState m_state;
	};
}

#include <NDK/Widgets/CheckboxWidget.inl>

#endif // NDK_WIDGETS_CHECKBOXWIDGET_HPP

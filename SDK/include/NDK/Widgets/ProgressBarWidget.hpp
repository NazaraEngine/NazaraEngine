// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_PROGRESSBARWIDGET_HPP
#define NDK_WIDGETS_PROGRESSBARWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Ndk
{
	class World;

	class NDK_API ProgressBarWidget : public BaseWidget
	{
		friend class Sdk;

		public:
			ProgressBarWidget(BaseWidget* parent = nullptr);
			ProgressBarWidget(const ProgressBarWidget&) = delete;
			ProgressBarWidget(ProgressBarWidget&&) = default;
			~ProgressBarWidget() = default;

			//virtual ProgressBarWidget* Clone() const = 0;

			inline void EnableText(bool enable = true);
			inline bool IsTextEnabled() const;

			inline unsigned GetPercentageValue() const;
			inline Nz::Vector2f GetProgressBarSize() const;
			inline Nz::Vector2f GetProgressBarBorderSize() const;
			inline float GetTextMargin() const;

			inline void SetPercentageValue(unsigned percentage);
			inline void SetTextMargin(float margin);
			inline void SetTextColor(const Nz::Color& color);

			inline void ResizeToContent() override {}

			NazaraSignal(OnValueChanged, const ProgressBarWidget* /*progressBar*/);

		private:
			void Layout() override;
			inline void UpdateText();


			EntityHandle m_borderEntity;
			EntityHandle m_barEntity;
			EntityHandle m_textEntity;

			static Nz::Color s_borderColor;
			static Nz::Color s_backgroundColor;
			static Nz::Color s_backgroundCornerColor;
			static Nz::Color s_barColor;
			static Nz::Color s_barCornerColor;
			Nz::Color m_textColor;

			Nz::SpriteRef m_borderSprite;
			Nz::SpriteRef m_backgroundSprite;
			Nz::SpriteRef m_barSprite;
			Nz::TextSpriteRef m_textSprite;

			static float s_borderScale;
			float m_textMargin;
			unsigned m_value;
	};
}

#include <NDK/Widgets/ProgressBarWidget.inl>

#endif // NDK_WIDGETS_PROGRESSBARWIDGET_HPP

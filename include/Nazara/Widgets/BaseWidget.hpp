// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_BASEWIDGET_HPP
#define NAZARA_WIDGETS_BASEWIDGET_HPP

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Platform/Event.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Utility/Node.hpp>
#include <Nazara/Widgets/Config.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <limits>
#include <optional>

namespace Nz
{
	class Canvas;

	class NAZARA_WIDGETS_API BaseWidget : public Nz::Node
	{
		friend Canvas;

		public:
			BaseWidget(BaseWidget* parent);
			BaseWidget(const BaseWidget&) = delete;
			BaseWidget(BaseWidget&&) = delete;
			virtual ~BaseWidget();

			template<typename T, typename... Args> T* Add(Args&&... args);
			inline void AddChild(std::unique_ptr<BaseWidget>&& widget);

			inline void Center();
			inline void CenterHorizontal();
			inline void CenterVertical();

			void ClearFocus();
			inline void ClearRenderingRect();

			void Destroy();

			void EnableBackground(bool enable);

			template<typename F> void ForEachWidgetChild(F iterator);
			template<typename F> void ForEachWidgetChild(F iterator) const;

			//virtual BaseWidget* Clone() const = 0;

			inline const Color& GetBackgroundColor() const;
			inline Canvas* GetCanvas();
			inline Nz::SystemCursor GetCursor() const;
			inline float GetHeight() const;

			inline float GetMaximumHeight() const;
			inline Nz::Vector2f GetMaximumSize() const;
			inline float GetMaximumWidth() const;

			inline float GetMinimumHeight() const;
			inline Nz::Vector2f GetMinimumSize() const;
			inline float GetMinimumWidth() const;

			inline float GetPreferredHeight() const;
			inline Nz::Vector2f GetPreferredSize() const;
			inline float GetPreferredWidth() const;

			inline const Nz::Rectf& GetRenderingRect() const;

			inline Nz::Vector2f GetSize() const;
			inline float GetWidth() const;
			inline std::size_t GetWidgetChildCount() const;

			bool HasFocus() const;

			inline void Hide();
			inline bool IsVisible() const;

			void Resize(const Nz::Vector2f& size);

			void SetBackgroundColor(const Color& color);
			void SetCursor(Nz::SystemCursor systemCursor);
			void SetFocus();
			void SetParent(BaseWidget* widget);

			inline void SetFixedHeight(float fixedHeight);
			inline void SetFixedSize(const Nz::Vector2f& fixedSize);
			inline void SetFixedWidth(float fixedWidth);

			inline void SetMaximumHeight(float maximumHeight);
			inline void SetMaximumSize(const Nz::Vector2f& maximumSize);
			inline void SetMaximumWidth(float maximumWidth);

			inline void SetMinimumHeight(float minimumHeight);
			inline void SetMinimumSize(const Nz::Vector2f& minimumSize);
			inline void SetMinimumWidth(float minimumWidth);

			virtual void SetRenderingRect(const Nz::Rectf& renderingRect);

			void Show(bool show = true);

			BaseWidget& operator=(const BaseWidget&) = delete;
			BaseWidget& operator=(BaseWidget&&) = delete;

		protected:
			entt::entity CreateEntity();
			void DestroyEntity(entt::entity entity);
			virtual void Layout();

			void InvalidateNode() override;

			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;
			Nz::Rectf GetScissorRect() const;

			virtual bool IsFocusable() const;
			virtual void OnFocusLost();
			virtual void OnFocusReceived();
			virtual bool OnKeyPressed(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnMouseEnter();
			virtual void OnMouseMoved(int x, int y, int deltaX, int deltaY);
			virtual void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseWheelMoved(int x, int y, float delta);
			virtual void OnMouseExit();
			virtual void OnParentResized(const Nz::Vector2f& newSize);
			virtual void OnTextEntered(char32_t character, bool repeated);
			virtual void OnTextEdited(const std::array<char, 32>& characters, int length);

			inline void SetPreferredSize(const Nz::Vector2f& preferredSize);

			virtual void ShowChildren(bool show);

		private:
			inline BaseWidget();

			void DestroyChild(BaseWidget* widget);
			void DestroyChildren();
			inline bool IsRegisteredToCanvas() const;
			inline void NotifyParentResized(const Nz::Vector2f& newSize);
			void RegisterToCanvas();
			inline void UpdateCanvasIndex(std::size_t index);
			void UnregisterFromCanvas();
			void UpdatePositionAndSize();

			struct WidgetEntity
			{
				entt::entity handle;
				bool isEnabled = true;

				//NazaraSlot(Ndk::Entity, OnEntityDisabled, onDisabledSlot);
				//NazaraSlot(Ndk::Entity, OnEntityEnabled, onEnabledSlot);
			};

			static constexpr std::size_t InvalidCanvasIndex = std::numeric_limits<std::size_t>::max();

			std::optional<entt::entity> m_backgroundEntity;
			std::size_t m_canvasIndex;
			std::shared_ptr<Sprite> m_backgroundSprite;
			std::vector<WidgetEntity> m_entities;
			std::vector<std::unique_ptr<BaseWidget>> m_children;
			entt::registry* m_registry;
			Canvas* m_canvas;
			Color m_backgroundColor;
			Rectf m_renderingRect;
			SystemCursor m_cursor;
			Vector2f m_maximumSize;
			Vector2f m_minimumSize;
			Vector2f m_preferredSize;
			Vector2f m_size;
			BaseWidget* m_widgetParent;
			bool m_visible;
	};
}

#include <Nazara/Widgets/BaseWidget.inl>

#endif // NAZARA_WIDGETS_BASEWIDGET_HPP

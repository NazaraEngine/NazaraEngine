// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_BASEWIDGET_HPP
#define NDK_BASEWIDGET_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityOwner.hpp>
#include <NDK/World.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Platform/Event.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Utility/Node.hpp>
#include <limits>

namespace Ndk
{
	class Canvas;

	class NDK_API BaseWidget : public Nz::Node
	{
		friend Canvas;

		public:
			struct Padding;

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

			void Destroy();

			void EnableBackground(bool enable);

			template<typename F> void ForEachWidgetChild(F iterator);
			template<typename F> void ForEachWidgetChild(F iterator) const;

			//virtual BaseWidget* Clone() const = 0;

			inline const Nz::Color& GetBackgroundColor() const;
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

			inline Nz::Vector2f GetSize() const;
			inline float GetWidth() const;
			inline std::size_t GetWidgetChildCount() const;

			bool HasFocus() const;

			inline bool IsVisible() const;

			void Resize(const Nz::Vector2f& size);

			void SetBackgroundColor(const Nz::Color& color);
			void SetCursor(Nz::SystemCursor systemCursor);
			void SetFocus();

			inline void SetFixedHeight(float fixedHeight);
			inline void SetFixedSize(const Nz::Vector2f& fixedSize);
			inline void SetFixedWidth(float fixedWidth);

			inline void SetMaximumHeight(float maximumHeight);
			inline void SetMaximumSize(const Nz::Vector2f& maximumSize);
			inline void SetMaximumWidth(float maximumWidth);

			inline void SetMinimumHeight(float minimumHeight);
			inline void SetMinimumSize(const Nz::Vector2f& minimumSize);
			inline void SetMinimumWidth(float minimumWidth);

			void Show(bool show = true);

			BaseWidget& operator=(const BaseWidget&) = delete;
			BaseWidget& operator=(BaseWidget&&) = delete;

		protected:
			const EntityHandle& CreateEntity();
			void DestroyEntity(Entity* entity);
			virtual void Layout();

			void InvalidateNode() override;

			virtual bool IsFocusable() const;
			virtual void OnFocusLost();
			virtual void OnFocusReceived();
			virtual bool OnKeyPressed(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnMouseEnter();
			virtual void OnMouseMoved(int x, int y, int deltaX, int deltaY);
			virtual void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseExit();
			virtual void OnParentResized(const Nz::Vector2f& newSize);
			virtual void OnTextEntered(char32_t character, bool repeated);

			inline void SetPreferredSize(const Nz::Vector2f& preferredSize);

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
				EntityOwner handle;
			};

			static constexpr std::size_t InvalidCanvasIndex = std::numeric_limits<std::size_t>::max();

			std::size_t m_canvasIndex;
			std::vector<WidgetEntity> m_entities;
			std::vector<std::unique_ptr<BaseWidget>> m_children;
			Canvas* m_canvas;
			EntityOwner m_backgroundEntity;
			WorldHandle m_world;
			Nz::Color m_backgroundColor;
			Nz::SpriteRef m_backgroundSprite;
			Nz::SystemCursor m_cursor;
			Nz::Vector2f m_maximumSize;
			Nz::Vector2f m_minimumSize;
			Nz::Vector2f m_preferredSize;
			Nz::Vector2f m_size;
			BaseWidget* m_widgetParent;
			bool m_visible;
	};
}

#include <NDK/BaseWidget.inl>

#endif // NDK_BASEWIDGET_HPP

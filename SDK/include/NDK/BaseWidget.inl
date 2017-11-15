// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseWidget.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>

namespace Ndk
{
	inline BaseWidget::BaseWidget() :
	m_canvasIndex(InvalidCanvasIndex),
	m_canvas(nullptr),
	m_backgroundColor(Nz::Color(230, 230, 230, 255)),
	m_cursor(Nz::SystemCursor_Default),
	m_contentSize(50.f, 50.f),
	m_widgetParent(nullptr),
	m_visible(true)
	{
		SetPadding(5.f, 5.f, 5.f, 5.f);
	}

	template<typename T, typename... Args>
	inline T* BaseWidget::Add(Args&&... args)
	{
		std::unique_ptr<T> widget = std::make_unique<T>(this, std::forward<Args>(args)...);
		T* widgetPtr = widget.get();
		AddChild(std::move(widget));

		return widgetPtr;
	}

	inline void BaseWidget::AddChild(std::unique_ptr<BaseWidget>&& widget)
	{
		widget->Show(m_visible);
		widget->SetParent(this);
		m_children.emplace_back(std::move(widget));
	}

	inline void BaseWidget::Center()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition((parentSize.x - mySize.x) / 2.f, (parentSize.y - mySize.y) / 2.f);
	}

	inline void BaseWidget::CenterHorizontal()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition((parentSize.x - mySize.x) / 2.f, GetPosition(Nz::CoordSys_Local).y);
	}

	inline void BaseWidget::CenterVertical()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition(GetPosition(Nz::CoordSys_Local).x, (parentSize.y - mySize.y) / 2.f);
	}

	/*!
	* \brief Destroy the widget, deleting it in the process.
	*
	* Calling this function immediately destroys the widget, freeing its memory.
	*/
	inline void BaseWidget::Destroy()
	{
		NazaraAssert(this != m_canvas, "Canvas cannot be destroyed by calling Destroy()");

		m_widgetParent->DestroyChild(this); //< This does delete us
	}

	inline const Nz::Color& BaseWidget::GetBackgroundColor() const
	{
		return m_backgroundColor;
	}

	inline Canvas* BaseWidget::GetCanvas()
	{
		return m_canvas;
	}

	inline Nz::SystemCursor BaseWidget::GetCursor() const
	{
		return m_cursor;
	}

	inline const BaseWidget::Padding& BaseWidget::GetPadding() const
	{
		return m_padding;
	}

	inline Nz::Vector2f BaseWidget::GetContentOrigin() const
	{
		return { m_padding.left, m_padding.top };
	}

	inline const Nz::Vector2f& BaseWidget::GetContentSize() const
	{
		return m_contentSize;
	}

	inline Nz::Vector2f BaseWidget::GetSize() const
	{
		return Nz::Vector2f(m_contentSize.x + m_padding.left + m_padding.right, m_contentSize.y + m_padding.top + m_padding.bottom);
	}

	inline bool BaseWidget::IsVisible() const
	{
		return m_visible;
	}

	inline void BaseWidget::SetContentSize(const Nz::Vector2f& size)
	{
		NotifyParentResized(size);
		m_contentSize = size;

		Layout();
	}

	inline void BaseWidget::SetPadding(float left, float top, float right, float bottom)
	{
		m_padding.left = left;
		m_padding.top = top;
		m_padding.bottom = bottom;
		m_padding.right = right;

		Layout();
	}

	inline bool BaseWidget::IsRegisteredToCanvas() const
	{
		return m_canvas && m_canvasIndex != InvalidCanvasIndex;
	}

	inline void BaseWidget::NotifyParentResized(const Nz::Vector2f& newSize)
	{
		for (const auto& widgetPtr : m_children)
			widgetPtr->OnParentResized(newSize);
	}

	inline void BaseWidget::UpdateCanvasIndex(std::size_t index)
	{
		m_canvasIndex = index;
	}
}

// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	template<typename Widget, typename Style>
	WidgetStyleFactory<Widget, Style>::WidgetStyleFactory(std::nullptr_t) :
	m_builder(nullptr)
	{
	}

	template<typename Widget, typename Style>
	WidgetStyleFactory<Widget, Style>::WidgetStyleFactory(FunctionRef<std::unique_ptr<Style>(Widget* widget)>&& builder) :
	m_builder(std::move(builder))
	{
	}

	template<typename Widget, typename Style>
	template<typename T, typename... Args>
	WidgetStyleFactory<Widget, Style>::WidgetStyleFactory(Nz::TypeTag<T>, Args&&... args) :
	m_builder(nullptr)
	{
		m_builder = [&](Widget* widget) -> std::unique_ptr<Style>
		{
			return std::make_unique<T>(widget, std::forward<Args>(args)...);
		};
	}

	template<typename Widget, typename Style>
	std::unique_ptr<Style> WidgetStyleFactory<Widget, Style>::operator()(Widget* widget) const
	{
		return m_builder(widget);
	}

	template<typename Widget, typename Style>
	WidgetStyleFactory<Widget, Style>::operator bool() const
	{
		return static_cast<bool>(m_builder);
	}
}

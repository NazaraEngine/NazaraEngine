// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_WIDGETSTYLEFACTORY_HPP
#define NAZARA_WIDGETS_WIDGETSTYLEFACTORY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Widgets/Export.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/TypeTag.hpp>

namespace Nz
{
	class BaseWidget;

	template<typename Widget, typename Style>
	class WidgetStyleFactory
	{
		public:
			WidgetStyleFactory(std::nullptr_t);
			WidgetStyleFactory(FunctionRef<std::unique_ptr<Style>(Widget* widget)>&& builder);
			template<typename T, typename... Args> WidgetStyleFactory(Nz::TypeTag<T>, Args&&... args);
			WidgetStyleFactory(const WidgetStyleFactory&) = delete;
			WidgetStyleFactory(WidgetStyleFactory&&) = delete;
			~WidgetStyleFactory() = default;

			std::unique_ptr<Style> operator()(Widget* widget) const;

			explicit operator bool() const;

			WidgetStyleFactory& operator=(const WidgetStyleFactory&) = delete;
			WidgetStyleFactory& operator=(WidgetStyleFactory&&) = delete;

		private:
			FunctionRef<std::unique_ptr<Style>(Widget* widget)> m_builder;
	};
}

#include <Nazara/Widgets/WidgetStyleFactory.inl>

#endif // NAZARA_WIDGETS_WIDGETSTYLEFACTORY_HPP

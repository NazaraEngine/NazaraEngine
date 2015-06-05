// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIGNAL_HPP
#define NAZARA_SIGNAL_HPP

#include <functional>
#include <vector>

template<typename... Args>
class NzSignal
{
	public:
		using Callback = std::function<void(Args...)>;

		NzSignal() = default;
		~NzSignal() = default;

		void Connect(const Callback& func);
		void Connect(Callback&& func);
		template<typename O> void Connect(O& object, void (O::*method)(Args...));
		template<typename O> void Connect(O* object, void (O::*method)(Args...));

		void operator()(Args&&... args);

	private:
		std::vector<Callback> m_callbacks;
};

#include <Nazara/Core/Signal.inl>

#endif // NAZARA_SIGNAL_HPP

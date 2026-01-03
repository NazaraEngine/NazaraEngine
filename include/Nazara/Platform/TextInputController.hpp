// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_TEXTINPUTCONTROLLER_HPP
#define NAZARA_PLATFORM_TEXTINPUTCONTROLLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class TextInputController;

	using TextInputControllerHandle = ObjectHandle<TextInputController>;

	class TextInputController : public HandledObject<TextInputController>
	{
		public:
			TextInputController() = default;
			TextInputController(const TextInputController&) = delete;
			TextInputController(TextInputController&&) noexcept = default;
			~TextInputController() = default;

			inline void StartTextInput();
			inline void StopTextInput();

			TextInputController& operator=(const TextInputController&) = delete;
			TextInputController& operator=(TextInputController&&) noexcept = default;

			NazaraSignal(OnTextInputStarted, const TextInputController* /*textInputController*/);
			NazaraSignal(OnTextInputStopped, const TextInputController* /*textInputController*/);
	};
}

#include <Nazara/Platform/TextInputController.inl>

#endif // NAZARA_PLATFORM_TEXTINPUTCONTROLLER_HPP

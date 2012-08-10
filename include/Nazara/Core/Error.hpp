// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERROR_HPP
#define NAZARA_ERROR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if (!(a)) NzError(nzErrorType_AssertFailed, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#else
	#define NazaraAssert(a, err)
#endif

#define NazaraError(err) NzError(nzErrorType_Normal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraInternalError(err) NzError(nzErrorType_Internal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraWarning(err) NzError(nzErrorType_Warning, err, __LINE__, __FILE__, NAZARA_FUNCTION)

NAZARA_API void NzError(nzErrorType type, const NzString& error, unsigned int line, const char* file, const char* function);
NAZARA_API unsigned int NzGetLastSystemErrorCode();
NAZARA_API NzString NzGetLastSystemError(unsigned int code = NzGetLastSystemErrorCode());

#endif // NAZARA_ERROR_HPP

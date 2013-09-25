// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERROR_HPP
#define NAZARA_ERROR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if (!(a)) NzError::Error(nzErrorType_AssertFailed, err, __LINE__, NzDirectory::GetCurrentFileRelativeToEngine(__FILE__), NAZARA_FUNCTION)
#else
	#define NazaraAssert(a, err)
#endif

#define NazaraError(err) NzError::Error(nzErrorType_Normal, err, __LINE__, NzDirectory::GetCurrentFileRelativeToEngine(__FILE__), NAZARA_FUNCTION)
#define NazaraInternalError(err) NzError::Error(nzErrorType_Internal, err, __LINE__, NzDirectory::GetCurrentFileRelativeToEngine(__FILE__), NAZARA_FUNCTION)
#define NazaraWarning(err) NzError::Error(nzErrorType_Warning, err, __LINE__, NzDirectory::GetCurrentFileRelativeToEngine(__FILE__), NAZARA_FUNCTION)

class NAZARA_API NzError
{
	public:
		NzError() = delete;
		~NzError() = delete;

		static void Error(nzErrorType type, const NzString& error);
		static void Error(nzErrorType type, const NzString& error, unsigned int line, const char* file, const char* function);

		static nzUInt32 GetFlags();
		static NzString GetLastError(const char** file = nullptr, unsigned int* line = nullptr, const char** function = nullptr);
		static unsigned int GetLastSystemErrorCode();
		static NzString GetLastSystemError(unsigned int code = GetLastSystemErrorCode());

		static void SetFlags(nzUInt32 flags);

	private:
		static nzUInt32 s_flags;
		static NzString s_lastError;
		static const char* s_lastErrorFunction;
		static const char* s_lastErrorFile;
		static unsigned int s_lastErrorLine;
};

#endif // NAZARA_ERROR_HPP

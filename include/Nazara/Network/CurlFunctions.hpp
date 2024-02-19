// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// no header guards

#if !defined(NAZARA_CURL_FUNCTION)
#error You must define NAZARA_CURL_FUNCTION before including this file
#endif

#ifndef NAZARA_CURL_FUNCTION_LAST
#define NAZARA_CURL_FUNCTION_LAST(F) NAZARA_CURL_FUNCTION(F)
#endif

NAZARA_CURL_FUNCTION(easy_cleanup)
NAZARA_CURL_FUNCTION(easy_getinfo)
NAZARA_CURL_FUNCTION(easy_init)
NAZARA_CURL_FUNCTION(easy_setopt)
NAZARA_CURL_FUNCTION(easy_strerror)
NAZARA_CURL_FUNCTION(global_cleanup)
NAZARA_CURL_FUNCTION(global_init)
NAZARA_CURL_FUNCTION(multi_add_handle)
NAZARA_CURL_FUNCTION(multi_cleanup)
NAZARA_CURL_FUNCTION(multi_info_read)
NAZARA_CURL_FUNCTION(multi_init)
NAZARA_CURL_FUNCTION(multi_perform)
NAZARA_CURL_FUNCTION(multi_remove_handle)
NAZARA_CURL_FUNCTION(multi_strerror)
NAZARA_CURL_FUNCTION(slist_append)
NAZARA_CURL_FUNCTION(slist_free_all)
NAZARA_CURL_FUNCTION_LAST(version_info)

#undef NAZARA_CURL_FUNCTION
#undef NAZARA_CURL_FUNCTION_LAST

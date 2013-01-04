// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Functor.hpp>
#include <process.h>
#include <Nazara/Core/Debug.hpp>

NzThreadImpl::NzThreadImpl(NzFunctor* functor)
{
    int error = pthread_create(&m_handle, nullptr, &NzThreadImpl::ThreadProc, functor);
	
	if (error != 0)
		NazaraInternalError("Failed to create thread: " + NzGetLastSystemError());
}

void NzThreadImpl::Detach()
{
	// http://stackoverflow.com/questions/418742/is-it-reasonable-to-call-closehandle-on-a-thread-before-it-terminates
	pthread_detach(&m_handle);
}

void NzThreadImpl::Join()
{
	pthread_join(&m_handle, nullptr);
}

unsigned int __stdcall NzThreadImpl::ThreadProc(void* userdata)
{
	NzFunctor* func = static_cast<NzFunctor*>(userdata);
	func->Run();
	delete func;

	/*
	En C++, il vaut mieux retourner depuis la fonction que de quitter le thread explicitement
	Source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms682659(v=vs.85).aspx
	*/

	return 0;
}

void NzThreadImpl::Sleep(nzUInt32 time)
{
    // code from SFML2 Unix SleepImpl.cpp source https://github.com/LaurentGomila/SFML/blob/master/src/SFML/System/Unix/SleepImpl.cpp

    // usleep is not reliable enough (it might block the
    // whole process instead of just the current thread)
    // so we must use pthread_cond_timedwait instead

    // this implementation is inspired from Qt

    nzUint64 usecs = time.asMicroseconds();

    // get the current time
    timeval tv;
    gettimeofday(&tv, NULL);

    // construct the time limit (current time + time to wait)
    timespec ti;
    ti.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    ti.tv_sec = tv.tv_sec + (usecs / 1000000) + (ti.tv_nsec / 1000000000);
    ti.tv_nsec %= 1000000000;

    // create a mutex and thread condition
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, 0);
    pthread_cond_t condition;
    pthread_cond_init(&condition, 0);

    // wait...
    pthread_mutex_lock(&mutex);
    pthread_cond_timedwait(&condition, &mutex, &ti);
    pthread_mutex_unlock(&mutex);

    // destroy the mutex and condition
    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&mutex);
}

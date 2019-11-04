//
// Created by ByteFlow on 2019/1/23.
//

#ifndef BYTEFLOW_MYLOCK_H
#define BYTEFLOW_MYLOCK_H

#include <pthread.h>

class SyncLock
{
public:
	SyncLock(bool bRecursive = true)
	{

		pthread_mutexattr_init(&m_attr);
		if (bRecursive)
		{
			pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
		}
		else
		{
			pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_DEFAULT);
		}
		pthread_mutex_init(&m_mutex, &m_attr);

	}

	~SyncLock(void)
	{

		pthread_mutex_destroy(&m_mutex);
		pthread_mutexattr_destroy(&m_attr);
	}

	int Lock()
	{

		return pthread_mutex_lock(&m_mutex);

	}

	int UnLock()
	{

		return pthread_mutex_unlock(&m_mutex);


	}

	int TryLock()
	{

		return pthread_mutex_trylock(&m_mutex);
	}

private:
	pthread_mutex_t m_mutex;
	pthread_mutexattr_t m_attr;

};

class ByteFlowLock
{
public:
	ByteFlowLock(SyncLock *pLock) :
			m_pLock(pLock)
	{
		if (pLock != NULL)
		{
			m_pLock->Lock();
		}
	}

	~ByteFlowLock()
	{
		if (m_pLock != NULL)
		{
			m_pLock->UnLock();
		}

	}

private:
	SyncLock *m_pLock;
};

#endif //BYTEFLOW_MYLOCK_H

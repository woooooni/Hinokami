#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"
#include "ThreadManager.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	DeadLockProfiler::GetInstance()->PushLock(name);
#endif
	ThreadManager::MTLS& tTLS = ThreadManager::GetInstance()->Get_TLS(this_thread::get_id());
	

	// ������ �����尡 �����ϰ� �ִٸ� ������ ����.
	const unsigned __int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (tTLS.LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// �ƹ��� ���� �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�.
	const __int64 beginTick = ::GetTickCount64();
	const unsigned __int32 desired = ((tTLS.LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			unsigned __int32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	DeadLockProfiler::GetInstance()->PopLock(name);
#endif
	ThreadManager::MTLS& tTLS = ThreadManager::GetInstance()->Get_TLS(this_thread::get_id());

	int i = tTLS.LThreadId;

	if (tTLS.LThreadId == 0)
		int i = 0;

	// ReadLock �� Ǯ�� ������ WriteUnlock �Ұ���.
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const __int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	DeadLockProfiler::GetInstance()->PushLock(name);
#endif
	ThreadManager::MTLS& tTLS = ThreadManager::GetInstance()->Get_TLS(this_thread::get_id());

	// ������ �����尡 �����ϰ� �ִٸ� ������ ����.
	const unsigned __int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (tTLS.LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// �ƹ��� �����ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ�� �ø���.
	const __int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			unsigned __int32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	DeadLockProfiler::GetInstance()->PopLock(name);
#endif

	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}

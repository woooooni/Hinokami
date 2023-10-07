#pragma once
#include "Base.h"
#include <stack>
#include <map>
#include <vector>
#include <unordered_map>
#include <set>
#include <mutex>

/*--------------------
	DeadLockProfiler
---------------------*/

BEGIN(Engine)

class ENGINE_DLL DeadLockProfiler : public CBase
{
	DECLARE_SINGLETON(DeadLockProfiler)
private:
	DeadLockProfiler();
	~DeadLockProfiler() = default;

public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>	_nameToId;
	unordered_map<int32, const char*>	_idToName;
	map<int32, set<int32>>				_lockHistory;

	mutex _lock;

private:
	vector<int32>	_discoveredOrder; // ��尡 �߰ߵ� ������ ����ϴ� �迭
	int32			_discoveredCount = 0; // ��尡 �߰ߵ� ����
	vector<bool>	_finished; // Dfs(i)�� ���� �Ǿ����� ����
	vector<int32>	_parent;
};

END
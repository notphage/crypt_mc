#pragma once
#include <string>
class c_shared_mutex
{
public:
	enum class mode
	{
		server,
		client
	};
	
	c_shared_mutex(const std::string& name, mode type)
	{
		//Create mutex if it doesn't exist, join if it does
		m_mutex = CreateMutexA(nullptr, (type == mode::server) ? FALSE : TRUE, name.c_str());

		m_init = m_mutex != nullptr;
	}
	
	~c_shared_mutex()
	{
		if (!m_init)
			return;

		CloseHandle(m_mutex);
	}
	
	bool lock() const
	{
		if (!m_init)
			return false;

		//Blocks until it has ownership
		const DWORD res = WaitForSingleObject(m_mutex, 3000);
		if (res == WAIT_OBJECT_0)
			return true;

		return false;
	}
	
	bool unlock() const
	{
		if (!m_init)
			return false;

		if (ReleaseMutex(m_mutex) != NULL)
			return true;

		return false;
	}
	
private:
	HANDLE m_mutex;
	bool m_init;
};

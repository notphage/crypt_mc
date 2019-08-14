#pragma once

class c_shared_signal
{
	HANDLE m_event;
	bool m_init = false;
	
public:
	explicit c_shared_signal(const std::string& name)
	{
		m_init = true;

		m_event = CreateEventA(nullptr, TRUE, FALSE, name.c_str());
		if (m_event == nullptr)
			m_init = false;
	}
	
	~c_shared_signal()
	{
		CloseHandle(m_event);
	}

	bool signal() const
	{
		if (!m_init)
			return false;

		return SetEvent(m_event) == NULL ? false : true;
	}

	bool wait() const
	{
		if (!m_init)
			return false;

		if (WaitForSingleObject(m_event, 10000) == WAIT_OBJECT_0)
			return true;
		
		return false;
	}
	
	bool reset() const
	{
		return ResetEvent(m_event) == NULL ? false : true;
	}
};

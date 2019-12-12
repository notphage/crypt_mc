#pragma once

#include "shared_signal.h"
#include "shared_mutex.h"

struct mem_message_t
{
	mem_message_t()
		: m_size(0)
	{}
	
	mem_message_t(uint8_t* data, size_t size)
	{
		m_size = size;
		m_data.resize(size);
		memcpy(&m_data[0], data, m_size);
	}
	
	//Vector data is written into shared Mem
	std::vector<uint8_t> m_data;
	size_t m_size;
};

struct shared_mem_header_t
{
	uint32_t m_offset_to_last_message;
	uint32_t m_message_count;
};

class c_shared_mem_queue
{
	std::atomic_bool m_should_close = false;
	mutable c_shared_mutex m_mutex;
	shared_mem_header_t* m_out_header = nullptr;
	shared_mem_header_t* m_in_header = nullptr;
	uint32_t m_buf_size;
	uint8_t* m_buffer;
	HANDLE m_mapped_file = nullptr;
	std::thread m_message_thread;
	c_shared_signal m_server_to_client_signal;
	c_shared_signal m_client_to_server_signal;
	std::function<void()> m_received_callback;
	
public:
	enum class mode
	{
		server,
		client
	};

	mode m_type;
	bool m_init;
	
	c_shared_mem_queue(const std::string& server_name, const uint32_t buf_size, mode type)
		: m_mutex(std::string(server_name + "_MTX"), (type == mode::server) ? c_shared_mutex::mode::server : c_shared_mutex::mode::client),
		m_server_to_client_signal(std::string(server_name + "_SC_SGNL")), m_client_to_server_signal(std::string(server_name + "_CS_SGNL"))
	{
		m_init = true;
		m_buf_size = buf_size;
		m_type = type;

		if (m_type == mode::server)
			m_mapped_file = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, m_buf_size, server_name.c_str());
		else if (m_type == mode::client)
			m_mapped_file = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, server_name.c_str());

		if (m_mapped_file == nullptr)
		{
			m_init = false;
			return;
		}

		m_buffer = static_cast<uint8_t*>(MapViewOfFile(m_mapped_file, FILE_MAP_ALL_ACCESS, 0, 0, m_buf_size));
		if (m_buffer == nullptr)
		{
			CloseHandle(m_mapped_file);
			m_init = false;
			return;
		}

		if (m_type == mode::server)
		{
			m_out_header = reinterpret_cast<shared_mem_header_t*>(m_buffer);
			m_out_header->m_message_count = 0;
			m_out_header->m_offset_to_last_message = 0;

			m_in_header = reinterpret_cast<shared_mem_header_t*>(m_buffer + (m_buf_size / 2));
			m_in_header->m_message_count = 0;
			m_in_header->m_offset_to_last_message = 0;
		}

		if (m_type == mode::client)
		{
			m_in_header = reinterpret_cast<shared_mem_header_t*>(m_buffer);
			m_out_header = reinterpret_cast<shared_mem_header_t*>(m_buffer + (m_buf_size / 2));
		}
		
		m_message_thread = std::thread(&c_shared_mem_queue::msg_thread, this);
	}
	
	~c_shared_mem_queue()
	{
		m_should_close = true;
		UnmapViewOfFile(m_buffer);
		CloseHandle(m_mapped_file);
		m_message_thread.join();
	}

	void msg_thread() const
	{
		do
		{
			this->wait_for_message();

		} while (!m_should_close);
	}

	void set_callback(const std::function<void()>& callback)
	{
		m_received_callback = callback;
	}

	//For batch operations
	void manual_lock() const
	{
		m_mutex.lock();
	}
	
	void manual_unlock() const
	{
		m_mutex.unlock();
	}

	//Call with true to use manual lock features
	bool push_message(mem_message_t msg, bool manual_lock = false) const
	{
		if (!m_init)
			return false;
		
		if (!manual_lock)
			std::lock_guard<c_shared_mutex> lock_guard(m_mutex);

		auto write_loc = reinterpret_cast<uint8_t*>(m_out_header) + sizeof(shared_mem_header_t) + m_out_header->m_offset_to_last_message;

		//sizeof(uint32_t) = sizeof(MemMessage::m_size), Qt doesn't like that syntax
		//Make sure we don't overrun our buffer
		const uintptr_t delta = write_loc + msg.m_size + sizeof(uint32_t) - reinterpret_cast<uint8_t*>(m_out_header);
		if (delta >= m_buf_size / 2)
			return false;

		//Write Data
		memcpy(write_loc, &msg.m_data[0], msg.m_size);
		write_loc += msg.m_size;

		//Write the message size
		*reinterpret_cast<uint32_t*>(write_loc) = msg.m_size;

		m_out_header->m_offset_to_last_message += msg.m_size + sizeof(uint32_t);
		m_out_header->m_message_count++;

		return m_type == mode::server ? m_server_to_client_signal.signal() : m_client_to_server_signal.signal();
	}
	
	bool pop_message(mem_message_t& msg) const
	{
		if (!m_init)
			return false;

		std::lock_guard<c_shared_mutex> Lock(m_mutex);
		if (m_in_header->m_message_count < 1)
			return false;

		uint8_t* read_loc = reinterpret_cast<uint8_t*>(m_in_header) + sizeof(shared_mem_header_t) + m_in_header->m_offset_to_last_message - sizeof(uint32_t);

		//Get size of message data
		const uint32_t msg_size = *reinterpret_cast<uint32_t*>(read_loc);
		read_loc -= msg_size;

		//Make room in the vector for our data
		msg.m_data.resize(msg_size);

		//Read the data
		memcpy(&msg.m_data[0], read_loc, msg_size);

		//Set the size of the message in the struct
		msg.m_size = msg_size;

		m_in_header->m_offset_to_last_message -= msg_size + sizeof(uint32_t);
		m_in_header->m_message_count--;
		return true;
	}
	
	bool is_message_available() const
	{
		if (!m_init)
			return false;

		std::lock_guard<c_shared_mutex> lock_guard(m_mutex);
		if (m_in_header->m_message_count > 0)
			return true;

		return false;
	}
	
	bool wait_for_message() const
	{
		if (m_type == mode::server)
		{
			if (!m_client_to_server_signal.wait())
				return false;
			
			if (!m_client_to_server_signal.reset())
				return false;
		}
		else
		{
			if (!m_server_to_client_signal.wait())
				return false;
			
			if (!m_server_to_client_signal.reset())
				return false;
		}
		
		if (m_received_callback)
			m_received_callback();

		return true;
	}
	
	uint32_t get_out_message_count() const
	{
		if (!m_init)
			return 0;

		std::lock_guard<c_shared_mutex> lock_guard(m_mutex);
		return m_out_header->m_message_count;
	}
	
	uint32_t get_in_message_count() const
	{
		if (!m_init)
			return 0;

		std::lock_guard<c_shared_mutex> lock_guard(m_mutex);
		return m_in_header->m_message_count;
	}
};

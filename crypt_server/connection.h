#pragma once

#include <sys/socket.h>

class c_connection
{
	int m_conn_socket = -1;
	std::string m_ip;
	std::vector<uint8_t> m_buffer;

	int send_impl(const void* data, size_t size);
	int recieve_impl(void* data, size_t size);
public:
	c_connection(int conn_socket, const std::string& ip)
		: m_conn_socket(conn_socket), m_ip(ip)
	{ }

	void disconnect();
	void force_disconnect();
	int send();
	int recieve();

	inline void set_buffer(const std::vector<uint8_t>& buffer)
	{
		m_buffer.assign(buffer.begin(), buffer.end());
	}

	template <std::size_t N>
	inline void set_buffer(uint8_t(&buffer)[N])
	{
		m_buffer.resize(N);
		memcpy(&m_buffer[0], &buffer[0], N * sizeof(uint8_t));
	}

	inline void set_buffer(const void* data, size_t size)
	{
		m_buffer.resize(size);
		memcpy(&m_buffer[0], data, size * sizeof(uint8_t));
	}

	inline void add_buffer(const std::vector<uint8_t>& buffer)
	{
		m_buffer.insert(m_buffer.end(), buffer.begin(), buffer.end());
	}

	template <std::size_t N>
	inline void add_buffer(uint8_t(&buffer)[N])
	{
		m_buffer.resize(m_buffer.size() + N);
		memcpy(&m_buffer[m_buffer.size() - N], &buffer[0], N * sizeof(uint8_t));
	}

	inline void clear_buffer()
	{
		m_buffer.clear();
	}

	inline const std::vector<uint8_t>& get_buffer()
	{
		return m_buffer;
	}

	inline size_t buffer_size()
	{
		return m_buffer.size();
	}

	inline void* buffer_data()
	{
		return m_buffer.data();
	}
};
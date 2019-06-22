#include "context.h"
#include <iostream>

int c_connection::send_impl(const void* data, size_t size)
{
	const char* data_ptr = (const char*)data;
	size_t bytes_sent = 0;

	while (size > 0)
	{
		bytes_sent = ::send(m_conn_socket, data_ptr, size, 0);
		if (bytes_sent == 0)
			return -1;

		data_ptr += bytes_sent;
		size -= bytes_sent;
	}

	return 1;
}

int c_connection::recieve_impl(void* data, size_t size)
{
	char* data_ptr = (char*)data;
	size_t bytes_recv = 0;

	while (size > 0)
	{
		bytes_recv = ::recv(m_conn_socket, data_ptr, size, 0);
		if (bytes_recv <= 0)
			return (int)bytes_recv;

		data_ptr += bytes_recv;
		size -= bytes_recv;
	}

	return 1;
}

int c_connection::send()
{
	size_t size = m_buffer.size();
	int result = 0;

	if ((result = send_impl(&size, sizeof size)) == 1)
		if ((result = send_impl(m_buffer.data(), size)) != 1)
			m_buffer.clear();

	return result;
}

int c_connection::recieve()
{
	m_buffer.clear();

	size_t size = 0;
	int result = 0;

	if ((result = recieve_impl(&size, sizeof size)) == 1)
	{
		if (size > 0)
		{
			m_buffer.resize(size);

			if ((result = recieve_impl(m_buffer.data(), size)) != 1)
				m_buffer.clear();
		}
	}

	return result;
}

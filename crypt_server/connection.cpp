#include "context.h"
#include <iostream>
#include <unistd.h>

int c_connection::send_impl(const void* data, size_t size)
{
	const char* data_ptr = (const char*)data;
	int32_t bytes_sent = 0;

	while (size > 0)
	{
		if ((bytes_sent = ::send(m_conn_socket, data_ptr, size, 0)) == -1)
			return -1;

		data_ptr += bytes_sent;
		size -= bytes_sent;
	}

	return 1;
}

int c_connection::recieve_impl(void* data, size_t size)
{
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(m_conn_socket, &read_fds);

	timeval time_out;
	time_out.tv_sec = 60;
	time_out.tv_usec = 0; //30 Microseconds for Polling

	auto ret = 0;
	if ((ret = select(m_conn_socket + 1, &read_fds, nullptr, nullptr, &time_out)) <= 0)
		return -1;

	char* data_ptr = (char*)data;
	int32_t bytes_recv = 0;

	while (size > 0)
	{
		if ((bytes_recv = ::recv(m_conn_socket, data_ptr, size, 0)) <= 0)
			return -1;

		data_ptr += bytes_recv;
		size -= bytes_recv;
	}

	return 1;
}

void c_connection::disconnect()
{
	if (shutdown(m_conn_socket, SHUT_RDWR) < 0)
	{
		ctx.m_console.write(std::string("Client ") + m_ip + std::string(" errored on disconnect: ") + strerror(errno));
		force_disconnect();

		return;
	}

	ctx.m_console.write(std::string("Client ") + m_ip + std::string(" was disconnected gracefully"));
	close(m_conn_socket);
}

void c_connection::force_disconnect()
{
	close(m_conn_socket);
	ctx.m_console.write(std::string("Client ") + m_ip + std::string(" was disconnected forcefully"));
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

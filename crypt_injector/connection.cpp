#include "context.h"
#include <iostream>

int c_connection::send_impl(const void* data, size_t size)
{
	const char* data_ptr = (const char*)data;
	int32_t bytes_sent = 0;

	while (size > 0)
	{
		if ((bytes_sent = ::send(m_conn_socket, data_ptr, size, 0)) == SOCKET_ERROR)
			return SOCKET_ERROR;

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
	time_out.tv_sec = 5;
	time_out.tv_usec = 0; //30 Microseconds for Polling

	auto ret = 0;
	if ((ret = select(m_conn_socket + 1, &read_fds, nullptr, nullptr, &time_out)) <= 0)
		return -1;

	char* data_ptr = (char*)data;
	int32_t bytes_recv = 0;

	while (size > 0)
	{
		if ((bytes_recv = ::recv(m_conn_socket, data_ptr, size, 0)) <= 0)
			return SOCKET_ERROR;

		data_ptr += bytes_recv;
		size -= bytes_recv;
	}

	return 1;
}

bool c_connection::connect()
{
	auto retval = 0;
	addrinfo* results = nullptr, *addr = nullptr, hints;

	if ((retval = WSAStartup(MAKEWORD(2, 2), &m_wsa_data)))
	{
		printf("> WSAStartup failed: %i\n", retval);
		goto cleanup;
	}

	memset(&hints, 0, sizeof addrinfo);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ((retval = getaddrinfo("127.0.0.1", "8123", &hints, &results)))
	{
		printf("> getaddrinfo failed: %i\n", retval);
		goto cleanup;
	}

	if (!results)
	{
		printf("> localhost could not be resolved.\n");
		goto cleanup;
	}

	addr = results;
	while (addr)
	{
		if ((m_conn_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET)
		{
			printf("> socket failed: %i\n", WSAGetLastError());
			goto cleanup;
		}

		if ((retval = ::connect(m_conn_socket, addr->ai_addr, (socklen_t)addr->ai_addrlen)) == SOCKET_ERROR)
		{
			retval = closesocket(m_conn_socket);
			if (retval == SOCKET_ERROR)
				printf("> closesocket failed: %i\n", WSAGetLastError());

			m_conn_socket = INVALID_SOCKET;

			addr = addr->ai_next;
		}
		else
			break;
	}

	freeaddrinfo(results);
	results = nullptr;

	if (m_conn_socket == INVALID_SOCKET)
	{
		printf("> Unable to establish a connection...\n");
		goto cleanup;
	}

	printf("> Connection established...\n");

	return true;

cleanup:

	if (m_conn_socket != INVALID_SOCKET)
	{
		// No more data to send
		retval = shutdown(m_conn_socket, SD_BOTH);
		if (retval == SOCKET_ERROR)
			printf("> Shutdown failed: %i\n", WSAGetLastError());

		retval = closesocket(m_conn_socket);
		if (retval == SOCKET_ERROR)
			printf("> closesocket failed: %i\n", WSAGetLastError());

		m_conn_socket = INVALID_SOCKET;
	}

	if (results != nullptr)
	{
		freeaddrinfo(results);
		results = nullptr;
	}

	WSACleanup();
	return false;
}

void c_connection::disconnect()
{
	auto retval = 0;

	if (m_conn_socket != INVALID_SOCKET)
	{
		// No more data to send
		retval = shutdown(m_conn_socket, SD_BOTH);
		if (retval == SOCKET_ERROR)
			printf("> Shutdown failed: %i\n", WSAGetLastError());

		retval = closesocket(m_conn_socket);
		if (retval == SOCKET_ERROR)
			printf("> closesocket failed: %i\n", WSAGetLastError());

		m_conn_socket = INVALID_SOCKET;
	}

	WSACleanup();
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

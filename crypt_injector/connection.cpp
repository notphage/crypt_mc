#include "context.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

int c_connection::send_impl(const void* data, size_t size) const
{
	DWORD tv = 5000;
	setsockopt(m_conn_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof(DWORD));

	const char* data_ptr = static_cast<const char*>(data);
	int32_t bytes_sent = 0;

	while (size > 0)
	{
		if ((bytes_sent = SSL_write(static_cast<SSL*>(m_client_ssl), data_ptr, size)) <= 0)
			return SOCKET_ERROR;

		data_ptr += bytes_sent;
		size -= bytes_sent;
	}

	return 1;
}

int c_connection::receive_impl(void* data, size_t size) const
{
	DWORD tv = 5000;
	setsockopt(m_conn_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)& tv, sizeof(DWORD));

	char* data_ptr = static_cast<char*>(data);
	int32_t bytes_recv = 0;

	while (size > 0)
	{
		if ((bytes_recv = SSL_read(static_cast<SSL*>(m_client_ssl), data_ptr, size)) <= 0)
			return SOCKET_ERROR;

		data_ptr += bytes_recv;
		size -= bytes_recv;
	}

	return 1;
}

bool c_connection::connect()
{
	WSADATA ws_data;
	auto retval = 0;
	addrinfo* results = nullptr, *addr, hints{};

	if ((retval = WSAStartup(MAKEWORD(2, 2), &ws_data)))
		goto cleanup;

	RtlSecureZeroMemory(&hints, sizeof addrinfo);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ((retval = LI_FN(getaddrinfo).cached()(xors("127.0.0.1"), xors("8123"), &hints, &results))) // 108.175.10.81 127.0.0.1
		goto cleanup;

	if (!results)
		goto cleanup;

	addr = results;
	while (addr)
	{
		if ((m_conn_socket = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET)
			goto cleanup;

		if ((retval = ::connect(m_conn_socket, addr->ai_addr, static_cast<socklen_t>(addr->ai_addrlen))) == SOCKET_ERROR)
		{
			LI_FN(closesocket).cached()(m_conn_socket);

			m_conn_socket = INVALID_SOCKET;

			addr = addr->ai_next;
		}
		else
			break;
	}

	LI_FN(freeaddrinfo).cached()(results);
	results = nullptr;

	if (m_conn_socket == INVALID_SOCKET)
		goto cleanup;

	return true;

cleanup:

	if (m_client_ssl)
	{
		SSL_shutdown(static_cast<SSL*>(m_client_ssl));
		SSL_free(static_cast<SSL*>(m_client_ssl));
		SSL_CTX_free(static_cast<SSL_CTX*>(m_ssl_ctx));

		m_client_ssl = nullptr;
	}

	if (m_conn_socket != INVALID_SOCKET)
	{
		retval = LI_FN(shutdown).cached()(m_conn_socket, SD_BOTH);
		retval = LI_FN(closesocket).cached()(m_conn_socket);
		m_conn_socket = INVALID_SOCKET;
	}

	if (results != nullptr)
	{
		LI_FN(freeaddrinfo).cached()(results);
		results = nullptr;
	}

	LI_FN(WSACleanup).cached()();
	return false;
}

bool c_connection::tls_connect()
{
	if ((m_ssl_ctx = static_cast<void*>(SSL_CTX_new(TLS_client_method()))) == nullptr)
		return false;

	m_client_ssl = static_cast<void*>(SSL_new(static_cast<SSL_CTX*>(m_ssl_ctx)));
	SSL_set_fd(static_cast<SSL*>(m_client_ssl), m_conn_socket);
	if (SSL_connect(static_cast<SSL*>(m_client_ssl)) < 0)
		return false;

	return true;
}

void c_connection::disconnect()
{
	auto retval = 0;

	if (m_client_ssl)
	{
		SSL_shutdown(static_cast<SSL*>(m_client_ssl));
		SSL_free(static_cast<SSL*>(m_client_ssl));
		SSL_CTX_free(static_cast<SSL_CTX*>(m_ssl_ctx));

		m_client_ssl = nullptr;
	}

	if (m_conn_socket != INVALID_SOCKET)
	{
		// No more data to send
		LI_FN(shutdown).cached()(m_conn_socket, SD_BOTH);
		LI_FN(closesocket).cached()(m_conn_socket);

		m_conn_socket = INVALID_SOCKET;
	}

	LI_FN(WSACleanup).cached()();
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

int c_connection::receive()
{
	m_buffer.clear();

	size_t size = 0;
	int result = 0;

	if ((result = receive_impl(&size, sizeof size)) == 1)
	{
		if (size > 0)
		{
			m_buffer.resize(size);

			if ((result = receive_impl(m_buffer.data(), size)) != 1)
				m_buffer.clear();
		}
	}

	return result;
}

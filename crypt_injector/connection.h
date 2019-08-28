#pragma once

class c_connection
{
	// gay hack
	void* m_client_ssl = nullptr;
	void* m_ssl_ctx = nullptr;
	WSADATA m_wsa_data = {};
	SOCKET m_conn_socket = INVALID_SOCKET;
	std::vector<uint8_t> m_buffer;

	int send_impl(const void* data, size_t size) const;
	int receive_impl(void* data, size_t size) const;
public:
	bool connect();
	bool tls_connect();
	void disconnect();
	int send();
	int receive();

	__forceinline void set_buffer(const std::vector<uint8_t>& buffer)
	{
		m_buffer.assign(buffer.begin(), buffer.end());
	}

	template <std::size_t N>
	__forceinline void set_buffer(uint8_t(&buffer)[N])
	{
		m_buffer.resize(N);
		memcpy(&m_buffer[0], &buffer[0], N * sizeof(uint8_t));
	}

	__forceinline void set_buffer(const void* data, size_t size)
	{
		m_buffer.resize(size);
		memcpy(&m_buffer[0], data, size * sizeof(uint8_t));
	}

	__forceinline void add_buffer(const std::vector<uint8_t>& buffer)
	{
		m_buffer.insert(m_buffer.end(), buffer.begin(), buffer.end());
	}

	template <std::size_t N>
	__forceinline void add_buffer(uint8_t(&buffer)[N])
	{
		m_buffer.resize(m_buffer.size() + N);
		memcpy(&m_buffer[m_buffer.size() - N], &buffer[0], N * sizeof(uint8_t));
	}

	__forceinline void clear_buffer()
	{
		m_buffer.clear();
	}

	__forceinline const std::vector<uint8_t>& get_buffer() const
	{
		return m_buffer;
	}

	__forceinline size_t buffer_size() const
	{
		return m_buffer.size();
	}

	__forceinline void* buffer_data()
	{
		return m_buffer.data();
	}
};
#pragma once
#include <string>
#include <vector>

#include "portable_executable.hpp"
#include "hash.h"

class map_ctx
{
public:
	map_ctx() noexcept {}
	explicit map_ctx(uint64_t new_image_name, std::vector<std::byte> new_buffer) noexcept :
		m_image_name_hash(new_image_name), m_pe(new_buffer) {}

	explicit map_ctx(std::string new_image_name, std::vector<std::byte> new_buffer) noexcept :
		m_image_name(new_image_name), m_image_name_hash(fnvr(new_image_name.c_str())), m_pe(new_buffer) {}

	std::byte* pe_buffer() noexcept;
	portable_executable& pe() noexcept;
	std::string& image_name() noexcept;
	uint64_t& image_name_hash() noexcept;
	std::uintptr_t& local_image() noexcept;
	std::uintptr_t& remote_image() noexcept;

private:
	uint64_t m_image_name_hash = 0;
	std::string m_image_name{};
	portable_executable m_pe{};
	std::uintptr_t m_local_image = 0;
	std::uintptr_t m_remote_image = 0;
};

#pragma once

#include <array>

namespace shellcode
{
	constexpr std::size_t call_dllmain_entry_size = 41;
	std::array<std::byte, call_dllmain_entry_size> call_dllmain_entry(std::uintptr_t image, std::uintptr_t dllmain) noexcept;

	constexpr std::size_t hijack_dllmain_entry_size = 114;
	std::array<std::byte, hijack_dllmain_entry_size> hijack_dllmain_entry(std::uintptr_t image, std::uintptr_t dllmain) noexcept;

	constexpr std::size_t call_dllmain_exit_size = 41;
	std::array<std::byte, call_dllmain_exit_size> call_dllmain_exit(std::uintptr_t image, std::uintptr_t dllmain) noexcept;

	constexpr std::size_t hijack_dllmain_exit_size = 114;
	std::array<std::byte, hijack_dllmain_exit_size> hijack_dllmain_exit(std::uintptr_t image, std::uintptr_t dllmain) noexcept;
}
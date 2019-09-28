#pragma once
#include "map_context.hpp"
#include "process.hpp"

namespace injection
{
	class executor
	{
	public:
		enum mode
		{
			CREATE_THREAD,
			HIJACK_THREAD,
			UNSPECIFIED
		};

		explicit executor(injection::executor::mode execution_mode) noexcept : m_mode(execution_mode) {}
		executor() : m_mode(UNSPECIFIED) {}

		// ACCESSORS
		injection::executor::mode& execution_mode() noexcept;

		// SANTIY
		explicit operator bool() noexcept
		{
			return this->execution_mode() != injection::executor::mode::UNSPECIFIED;
		}

		// EXECUTION HANDLER
		bool handle_entry(map_ctx& ctx, native::process& process) noexcept;
		bool handle_exit(map_ctx& ctx, native::process& process) noexcept;

	private:
		bool handle_hijack_entry(map_ctx& ctx, native::process& process) noexcept;
		bool handle_create_entry(map_ctx& ctx, native::process& process) noexcept;

		bool handle_hijack_exit(map_ctx& ctx, native::process& process) noexcept;
		bool handle_create_exit(map_ctx& ctx, native::process& process) noexcept;

		injection::executor::mode m_mode;
	};
	
}
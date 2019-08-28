#include "context.h"

void c_client::run_shared_mem()
{
	while (m_shared_mem_stage != shared_mem_stage::STAGE_INVALID)
	{
		switch (m_shared_mem_stage)
		{
			case shared_mem_stage::STAGE_CREATE:
			{
				if (!m_mem_queue.wait_for_message())
				{
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

					break;
				}
				
				mem_message_t msg{};
				 
				if (!m_mem_queue.pop_message(msg))
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
				else
					m_shared_mem_stage = shared_mem_stage::STAGE_WAITING;

				const auto init_packet = reinterpret_cast<c_mem_init_packet*>(msg.m_data.data());
				if (!m_mem_handler.validate_mem_packet(*init_packet))
				{
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
					break;
				}
				
				m_mem_handler.xor_mem_packet(*init_packet);

				strcpy_s(ctx.m_username, sizeof ctx.m_username, init_packet->m_username);
				m_cookie = init_packet->m_cookie;

				break;
			}

			case shared_mem_stage::STAGE_WAITING:
			{
				static auto old = std::chrono::high_resolution_clock::now();
				auto now = std::chrono::high_resolution_clock::now();
				uint32_t delta;
				
				do
				{
					now = std::chrono::high_resolution_clock::now();
					delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				} while (delta < 5000);
				
				old = now;

				m_cookie = fnvr(m_cookie);
				auto ping_packet = m_mem_handler.create_ping_mem_packet(m_cookie);

				const mem_message_t msg(reinterpret_cast<uint8_t*>(&ping_packet), sizeof ping_packet);

				if (!m_mem_queue.push_message(msg))
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

				break;
			}

			case shared_mem_stage::STAGE_FORCECLOSE:
			{
				memset(ctx.m_instance, 0, 0x500000);
				return;
			}

			case shared_mem_stage::STAGE_CLOSE:
			{
				auto ping_packet = m_mem_handler.create_ping_mem_packet(0);

				const mem_message_t msg(reinterpret_cast<uint8_t*>(&ping_packet), sizeof ping_packet);

				if (!m_mem_queue.push_message(msg))
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

				m_mem_queue.m_init = false;
				
				return;
			}

			default:
			{
				m_shared_mem_stage = shared_mem_stage::STAGE_INVALID;
				break;
			}
		}
	}
}

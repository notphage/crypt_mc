#pragma once

struct c_game_entry
{
	std::string m_name;
	uint32_t m_version;
	game_packet_status_t m_status;

	c_game_entry(const std::string& name, const uint32_t version, game_packet_status_t status)
		: m_name(name), m_version(version), m_status(status)
	{ }
};

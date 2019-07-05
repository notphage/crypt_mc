#pragma once

struct c_game_entry
{
	std::string m_name;
	std::string m_process_name;
	game_packet_status_t m_status;
	std::vector<uint8_t> m_cheat;

	c_game_entry(const std::string& name, const std::string& process, const std::string& file, game_packet_status_t status)
		: m_name(name), m_process_name(process), m_status(status)
	{
		//std::ifstream ifs(std::string("/cheats/") + file, std::ios::binary | std::ios::ate);
		//
		//if (!ifs)
		//	throw std::runtime_error(file + " couldn't be loaded!: " + std::strerror(errno));
		//
		//auto end = ifs.tellg();
		//ifs.seekg(0, std::ios::beg);
		//
		//auto size = static_cast<size_t>(end - ifs.tellg());
		//
		//if (!size)
		//	throw std::runtime_error(file + " has a filesize of 0!: " + std::strerror(errno));
		//
		//m_cheat.resize(size);
		//if (!ifs.read((char*)m_cheat.data(), m_cheat.size()))
		//	throw std::runtime_error(file + " couldn't be read!: " + std::strerror(errno));
	}
};

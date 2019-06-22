#pragma once

#include <stdio.h>
#include <termios.h>

class c_console
{
	/* Read 1 character - echo defines echo mode */
	char getch_(int echo)
	{
		static termios term_new, old;

		tcgetattr(0, &old); /* grab old terminal i/o settings */
		term_new = old; /* make new settings same as old settings */
		term_new.c_lflag &= ~ICANON; /* disable buffered i/o */
		if (echo) {
			term_new.c_lflag |= ECHO; /* set echo mode */
		}
		else {
			term_new.c_lflag &= ~ECHO; /* set no echo mode */
		}
		tcsetattr(0, TCSANOW, &term_new); /* use these new terminal i/o settings now */

		char ch;
		ch = (char)getchar();

		tcsetattr(0, TCSANOW, &old);

		return ch;
	}

	/* Read 1 character without echo */
	char getch()
	{
		return getch_(0);
	}

	/* Read 1 character with echo */
	char getche()
	{
		return getch_(1);
	}

	void input_loop()
	{
		while (!m_stop_loops)
		{
			char c = getch();
			if (c == 8) //Backspace
			{
				if (!m_input_buffer.empty())
				{
					m_input_buffer.pop_back();
					m_input_field = m_input_buffer;
					std::unique_lock<std::mutex> l(m_write_mutex);
					m_condition_variable.notify_one();
				}
			}
			else if (c == 13) //Carriage return
			{
				if (!m_input_buffer.empty())
				{
					std::unique_lock<std::mutex> l(m_read_mutex);
					m_read_queue.push(m_input_buffer);
					write(m_input_buffer);
					m_input_buffer.clear();
					m_input_field = m_input_buffer;
				}
			}
			else if (c >= 32) //Accepted input characters
			{
				m_input_buffer.push_back(c);
				m_input_field = m_input_buffer;
				std::unique_lock<std::mutex> l(m_write_mutex);
				m_condition_variable.notify_one();
			}
		}
	}

	void write_loop()
	{
		while (!m_stop_loops)
		{
			std::unique_lock<std::mutex> l(m_write_mutex);
			m_condition_variable.wait(l);

			if (!m_write_queue.empty()) // Update input field and add a new line
			{
				std::string text = m_write_queue.front();
				m_write_queue.pop();
				std::cout << text << std::endl;
				std::cout << m_input_field;
			}
		}
	}

	std::string              m_input_buffer; //Current input buffer
	std::string              m_input_field; //Printed at the bottom of the console
	std::queue<std::string>  m_write_queue;
	std::queue<std::string>  m_read_queue;
	std::condition_variable  m_condition_variable;
	std::mutex               m_write_mutex;
	std::mutex               m_read_mutex;
	std::thread				 m_write_thread;
	std::thread				 m_input_thread;
	std::atomic<bool>        m_stop_loops;

public:
	c_console()
		: m_write_thread(&c_console::write_loop, this), m_input_thread(&c_console::input_loop, this)
	{ }

	~c_console()
	{
		m_stop_loops = true;
		m_condition_variable.notify_one();
		m_write_thread.join();
		m_input_thread.join();
	}

	void write(const std::string& text)
	{
		std::unique_lock<std::mutex> l(m_write_mutex);
		m_write_queue.push(text);
		m_condition_variable.notify_one();
	}

	bool read(std::string& text)
	{
		std::unique_lock<std::mutex> l(m_read_mutex);
		if (!m_read_queue.empty())
		{
			text = m_read_queue.front();
			m_read_queue.pop();
			return true;
		}
		return false;
	}

};

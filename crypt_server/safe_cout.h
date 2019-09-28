#pragma once
#include <cassert>
#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>

struct safe_ostream
{
	struct guarded_impl
	{
		guarded_impl() = delete;
		guarded_impl(const guarded_impl&) = delete;
		void operator=(const guarded_impl&) = delete;
		guarded_impl(std::ostream& ostream, std::mutex& mutex)
			: ostream_(ostream), guard_(mutex), ofstream_("logs/connection.log", std::ios::out | std::ios::app | std::ios::binary)
		{
		}

		~guarded_impl()
		{
			ostream_.flush();
			ofstream_.flush();
			ofstream_.close();
		}

		template<typename T> void write(const T& x)
		{
			ostream_ << x;

			if (ofstream_.is_open())
				ofstream_ << x;
		}

		std::ostream& ostream_;
		std::ofstream ofstream_;
		std::lock_guard<std::mutex> guard_;
	};

	struct impl
	{
		impl() = delete;
		void operator=(const impl&) = delete;
		explicit impl(std::ostream& ostream, std::mutex& mutex)
			: unique_impl_(new guarded_impl(ostream, mutex))
		{ }

		impl(const impl& rhs)
		{
			assert(rhs.unique_impl_.get());
			unique_impl_.swap(rhs.unique_impl_);
		}

		template<typename T> impl& operator<<(const T& x)
		{
			guarded_impl* p = unique_impl_.get();
			assert(p);
			p->write(x);
			return *this;
		}

		mutable std::unique_ptr<guarded_impl> unique_impl_;
	};

	explicit safe_ostream(std::ostream& ostream)
		: ostream_(ostream)
	{ }

	template<typename T> impl operator<<(const T& x)
	{
		return impl(ostream_, mutex_) << x;
	}

	std::ostream& ostream_;
	std::mutex mutex_;
};

safe_ostream safe_cout(std::cout);
safe_ostream safe_cerr(std::cerr);

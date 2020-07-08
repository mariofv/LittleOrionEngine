#ifndef __ENGINESINK_H__
#define __ENGINESINK_H__

#include <spdlog/sinks/base_sink.h>

template<typename Mutex>
class EngineSink : public spdlog::sinks::base_sink <Mutex>
{
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{

		// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
		// msg.raw contains pre formatted log

		// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
		spdlog::memory_buf_t formatted;
		base_sink<Mutex>::formatter_->format(msg, formatted);
		messages.push_back(fmt::to_string(formatted));
	}

	void flush_() override
	{
	}

public:
	std::vector<std::string> messages;

};

#endif //__ENGINESINK_H__
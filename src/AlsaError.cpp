#include "iimavlib/AlsaError.h"
#include "iimavlib/Utils.h"
#include <alsa/error.h>

namespace iimavlib
{
bool check_call(int res, std::string message)
{
	if (res < 0) {
		logger[log_level::fatal] << message << ": " << snd_strerror(res);
		return false;
	}
	return true;
}
void throw_call(int res, std::string message)
{
	if (!check_call(res,message)) throw std::runtime_error(message+" ("+std::string(snd_strerror(res))+")");
}
void throw_call(bool res, std::string message)
{
	if (!res) {
		logger[log_level::fatal] << message;
		throw std::runtime_error(message);
	}
}
}
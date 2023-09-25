#include "../iimavlib/WinMMError.h"
#include "../iimavlib/Utils.h"
#include <map>

namespace iimavlib {
namespace {
std::map<MMRESULT, std::string> mmerror_strings = InitMap<MMRESULT, std::string>
(MMSYSERR_INVALHANDLE, "Specified device handle is invalid")
(MMSYSERR_NODRIVER, "No device driver is present")
(MMSYSERR_NOMEM, "Unable to allocate or lock memory")
(WAVERR_UNPREPARED, "The buffer pointed to by the pwh parameter hasn't been prepared")
(MMSYSERR_NOTSUPPORTED, "Function isn't supported");

std::string mmresult_to_string(MMRESULT error)
{
	auto it = mmerror_strings.find(error);
	if (it == mmerror_strings.end()) return "Unknown error";
	return it->second;
}
}
bool check_call(MMRESULT res, std::string message)
{
	if (res != MMSYSERR_NOERROR) {
		logger[log_level::fatal] << message << ": " << mmresult_to_string(res) << " (" << res << ")\n";
		return false;
	}
	return true;
}
void throw_call(MMRESULT res, std::string message)
{
	if (!check_call(res, message)) throw std::runtime_error(message + " (" + mmresult_to_string(res) + ")");
}
void throw_call(bool res, std::string message)
{
	if (!res) {
		logger[log_level::fatal] << message << "\n";
		throw std::runtime_error(message);
	}
}
}
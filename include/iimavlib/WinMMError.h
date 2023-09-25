#ifndef WINMMERROR_H_
#define WINMMERROR_H_

#include <string>
#include <windows.h>

namespace iimavlib
{
bool check_call(MMRESULT res, std::string message);
void throw_call(bool res, std::string message);
void throw_call(MMRESULT res, std::string message);
}
#endif // !WINMMERROR_H_

#ifndef ALSAERROR_H_
#define ALSAERROR_H_

#include <string>

namespace iimavlib
{
bool check_call(int res, std::string message);
void throw_call(bool res, std::string message);
void throw_call(int res, std::string message);
}

#endif //ALSAERROR_H_
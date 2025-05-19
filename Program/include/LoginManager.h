#ifndef LM_P
#define LM_P

#include "Role.h"
#include <string>

class LoginManager
{
private:
public:
    static Role authenticate(std::string surname);
};

#endif
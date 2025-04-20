#include "Role.h"
#include <string>

class LoginManager
{
private:
public:
    static Role authenticate(std::string surname);
};

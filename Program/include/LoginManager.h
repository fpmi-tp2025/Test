#ifndef LM_P
#define LM_P

#include "Role.h"
#include "DataAccess.h"
#include <string>

class LoginManager
{
private:
public:
    static Role authenticate(std::string surname);
    void showDriverMenu(DataAccess& db, const std::string& surname);
    void showManagerMenu(DataAccess& db);
};

#endif
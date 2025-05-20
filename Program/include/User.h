#ifndef U_P
#define U_P

#include "Role.h"
#include <string>
#include <vector>

struct User
{
    std::string surname;
    Role role;
    char** avaliableCars;

    static std::vector<std::string> decodeAvaliableCars(const void* blobPtr, int blobSize);
    static std::unique_ptr<char*> encodeAvaliableCars(const std::vector<std::string>& avaliableCars);
};

#endif
#include "Role.h"
#include <string>
#include <vector>

struct User
{
    std::string surname;
    Role role;
    char** avaliableCars;

    static std::vector<std::string> decodeAvailableCars(const void* blobPtr, int blobSize);
    std::unique_ptr<const void> encodeAvailableCars(const std::vector<std::string>& avaliableCars);
};

#include "Role.h"
#include <string>
#include <vector>

struct User
{
    std::string surname;
    Role role;
    char** avaliableCars;

    static std::vector<std::string> decodeAvaliableCars(const void* blobPtr, int blobSize);
    std::unique_ptr<const void> encodeAvaliableCars(const std::vector<std::string>& avaliableCars);
};

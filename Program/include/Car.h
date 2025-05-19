#ifndef CAR_P
#define CAR_P

#include <sqlite3.h>
#include <string>
#include <vector>

struct Car
{
    std::string number;
    std::string brand;
    int usageOnBuyMoment;
    int usage;
    int capableWeight;

    static std::vector<Car> GetCarFromDB(sqlite3_stmt* res);
};

#endif
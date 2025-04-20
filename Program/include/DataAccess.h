#include "Role.h"
#include <string>
#include <chrono>

using string = std::string;
using ymd = std::chrono::year_month_day;

class DataAccess
{
private:
    /* data */
public:
    Role getRoleFor(std::string surname);
    string GetCompletedOrders(string for_who, ymd from, ymd to);
};

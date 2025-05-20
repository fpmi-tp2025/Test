#ifndef DA_P
#define DA_P

#include "Role.h"
#include <string>
#include <chrono>
#include <vector>
#include <sqlite3.h>

using string = std::string;
using ymd = std::chrono::year_month_day;

class DataAccess
{
private:
    sqlite3* db;
    sqlite3_stmt* res;

public:
    DataAccess(string dbName = "database.db");
    Role getRoleFor(std::string surname);
    string GetCompletedOrders(string for_who, ymd from, ymd to);
    string GetCarRunWeightAndData(string number, string surname);
    string GetLowestIncomeDriver();
    string GetLongestRunCar(string surname);
    bool CheckCargoWeight(const std::string &carNumber, double weight);
    void CalculateDriverEarnings(const std::string& startDate, const std::string& endDate);
    double GetDriverEarnings(const std::string& surname, const std::string& startDate, const std::string& endDate);
};

#endif
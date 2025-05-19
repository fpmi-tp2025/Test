#include <gtest/gtest.h>
#include "DataAccess.h"
#include <sqlite3.h>
#include <fstream>
#include <fstream>
#include <chrono>

using namespace std::chrono;

// Test getRoleFor with non-existent user
TEST(DataAccessTest, GetRoleForNonExistentUserThrows) {
    std::ofstream a("FILE");
    a << "Hello, File!\n";
    a.close();
    DataAccess db;
    EXPECT_THROW(db.getRoleFor("NonExistent"), std::invalid_argument);
}

// Test GetCompletedOrders with valid driver and date range
TEST(DataAccessTest, GetCompletedOrdersValid) {
    DataAccess db;
    ymd from = May/1/2023;
    ymd to = May/1/2023;
    std::string result = db.GetCompletedOrders("Ivanov", from, to);
    EXPECT_NE(result.find("distance: 100"), std::string::npos);
}

// Test GetCompletedOrders with invalid date range
TEST(DataAccessTest, GetCompletedOrdersInvalidDateRangeThrows) {
    DataAccess dataAccess;
    ymd from = May/2/2023;
    ymd to = May/1/2023;
    EXPECT_THROW(dataAccess.GetCompletedOrders("Ivanov", from, to), std::invalid_argument);
}

// Test GetCompletedOrders with non-existent driver
TEST(DataAccessTest, GetCompletedOrdersNonExistentDriverThrows) {
    DataAccess db;
    ymd from = May/1/2023;
    ymd to = May/2/2023;
    EXPECT_THROW(db.GetCompletedOrders("NonExistent", from, to), std::invalid_argument);
}

// Test GetCarRunWeightAndData with admin user and existing car
TEST(DataAccessTest, GetCarRunWeightAndDataAdminValid) {
    DataAccess db;
    std::string result = db.GetCarRunWeightAndData("1", "Ivanov");
    EXPECT_NE(result.find("capableWeight: 1500"), std::string::npos) << result;
}

// Test GetCarRunWeightAndData with non-existent car
TEST(DataAccessTest, GetCarRunWeightAndDataNonExistentCarThrows) {
    DataAccess dataAccess;
    EXPECT_THROW(dataAccess.GetCarRunWeightAndData("999", "Ivanov"), std::invalid_argument);
}

// Test GetLowestIncomeDriver with sample data
TEST(DataAccessTest, GetLowestIncomeDriverSampleData) {
    DataAccess dataAccess;
    std::string result = dataAccess.GetLowestIncomeDriver();
    EXPECT_NE(result.find("Surname: Ivanov"), std::string::npos) << result;
}

// Test GetLongestRunCar with admin user
TEST(DataAccessTest, GetLongestRunCarAdmin) {
    DataAccess dataAccess;
    std::string result = dataAccess.GetLongestRunCar("Ivanov");
    EXPECT_NE(result.find("Number: 8"), std::string::npos);
    EXPECT_NE(result.find("Current usage: 100"), std::string::npos);
}

// Test GetLongestRunCar with driver user (requires setup modification)
TEST(DataAccessTest, GetLongestRunCarDriver) {
    // Modify setup to add a driver user with access to car 3
    DataAccess dataAccess;
    std::string result = dataAccess.GetLongestRunCar("TestDriver");
    EXPECT_NE(result.find("Number: 3"), std::string::npos);
}
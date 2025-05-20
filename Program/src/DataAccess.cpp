#include "DataAccess.h"
#include <sstream>
#include "User.h"
#include <iostream>
#include <stdexcept>
#include "Car.h"

void check_rc(int rc, sqlite3* db) {
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

DataAccess::DataAccess(string dbName)
{
    int rc = sqlite3_open(dbName.c_str(), &db);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }
}

Role DataAccess::getRoleFor(std::string surname) {
    const char* sql = "SELECT role FROM User WHERE surname=?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    check_rc(rc, db);
    rc = sqlite3_bind_text(res, 1, surname.c_str(), -1, SQLITE_STATIC);
    check_rc(rc, db);
    
    rc = sqlite3_step(res);
    if (rc == SQLITE_DONE) {
        throw std::invalid_argument("No user with this surname!");
    }
    if (rc != SQLITE_ROW) {
        std::cerr << "Error happened while reading from db (step): " << sqlite3_errmsg(db) << '\n';
        sqlite3_finalize(res);
        return Role::worker; // или другое значение по умолчанию
    }
    
    const unsigned char* value = sqlite3_column_text(res, 0);
    std::string roleStr(reinterpret_cast<const char*>(value));
    
    sqlite3_finalize(res);
    
    if (std::string(reinterpret_cast<const char*>(value)) == "worker") {
        return Role::worker;
    } else if (std::string(reinterpret_cast<const char*>(value)) == "driver") {
        return Role::driver;
    } else {
        std::cerr << "Wrong role returned: " << roleStr << '\n';
        return Role::worker; // или другое значение по умолчанию
    }
}

string DataAccess::GetCompletedOrders(string for_who, ymd from, ymd to) {
    std::string start_str = std::format("{:04}-{:02}-{:02}", int(from.year()), unsigned(from.month()), unsigned(from.day()));
    std::string end_str = std::format("{:04}-{:02}-{:02}", int(to.year()), unsigned(to.month()), unsigned(to.day()));
    
    const char* sql = "SELECT date, surnameOfDriver, numberOfCar, distance, massOfBaggage, cost FROM CompletedOrders WHERE surnameOfDriver = ? AND date BETWEEN ? AND ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return "Error preparing statement";
    }
    
    sqlite3_bind_text(res, 1, for_who.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(res, 2, start_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(res, 3, end_str.c_str(), -1, SQLITE_STATIC);
    
    std::ostringstream oss;
    bool has_results = false;
    while (sqlite3_step(res) == SQLITE_ROW) {
        has_results = true;
        for (int i = 0; i < sqlite3_column_count(res); ++i) {
            const char* column_name = sqlite3_column_name(res, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(res, i));
            oss << column_name << ": " << (column_value ? column_value : "NULL") << " | ";
        }
        oss << std::endl;
    }
    
    sqlite3_finalize(res);
    
    if (!has_results) {
        throw std::runtime_error("No completed orders found for driver '" + for_who + "' in the specified date range.");
    }
    
    return oss.str();
}

string DataAccess::GetCarRunWeightAndData(string number, string surname)
{
    const char* sql = "SELECT avaliableCars, role FROM User WHERE surname=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }
    
    sqlite3_bind_text(res, 1, surname.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(res);
    
    if (rc != SQLITE_ROW) {
        std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
    }
    const void* value = sqlite3_column_text(res, 0); 
    int blobSize = sqlite3_column_bytes(res, 0);
    
    const unsigned char* role = sqlite3_column_text(res, 1);
    if (std::string(reinterpret_cast<const char*>(value)) == "driver") {
        
        auto cars = User::decodeAvaliableCars(value, blobSize);
        
        if (std::find(cars.begin(), cars.end(), number) == cars.end()) {
            throw std::invalid_argument("user doesn't have access to this car");
        }
    }
    
    const char* sql2 = "SELECT capableWeight, usage FROM Car WHERE number=?;";
    
    if (sqlite3_prepare_v2(db, sql2, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }
    
    sqlite3_bind_text(res, 1, number.c_str(), -1, SQLITE_STATIC);
    if (rc == SQLITE_DONE) {
        throw std::invalid_argument("No car with this number");
    }
    if (rc != SQLITE_ROW) {
        std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
    }
    
    std::ostringstream oss;
    while (sqlite3_step(res) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(res); // Get number of columns
        
        for (int i = 0; i < column_count; ++i) {
            const char* column_name = sqlite3_column_name(res, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(res, i));
            
            oss << column_name << ": " << (column_value ? column_value : "NULL") << " | ";
        }
        oss << std::endl; // New row
    }
    
    sqlite3_finalize(res);
    return oss.str();
    
}

string DataAccess::GetLowestIncomeDriver()
{
    std::string result;
    sqlite3_stmt* stmt;
    
    // Query to find the driver with the fewest trips and their total earnings
    const char* query = R"(
        SELECT d.*, COUNT(co.id) as trip_count, SUM(co.cost) as total_earnings
        FROM Driver d
        LEFT JOIN CompletedOrders co ON d.surname = co.surnameOfDriver
        GROUP BY d.surname
        ORDER BY trip_count ASC
        LIMIT 1
    )";
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        result = "Error preparing query: " + std::string(sqlite3_errmsg(db));
        return result;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Get all driver information
        int number = sqlite3_column_int(stmt, 0);
        const unsigned char* surname = sqlite3_column_text(stmt, 1);
        int category = sqlite3_column_int(stmt, 2);
        int experience = sqlite3_column_int(stmt, 3);
        const unsigned char* address = sqlite3_column_text(stmt, 4);
        int yearOfBirth = sqlite3_column_int(stmt, 5);
        int trip_count = sqlite3_column_int(stmt, 6);
        double total_earnings = sqlite3_column_double(stmt, 7);
        
        // Format the result string
        result += "Driver with fewest trips (" + std::to_string(trip_count) + "):\n";
        result += "Number: " + std::to_string(number) + "\n";
        result += "Surname: " + std::string(reinterpret_cast<const char*>(surname)) + "\n";
        result += "Category: " + std::to_string(category) + "\n";
        result += "Experience: " + std::to_string(experience) + " years\n";
        result += "Address: " + std::string(reinterpret_cast<const char*>(address)) + "\n";
        result += "Year of Birth: " + std::to_string(yearOfBirth) + "\n";
        result += "Total Earnings: " + std::to_string(total_earnings) + "\n";
    } else {
        result = "No drivers found in the database.";
    }
    
    sqlite3_finalize(stmt);
    return result;
}

string DataAccess::GetLongestRunCar(string surname)
{
    std::string result;
    sqlite3_stmt* stmt;
    
    // Query to find the car with the highest total usage
    const char* query = R"(
        SELECT c.*
        FROM Car c
        ORDER BY c.usage DESC
    )";
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        result = "Error preparing query: " + std::string(sqlite3_errmsg(db));
        return result;
    }
    
    auto cars = Car::GetCarFromDB(res);
    
    sqlite3_finalize(stmt);
    
    Role role = getRoleFor(surname);
    
    if (role == driver) {
        const char* sql = "SELECT avaliableCars FROM User WHERE surname=?;";
        
        if (sqlite3_prepare_v2(db, sql, -1, &res, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement." << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }
        
        sqlite3_bind_text(res, 1, surname.c_str(), -1, SQLITE_STATIC);
        
        int rc = sqlite3_step(res);
        
        if (rc != SQLITE_ROW) {
            std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
        }
        const void* value = sqlite3_column_text(res, 0); 
        int blobSize = sqlite3_column_bytes(res, 0);
        
        sqlite3_finalize(res);
        
        auto carsForUser = User::decodeAvaliableCars(value, blobSize);
        
        for (auto i : cars) {
            
            if (std::find(carsForUser.begin(), carsForUser.end(), i.number) != carsForUser.end()) {
                std::ostringstream oss;
                
                oss << "Car with highest mileage (" << i.usage << "):\n"
                << "Number: " << i.number << "\n"
                << "Brand: " << i.brand << "\n"
                << "Usage at purchase: " << i.usageOnBuyMoment << "\n"
                << "Current usage: " << i.usage << "\n"
                << "Capable weight: " << i.capableWeight << "\n";
                
                return oss.str();
            }
        }
    } else {
        auto i = cars[0];
        
        std::ostringstream oss;
        
        oss << "Car with highest mileage (" << i.usage << "):\n"
        << "Number: " << i.number << "\n"
        << "Brand: " << i.brand << "\n"
        << "Usage at purchase: " << i.usageOnBuyMoment << "\n"
        << "Current usage: " << i.usage << "\n"
        << "Capable weight: " << i.capableWeight << "\n";
        
        return oss.str();
    }
}

bool DataAccess::CheckCargoWeight(const std::string& carNumber, double weight) {
    const char* sql = "SELECT capableWeight FROM Car WHERE number = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, carNumber.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }
    
    double capableWeight = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    
    return weight <= capableWeight;
}

void DataAccess::CalculateDriverEarnings(const std::string& startDate, const std::string& endDate) {
    const char* sql = R"(
        INSERT INTO DriverEarnings (surname, earnings, period_start, period_end)
        SELECT surnameOfDriver, SUM(cost * 0.2), ?, ?
        FROM CompletedOrders
        WHERE date BETWEEN ? AND ?
        GROUP BY surnameOfDriver
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return;
    }
    
    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, endDate.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
    }
    
    sqlite3_finalize(stmt);
}

double DataAccess::GetDriverEarnings(const std::string& surname, const std::string& startDate, const std::string& endDate) {
    const char* sql = R"(
        SELECT SUM(cost * 0.2)
        FROM CompletedOrders
        WHERE surnameOfDriver = ? AND date BETWEEN ? AND ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return 0.0;
    }
    
    sqlite3_bind_text(stmt, 1, surname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, endDate.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0.0;
    }
    
    double earnings = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    
    return earnings;
}
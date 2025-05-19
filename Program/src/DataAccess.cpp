#include "DataAccess.h"
#include <sstream>
#include "User.h"

void check_rc(int rc, sqlite3* db) {
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

DataAccess::DataAccess(string dbName)
{
    int rc = sqlite3_open(dbName, &db);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return nullptr;
    }
}

Role DataAccess::getRoleFor(std::string surname)
{
    const char* sql = "SELECT role FROM User WHERE surname=?";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    check_rc(rc, db);
    rc = sqlite3_bind_text(res, 1, surname, -1, SQLITE_STATIC);
    check_rc(rc, db);
    rc = sqlite3_step(res);
    if (rc != SQLITE_ROW) {
        std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
    }
    const unsigned char* value = sqlite3_column_text(res, 0);
    
    sqlite3_finalize(stmt);
    
    if (value == "worker") {
        return Role::worker;
    } else if (value == "driver") {
        return Role::driver;
    } else {
        std::cerr << "Wrong role returned: " << value << '\n';
    }
    
    
}

string DataAccess::GetCompletedOrders(string for_who, ymd from, ymd to)
{
    // Convert dates to string format
    std::string start_str = std::format("{:04}-{:02}-{:02}", int(from.year()), unsigned(from.month()), unsigned(from.day()));
    std::string end_str = std::format("{:04}-{:02}-{:02}", int(to.year()), unsigned(to.month()), unsigned(to.day()));
    
    // Prepare SQL statement with placeholders
    const char* sql = "SELECT date, surnameOfDriver, numberOfCar, distance, massOfBaggage, cost FROM CompletedOrders WHERE date BETWEEN ? AND ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        sqlite3_close(db);
        return 1;
    }
    
    // Bind parameters
    sqlite3_bind_text(res, 1, start_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(res, 2, end_str.c_str(), -1, SQLITE_STATIC);
    
    // Execute query and fetch results
    std::ostringstream oss;
    while (sqlite3_step(res) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt); // Get number of columns

        for (int i = 0; i < column_count; ++i) {
            const char* column_name = sqlite3_column_name(stmt, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            oss << column_name << ": " << (column_value ? column_value : "NULL") << " | ";
        }
        oss << std::endl; // New row
    }

    sqlite3_finalize(stmt);
    return oss.str();
}

string DataAccess::GetCarRunWeightAndData(string number, string surname)
{
    const char* sql = "SELECT avaliableCars FROM User WHERE surname=?;";

    if (sqlite3_prepare_v2(db, sql, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(res, 1, surname.c_str(), -1, SQLITE_STATIC);

    if (rc != SQLITE_ROW) {
        std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
    }
    const void* value = sqlite3_column_text(res, 0); 
    int blobSize = sqlite3_column_bytes(stmt, 0);

    auto cars = User::decodeAvaliableCars(value, blobSize);

    if (!cars.contains(number)) {
        throw error;
    }

    const char* sql2 = "SELECT capableWeight, usage FROM Car WHERE Car_pk=?;";

    if (sqlite3_prepare_v2(db, sql2, -1, &res, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(res, 1, number.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_ROW) {
        std::cerr << "Error happend while reading from db (step): " << sqlite3_errmsg(db) << '\n';
    }

    std::ostringstream oss;
    while (sqlite3_step(res) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt); // Get number of columns

        for (int i = 0; i < column_count; ++i) {
            const char* column_name = sqlite3_column_name(stmt, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            oss << column_name << ": " << (column_value ? column_value : "NULL") << " | ";
        }
        oss << std::endl; // New row
    }

    sqlite3_finalize(stmt);
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
        LIMIT 1
    )";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        result = "Error preparing query: " + std::string(sqlite3_errmsg(db));
        return result;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Get all car information
        const unsigned char* number = sqlite3_column_text(stmt, 0);
        const unsigned char* brand = sqlite3_column_text(stmt, 1);
        int usageOnBuyMoment = sqlite3_column_int(stmt, 2);
        int usage = sqlite3_column_int(stmt, 3);
        int capableWeight = sqlite3_column_int(stmt, 4);

        // Format the result string
        result += "Car with highest mileage (" + std::to_string(usage) + "):\n";
        result += "Number: " + std::string(reinterpret_cast<const char*>(number)) + "\n";
        result += "Brand: " + std::string(reinterpret_cast<const char*>(brand)) + "\n";
        result += "Usage at purchase: " + std::to_string(usageOnBuyMoment) + "\n";
        result += "Current usage: " + std::to_string(usage) + "\n";
        result += "Capable weight: " + std::to_string(capableWeight) + "\n";
    } else {
        result = "No cars found in the database.";
    }

    sqlite3_finalize(stmt);
    return result;
}

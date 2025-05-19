#include "Car.h"

std::vector<Car> Car::GetCarFromDB(sqlite3_stmt *res)
{
    std::vector<Car> cars;

    while (sqlite3_step(res) == SQLITE_ROW) {
        Car car;
        
        // Get column values (with null checks)
        const unsigned char* number = sqlite3_column_text(res, 0);
        const unsigned char* brand = sqlite3_column_text(res, 1);
        
        car.number = number ? reinterpret_cast<const char*>(number) : "";
        car.brand = brand ? reinterpret_cast<const char*>(brand) : "";
        car.usageOnBuyMoment = sqlite3_column_int(res, 2);
        car.usage = sqlite3_column_int(res, 3);
        car.capableWeight = sqlite3_column_int(res, 4);
        
        cars.push_back(car);
    }

    return cars;
}
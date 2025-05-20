#include "LoginManager.h"
#include "DataAccess.h"
#include <iostream>
#include <sstream>
#include <chrono>

Role LoginManager::authenticate(std::string surname) {
    DataAccess db;
    return db.getRoleFor(surname);
}

void LoginManager::showDriverMenu(DataAccess& db, const std::string& surname) {
    int choice;
    std::string startDate, endDate, carNumber;
    
    while (true) {
        std::cout << "\nМеню водителя:\n";
        std::cout << "1. Показать данные о моей машине\n";
        std::cout << "2. Показать мои персональные данные\n";
        std::cout << "3. Показать выполненные заказы за период\n";
        std::cout << "4. Показать общий пробег и массу грузов по машине\n";
        std::cout << "5. Показать мою статистику (поездки, грузы, заработок)\n";
        std::cout << "6. Показать мои начисления за период\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;
        
        if (choice == 0) break;
        
        
        switch (choice) {
            case 1: {
                std::cout << "Введите номер машины: ";
                std::cin >> carNumber;
                std::cout << db.GetCarRunWeightAndData(carNumber, surname) << std::endl;
                break;
            }
            case 2: {
                // Реализация показа персональных данных
                break;
            }
            
        }
    }
}

void LoginManager::showManagerMenu(DataAccess &db)
{
}

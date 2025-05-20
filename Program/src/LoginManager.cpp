#include "../include/LoginManager.h"
#include "../include/DataAccess.h"

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
        
        try {
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
                case 3: {
                    std::cout << "Введите начальную дату (ГГГГ-ММ-ДД): ";
                    std::cin >> startDate;
                    std::cout << "Введите конечную дату (ГГГГ-ММ-ДД): ";
                    std::cin >> endDate;
                    std::cout << db.GetCompletedOrders(surname, startDate, endDate) << std::endl;
                    break;
                }
                case 4: {
                    std::cout << "Введите номер машины: ";
                    std::cin >> carNumber;
                    std::cout << db.GetCarRunWeightAndData(carNumber, surname) << std::endl;
                    break;
                }
                case 5: {
                    // Реализация показа статистики
                    break;
                }
                case 6: {
                    std::cout << "Введите начальную дату (ГГГГ-ММ-ДД): ";
                    std::cin >> startDate;
                    std::cout << "Введите конечную дату (ГГГГ-ММ-ДД): ";
                    std::cin >> endDate;
                    // Реализация показа начислений
                    break;
                }
                default:
                    std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
}

void LoginManager::showManagerMenu(DataAccess& db) {
    int choice;
    
    while (true) {
        std::cout << "\nМеню менеджера:\n";
        std::cout << "1. Показать водителя с наименьшим количеством поездок\n";
        std::cout << "2. Показать машину с наибольшим пробегом\n";
        std::cout << "3. Добавить запись\n";
        std::cout << "4. Обновить запись\n";
        std::cout << "5. Удалить запись\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;
        
        if (choice == 0) break;
        
        switch (choice) {
            case 1:
                std::cout << db.GetLowestIncomeDriver() << std::endl;
                break;
            case 2:
                std::cout << db.GetLongestRunCar("") << std::endl;
                break;
            case 3:
                // Реализация добавления записи
                break;
            case 4:
                // Реализация обновления записи
                break;
            case 5:
                // Реализация удаления записи
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }
}
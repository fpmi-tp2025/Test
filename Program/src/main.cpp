#include <iostream>
#include <string>
#include "LoginManager.h"
#include "DataAccess.h"
#include "User.h"

int main() {
    std::string surname;
    std::cout << "Введите вашу фамилию: ";
    std::cin >> surname;
    
    LoginManager loginManager;
    Role role = loginManager.authenticate(surname);
    
    DataAccess db;
    
    if (role == Role::driver) {
        loginManager.showDriverMenu(db, surname);
    } else if (role == Role::worker) {
        loginManager.showManagerMenu(db);
    } else {
        std::cout << "Неизвестная роль пользователя. Доступ запрещен.\n";
        return 1;
    }
    
    return 0;
}
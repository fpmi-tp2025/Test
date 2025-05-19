cmake_minimum_required(VERSION 4.0.0)

project("autopark")

find_package(SQLite3 REQUIRED)

set(SRC "src")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(autopark ${SRC}/main.cpp)

add_library(DataAccess src/DataAccess.cpp)
target_include_directories(DataAccess PUBLIC ${CMAKE_HOME_DIRECTORY}/include)
add_library(User src/User.cpp)
target_include_directories(User PUBLIC ${CMAKE_HOME_DIRECTORY}/include)
add_library(Car src/Car.cpp)
target_include_directories(Car PUBLIC ${CMAKE_HOME_DIRECTORY}/include)
target_link_libraries(DataAccess PUBLIC User Car SQLite3)
add_subdirectory(tests)
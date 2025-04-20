CMAKE_MINIMUM_REQUIRED_VERSION(4.0.0)

project("autopark")

find_package(SQLite3 REQUIRED)

set(SRC "src")

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

add_executable(autopark ${SRC}/main.cpp)
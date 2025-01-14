cmake_minimum_required(VERSION 3.1.0)

project(ChatClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

add_executable(ChatClient
    main.cpp
    loginwindow.cpp
    chatwindow.cpp
    networkmanager.cpp
    registerwindow.cpp
)

target_link_libraries(ChatClient PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
) 
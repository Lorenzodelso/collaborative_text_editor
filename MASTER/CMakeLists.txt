//server
cmake_minimum_required(VERSION 3.15)
project(collaborative_text_editor-master)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Network REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        Qt5::Network
        )
list(REMOVE_DUPLICATES LIBRARIES)
add_executable(collaborative_text_editor-master
        Char.cpp Char.h
        CRDT.cpp CRDT.h
        DocOperation.cpp DocOperation.h
        mainServer.cpp
        QUser.cpp QUser.h
        QUtente.cpp QUtente.h
        QUtenteServer.h QUtenteServer.cpp
        Server.cpp Server.h
        QTcpServerMio.cpp QTcpServerMio.h
        WorkerSocket.cpp WorkerSocket.h
        WorkerDoc.cpp WorkerDoc.h
        )
target_link_libraries(pds1 ${LIBRARIES})
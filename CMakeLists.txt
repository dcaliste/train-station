cmake_minimum_required(VERSION 3.16)

project(train-station
	VERSION 0.2.1
	DESCRIPTION "Train controller")

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

set(QT_MIN_VERSION "5.6.0")
find_package(Qt5 ${QT_MIN_VERSION} COMPONENTS Qml Quick DBus REQUIRED)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package(KF5 COMPONENTS BluezQt REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SAILFISHAPP sailfishapp IMPORTED_TARGET REQUIRED)

add_subdirectory(src)
add_subdirectory(qml)

install(FILES train-station.desktop
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

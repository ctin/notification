cmake_minimum_required(VERSION 3.0.0 )
project(notifications)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(src main.cpp
	mainwindow.cpp
	Notifications/NotificationWidget.cpp
	Notifications/NotificationLayout.cpp
	Result/Result.cpp
	Operation/Operation.cpp)
set(headers mainwindow.h
	Notifications/NotificationWidget.h
	Notifications/NotificationLayout.h
	Result/Result.h
	Operation/Operation.h)
set(forms mainwindow.ui)
add_executable(notifications ${src} ${headers} ${forms})
target_link_libraries(notifications PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_compile_definitions(notifications PRIVATE  QT_DEPRECATED_WARNINGS)
set_target_properties(notifications PROPERTIES DEBUG_POSTFIX "d")
install(TARGETS notifications DESTINATION Bin)

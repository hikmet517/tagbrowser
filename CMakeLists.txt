cmake_minimum_required(VERSION 3.16)

project(tagbrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(tagbrowser
  main.cpp
  MainWindow.hpp
  MainWindow.cpp
  TMSU.hpp
  TMSU.cpp
  FileData.hpp
  ThumbnailModel.hpp
  ThumbnailModel.cpp
  ThumbnailView.hpp
  ThumbnailView.cpp
  TagWidget.hpp
  TagWidget.cpp
  TagEdit.hpp
  TagEdit.cpp
  FilterWidget.hpp
  FilterWidget.cpp
  FilterTagProxyModel.hpp
  FilterTagProxyModel.cpp
  )

find_package(KF5KIO REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(tagbrowser Qt5::Widgets Qt5::Core  KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM sqlite3)

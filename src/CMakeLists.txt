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
  ThumbnailJob.hpp
  ThumbnailJob.cpp
  ThumbnailJobSQLite.hpp
  ThumbnailJobSQLite.cpp
  )


find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(tagbrowser Qt5::Widgets Qt5::Core sqlite3 ffmpegthumbnailer)

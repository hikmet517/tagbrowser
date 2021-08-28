set(tagbrowser_SOURCES
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

if(WIN32)
  add_executable(tagbrowser WIN32 ${tagbrowser_SOURCES})
else()
  add_executable(tagbrowser ${tagbrowser_SOURCES})
endif()


find_package(Qt5 COMPONENTS Widgets Core REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(tagbrowser Qt5::Widgets Qt5::Core sqlite3 ffmpegthumbnailer)

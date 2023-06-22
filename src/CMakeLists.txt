add_executable(train-station
  main.cpp
  )

target_link_libraries(train-station
  PRIVATE
  Qt5::Qml
  Qt5::Quick
  PkgConfig::SAILFISHAPP
  )

install(TARGETS train-station
  DESTINATION ${CMAKE_INSTALL_BINDIR})

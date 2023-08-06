add_executable(train-station
  main.cpp
  interconnect.h
  interconnect.cpp
  frame.h
  frame.cpp
  track.h
  track.cpp
  spp.h
  spp.cpp
  )

target_link_libraries(train-station
  PRIVATE
  Qt5::Qml
  Qt5::Quick
  Qt5::DBus
  KF5::BluezQt
  PkgConfig::SAILFISHAPP
  )

install(TARGETS train-station DESTINATION ${CMAKE_INSTALL_BINDIR})

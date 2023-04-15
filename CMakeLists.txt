cmake_minimum_required (VERSION 3.16)

set(PROJ_NAME rankserver-cpp)
project(${PROJ_NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(Crow REQUIRED)
find_package(sorting REQUIRED)

add_executable(rankserver
  src/main.cpp
  src/PageBuilder.cpp
  src/RankServer.cpp
)
target_include_directories(rankserver
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${Boost_INCLUDE_DIR}
)
target_link_libraries(rankserver
  ${Boost_LIBRARIES}
  sorting
  Crow::Crow
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJ_NAME}ConfigVersion.cmake"
    VERSION 0.0
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS rankserver
    EXPORT ${PROJ_NAME}Targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJ_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJ_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJ_NAME}
)

install(EXPORT ${PROJ_NAME}Targets DESTINATION lib/cmake/${PROJ_NAME})
install(FILES "${PROJECT_BINARY_DIR}/${PROJ_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJ_NAME}Config.cmake"
        DESTINATION lib/cmake/${PROJ_NAME})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

cmake_minimum_required(VERSION 3.8)
project(vda5050_core)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake QUIET)
find_package(PahoMqttCpp REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

include(GNUInstallDirs)

option(ENABLE_ROS2 "Enable ROS 2 message support in JSON utilities" OFF)
if(ament_cmake_FOUND AND ENABLE_ROS2)
  message(STATUS "Building vda5050_json_utils with ROS 2 support")
  find_package(vda5050_interfaces REQUIRED)
else()
  message(STATUS "Building vda5050_json_utils without ROS 2 support")
endif()

add_library(types INTERFACE)
add_library(${PROJECT_NAME}::types ALIAS types)
target_include_directories(types
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(json_utils INTERFACE)
add_library(${PROJECT_NAME}::json_utils ALIAS json_utils)
target_link_libraries(json_utils
  INTERFACE
    types
    nlohmann_json::nlohmann_json
)
target_include_directories(json_utils
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(ament_cmake_FOUND AND ENABLE_ROS2)
  target_link_libraries(json_utils
    INTERFACE
      ${vda5050_interfaces_LIBRARIES}
  )
  target_include_directories(json_utils
    INTERFACE
      ${vda5050_interfaces_INCLUDE_DIRS}
  )
  target_compile_definitions(json_utils INTERFACE ENABLE_ROS2)
endif()

add_library(logger SHARED src/logger/logger.cpp)
add_library(${PROJECT_NAME}::logger ALIAS logger)
target_link_libraries(logger PUBLIC fmt::fmt)
target_include_directories(logger
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


add_library(transport src/transport/paho_mqtt_client.cpp)
add_library(${PROJECT_NAME}::transport ALIAS transport)
target_link_libraries(transport
  PUBLIC
    PahoMqttCpp::paho-mqttpp3
    logger
)
target_include_directories(transport
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(lib_targets types json_utils logger transport)

install(
  DIRECTORY include/${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
  TARGETS ${lib_targets}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT export_${PROJECT_NAME}
  DESTINATION share/${PROJECT_NAME}/cmake
  NAMESPACE ${PROJECT_NAME}::
)

if(ament_cmake_FOUND)
  ament_export_targets(export_vda5050_core HAS_LIBRARY_TARGET)
  ament_export_dependencies(PahoMqttCpp fmt nlohmann_json)

  if(ENABLE_ROS2)
    ament_export_dependencies(vda5050_interfaces)
  endif()
endif()

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
  if(ament_cmake_FOUND)
    include(cmake/vda5050_ament_lint_common.cmake)
    vda5050_ament_lint_common()

    find_package(ament_cmake_gmock REQUIRED)

    set(json_tests test/json_utils/test_json_utils.cpp)
    if(ENABLE_ROS2)
      list(APPEND json_tests test/json_utils/test_json_utils_ros2.cpp)
    endif()

    ament_add_gmock(test_json_utils ${json_tests})
    target_link_libraries(test_json_utils vda5050_core::json_utils)
    target_include_directories(test_json_utils
      PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    ament_add_gmock(test_transport
      test/transport/unit_mqtt_client_interface.cpp
      test/transport/integration_paho_mqtt_client.cpp
    )
    target_link_libraries(test_transport vda5050_core::transport)

    ament_add_gtest(test_logger
      test/logger/unit_default_logger.cpp
      test/logger/unit_logger.cpp
    )
    target_link_libraries(test_logger vda5050_core::logger)
  else()
    enable_testing()

    include(cmake/vda5050_lint_common.cmake)
    vda5050_lint_common()

    find_package(GTest REQUIRED)
    include(GoogleTest)

    set(json_tests test/json_utils/test_json_utils.cpp)

    add_executable(test_json_utils ${json_tests})
    target_link_libraries(test_json_utils
      GTest::gtest_main
      ${PROJECT_NAME}::json_utils
    )

    add_executable(test_transport
      test/transport/unit_mqtt_client_interface.cpp
      test/transport/integration_paho_mqtt_client.cpp
    )
    target_link_libraries(test_transport
      GTest::gmock_main
      ${PROJECT_NAME}::transport
    )

    add_executable(test_logger
      test/logger/unit_logger.cpp
      test/logger/unit_default_logger.cpp
    )
    target_link_libraries(test_logger
      GTest::gtest_main
      ${PROJECT_NAME}::logger
    )

    set(test_execs
      test_json_utils
      test_transport
      test_logger
    )
    foreach(exec ${test_execs})
      gtest_discover_tests(${exec})
    endforeach()
  endif()
endif()

if(ament_cmake_FOUND)
  ament_package()
endif()

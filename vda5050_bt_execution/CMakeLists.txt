cmake_minimum_required(VERSION 3.20)
project(vda5050_bt_execution)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(vda5050_core REQUIRED)
find_package(vda5050_types REQUIRED)
find_package(vda5050_json_utils REQUIRED)

include(GNUInstallDirs)

file(GLOB_RECURSE srcs "src/vda5050_bt_execution/*.cpp")
add_library(vda5050_bt_execution ${srcs})
target_link_libraries(vda5050_bt_execution
  PUBLIC
    vda5050_core::vda5050_core
    vda5050_types::vda5050_types
    vda5050_json_utils::vda5050_json_utils
    behaviortree_cpp::behaviortree_cpp
)
target_include_directories(vda5050_bt_execution
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
  DIRECTORY include/vda5050_bt_execution
  DESTINATION include
)

install(
  TARGETS vda5050_bt_execution
  EXPORT export_vda5050_bt_execution
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT export_vda5050_bt_execution
  DESTINATION share/${PROJECT_NAME}/cmake
  NAMESPACE vda5050_bt_execution::
)

ament_export_targets(export_vda5050_bt_execution HAS_LIBRARY_TARGET)
ament_export_dependencies(vda5050_core
  vda5050_types
  vda5050_json_utils
  behaviortree_cpp
)

if(BUILD_TESTING)
  find_package(ament_cmake_cppcheck REQUIRED)
  ament_cppcheck()

  find_package(ament_cmake_cpplint REQUIRED)
  ament_cpplint()

  find_package(ament_cmake_clang_format REQUIRED)
  ament_clang_format(
    CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../.clang-format"
  )

  find_package(ament_cmake_copyright REQUIRED)
  ament_copyright()
endif()

ament_package()

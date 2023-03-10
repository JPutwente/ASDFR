cmake_minimum_required(VERSION 3.8)
project(jiwy_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(asdfr_interfaces REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(geometry_msgs REQUIRED)


add_executable(jiwy_controller src/jiwy_controller.cpp)
ament_target_dependencies(jiwy_controller rclcpp std_msgs sensor_msgs asdfr_interfaces)

add_executable(jc_lighttracker src/jc_light_tracker.cpp)
ament_target_dependencies(jc_lighttracker rclcpp std_msgs sensor_msgs asdfr_interfaces geometry_msgs OpenCV cv_bridge)

add_executable(simplesub src/SimpleSub.cpp)
ament_target_dependencies(simplesub rclcpp std_msgs sensor_msgs asdfr_interfaces OpenCV cv_bridge)

target_include_directories(jiwy_controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(jiwy_controller PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS 
  jiwy_controller
  jc_lighttracker
  simplesub
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

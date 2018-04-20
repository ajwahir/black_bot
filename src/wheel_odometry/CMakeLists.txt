cmake_minimum_required(VERSION 2.8.3)
project(wheel_odometry)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  nav_msgs
  geometry_msgs
  tf
)

catkin_package()

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(wheel_odometry src/wheel_odometry.cpp)

add_dependencies(wheel_odometry ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(wheel_odometry
        ${catkin_LIBRARIES})
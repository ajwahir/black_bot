cmake_minimum_required(VERSION 2.8.3)
project(linkay_lidar)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
)

catkin_package()

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(linkay_lidar src/linkay_lidar.cpp)

add_dependencies(linkay_lidar ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(linkay_lidar
        ${catkin_LIBRARIES})
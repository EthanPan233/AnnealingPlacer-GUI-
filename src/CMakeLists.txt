cmake_minimum_required(VERSION 3.10)

# Message
message(STATUS "UBC Feb 2021 Ethan Pan")
message(STATUS "CPEN 513 Assignment2")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS ${CMAKE_MODULE_PATH})

# set the project name
project(AnnealingPlacement)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
  
# add src files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_FILES ${SRC_DIR}/Circuit.cpp ${SRC_DIR}/Cell.cpp ${SRC_DIR}/main.cpp ${SRC_DIR}/draw.cpp ${SRC_DIR}/graphics.cpp)

######################
# Check Dependencies #
######################
# uncommment this line if you are using self-compiled boost lib
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

include_directories(/usr/openwin/lib)
include_directories(/usr/openwin/include)

# add X11
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

# add spdlog
find_package(spdlog REQUIRED)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/spdlog)
# target_link_libraries(AnnealingPlacement PRIVATE spdlog::spdlog_header_only)

# add the executable
add_executable(AnnealingPlacement ${SRC_FILES})



target_include_directories(AnnealingPlacement PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

# Boost
target_include_directories(AnnealingPlacement PUBLIC ${Boost_INCLUDE_DIR} /usr/openwin/lib /usr/openwin/include)
target_link_libraries(AnnealingPlacement PUBLIC ${Boost_LIBRARIES} ${X11_LIBRARIES} spdlog)
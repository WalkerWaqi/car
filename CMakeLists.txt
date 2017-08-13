cmake_minimum_required(VERSION 3.0)
project(car)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -fpermissive")
add_definitions(-DELPP_FEATURE_ALL
                -DELPP_DEFAULT_LOG_FILE="/opt/bin/logger/crash.log")

find_package(Boost COMPONENTS system thread filesystem atomic program_options regex signals)
if(Boost_FOUND)
    message(STATUS "boost library found")
else()
    message(FATAL_ERROR "boost library is needed but cant be found")
endif()

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message(STATUS "protobuf library found")
else()
    message(FATAL_ERROR "protobuf library is needed but cant be found")
endif()
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../../protocols/protocols/TevaProtocols.proto)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/car
        ${CMAKE_CURRENT_SOURCE_DIR}/../../protocols/protocols
        ${CMAKE_CURRENT_SOURCE_DIR}/command
        ${CMAKE_CURRENT_SOURCE_DIR}/session
        ${CMAKE_CURRENT_SOURCE_DIR}/state
        ${CMAKE_CURRENT_SOURCE_DIR}/timer
        ${CMAKE_CURRENT_SOURCE_DIR}/serialport)

file(GLOB_RECURSE HEADER_FILES *.h *.hpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include TOOLS_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/command COMMAND_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/session SESSION_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/state STATE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/timer TIMER_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/serialport SERIALPORT_FILES)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/car/car.cpp ${HEADER_FILES} ${TOOLS_FILES} ${SOURCE_FILES} ${COMMAND_FILES}  ${SESSION_FILES} ${STATE_FILES} ${TIMER_FILES} ${SERIALPORT_FILES} ${PROTO_SRCS})
target_link_libraries(${PROJECT_NAME} pthread ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES} caf_core_static log4cplus)

add_executable(pimcs ${CMAKE_CURRENT_SOURCE_DIR}/pimcs/pimcs.cpp ${CMAKE_CURRENT_SOURCE_DIR}/init.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OptionPrinter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/CustomOptionDescription.cpp)
target_link_libraries(pimcs pthread ${Boost_LIBRARIES})

add_executable(convert ${CMAKE_CURRENT_SOURCE_DIR}/convert/convert.cpp)
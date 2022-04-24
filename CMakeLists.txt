cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)

SET(PROJECT_NAME cpp-v8-typescript-example)
project(${PROJECT_NAME})

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

message(STATUS "Is MSVC: " ${MSVC})

message(STATUS "C++: " ${CMAKE_CXX_STANDARD})
message(STATUS "C: " ${CMAKE_C_STANDARD})

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} 
PRIVATE
	main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE V8_COMPRESS_POINTERS)

target_link_libraries(${PROJECT_NAME} PRIVATE v8 v8_libbase v8_libplatform)

file(GLOB TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ts)
file(COPY ${TS_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB TS_FILES ${CMAKE_BINARY_DIR}/*.ts)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND npx ARGS tsc ${TS_FILES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Executing npx..."
	VERBATIM
)
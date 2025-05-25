cmake_minimum_required(VERSION 3.28)
project(tjvm)

set(VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
	"source/javaParser.cpp" "source/types.cpp"
 "source/javaData.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE "include/")

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

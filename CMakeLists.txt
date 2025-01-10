cmake_minimum_required(VERSION 3.20)
project(OCCTSurfacer VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCASCADE REQUIRED)
find_package(VTK)
find_package(OpenMP)

add_executable(OCCTSurfacer)

target_sources(OCCTSurfacer
    PUBLIC
    main.cpp
    Relation.cpp
    Models.cpp
)
target_include_directories(OCCTSurfacer
    PRIVATE
    ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(OCCTSurfacer
    PRIVATE
    ${OpenCASCADE_LIBRARIES}
)


if(OpenMP_CXX_FOUND)
    target_compile_options(OCCTSurfacer PUBLIC -Xpreprocessor -fopenmp)
    target_link_libraries(OCCTSurfacer PUBLIC omp)
    target_compile_definitions(OCCTSurfacer PUBLIC ENABLE_OPENMP)
else()
    message(WARNING "OpenMP not found. The program will be built without OpenMP support.")
    target_compile_definitions(OCCTSurfacer PUBLIC DISABLE_OPENMP)
endif()

include(CTest)
enable_testing()
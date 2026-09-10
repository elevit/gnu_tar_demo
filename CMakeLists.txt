cmake_minimum_required(VERSION 3.20)
project(LicensedGnuTarDemo VERSION 1.0.0 LANGUAGES CXX)

add_executable(tar_demo
    src/main.cpp
    src/GnuTar.cpp
)
target_include_directories(tar_demo PRIVATE include)
target_compile_features(tar_demo PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(tar_demo PRIVATE /W4 /permissive- /EHsc)
    target_compile_definitions(tar_demo PRIVATE UNICODE _UNICODE)
else()
    target_compile_options(tar_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()


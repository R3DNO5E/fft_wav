cmake_minimum_required(VERSION 3.10)
project(fft_wav)
execute_process(COMMAND sdl2-config --cflags --libs OUTPUT_VARIABLE SDL2_CFLAGS)
string(REGEX REPLACE "\n" " " SDL2_CFLAGS ${SDL2_CFLAGS})
set(CMAKE_CXX_FLAGS "${SDL2_CFLAGS} ${CMAKE_CXX_FLAGS} ")
message("${SDL2_CFLAGS}")
set(CMAKE_CXX_STANDARD 14)

add_library(wav libwav.cpp)
add_library(plot libplot.cpp)

add_executable(fft_wav main.cpp)

target_link_libraries(plot SDL2)
target_link_libraries(plot SDL2_ttf)

target_link_libraries(fft_wav wav)
target_link_libraries(fft_wav plot)
target_link_libraries(fft_wav fftw3)
target_link_libraries(fft_wav SDL2)

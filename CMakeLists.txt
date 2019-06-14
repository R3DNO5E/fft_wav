cmake_minimum_required(VERSION 3.10)
project(fft_wav)

set(CMAKE_CXX_STANDARD 14)

add_library(wav libwav.cpp)
add_library(plot libplot.cpp)

add_executable(fft_wav main.cpp)

target_link_libraries(fft_wav SDL2)
target_link_libraries(plot SDL2)
target_link_libraries(plot SDL2_ttf)

target_link_libraries(fft_wav wav)
target_link_libraries(fft_wav plot)
target_link_libraries(fft_wav fftw3)

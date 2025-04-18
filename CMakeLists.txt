cmake_minimum_required(VERSION 3.27)
project(stream_vcmax_without_urban)

set(CMAKE_CXX_STANDARD 20)



include_directories(.)

add_executable(stream_vcmax_without_urban
        main.cpp
        aerodynamic.h
        biochemical.cpp
        biochemical.h
        control.h
        defined.cpp
        defined.h
        eb.cpp
        eb.h
        engine.h
        engine.cpp
        evapo.cpp
        evapo.h
        fileio.cpp
        fileio.h
        leafopt.h
        growth.h
        pixelio.cpp
        pixelio.h
        rt.cpp
        rt.h
        scifuns.h
        geometry.h
        structs.h
        utils.cpp
        utils.h
        modelio.h
        modelio.cpp
        thread_pool.hpp
        thread_pool.cpp
        model.h
        model.cpp
        aerodynamic.cpp
        scifuns.cpp
        leafopt.cpp
        geometry.cpp
        sensor.cpp
        sensor.h
        soilopt.cpp
        spa.cpp
        spa.h
)

find_package(GDAL CONFIG REQUIRED)

target_link_libraries(stream_vcmax_without_urban PRIVATE GDAL::GDAL)



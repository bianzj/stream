cmake_minimum_required(VERSION 3.27)
project(stream1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)



include_directories(.)

add_executable(stream
        china/main.cpp
        base/aerodynamic.h
        base/biochemical.cpp
        base/biochemical.h
        base/control.h
        base/defined.cpp
        base/defined.h
        base/eb.cpp
        base/eb.h
        base/engine.h
        base/engine.cpp
        base/evapo.cpp
        base/evapo.h
        base/fileio.cpp
        base/fileio.h
        base/leafopt.h
        base/growth.h
        base/pixelio.cpp
        base/pixelio.h
        base/rt.cpp
        base/rt.h
        base/scifuns.h
        base/geometry.h
        base/structs.h
        base/utils.cpp
        base/utils.h
        base/modelio.h
        base/modelio.cpp
        base/thread_pool.hpp
        base/thread_pool.cpp
        base/model.h
        base/model.cpp
        base/aerodynamic.cpp
        base/scifuns.cpp
        base/leafopt.cpp
        base/geometry.cpp
        base/sensor.cpp
        base/sensor.h
        base/soilopt.cpp
        base/spa.cpp
        base/spa.h
)

find_package(GDAL CONFIG REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(NETCDF REQUIRED IMPORTED_TARGET netcdf)

target_link_libraries(stream PRIVATE
        GDAL::GDAL
        HDF5::HDF5
        PkgConfig::NETCDF
)



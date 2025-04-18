//
// Created by airt on 24-1-18.
//

#ifndef STREAM_SENSOR_H
#define STREAM_SENSOR_H


#include "structs.h"

//----------------------------------------
// 1) the viewing  geometry
// 2) the radiative transfer process
//-----------------------------------------
class Sensor {
    Sensor(){};


    void calculate_geometry();
    void read_geometry(std::string vzestr, std::string vaastr,
                       std::string szastr, std::string saastr);
    void directinal_temperature_from_existing_sensor();
    void directinal_temperature_from_virtual_sensor();


};


#endif //STREAM_SENSOR_H

#pragma once
#include <iostream>

#include "fileio.h"
#include "modelio.h"
#include "thread_pool.hpp"
#include "rt.h"
#include "evapo.h"
#include "eb.h"
#include "aerodynamic.h"
#include "biochemical.h"
#include "thread_pool.hpp"
#include "soilopt.h"
#include "leafopt.h"
#include "geometry.h"


class Model
{

public:
    Model(){};
// input parameter;
// variable initial;

// for each day
    // for each time mode
        // optional: variable reinput from outside;
        // calculate the radiation first;
        // calculate the aerodynamic and surface resistance;
        // energy balance to yield the suitable temperature;
        // upload the variable;
        // observation mission check; 
        // image output;
    // upload the scene;


    void init();
    void runpixel(std::shared_ptr<Defined> define, std::shared_ptr<PixelIO> pixelio);
    void run();
   // void runpixel(std::shared_ptr<PixelIO> pixelio);

    void solarAngle(std::shared_ptr<Defined> defined,std::shared_ptr<PixelIO> pixelIO);
   // ModelIO m_modelio;

    //void inputMeta(std::string);

    void inputGeoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio);

    void inputGeoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO>& modelio,
                      int startWidth, int endWidth, int startHeight, int endHeight);

    void updateVegData(std::shared_ptr<PixelIO> &pixelio, std::shared_ptr<ModelIO>& modelio, int ksubType);  //change，添加根据type更新

    int inputMeteoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio, int year, int doy);

    int inputSatData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio, int year, int doy);

    void inputDefinedData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio);

    void flash(std::shared_ptr<PixelIO> &pixelio);

    RT m_rt;
    EB m_eb;
    Evapo m_evapo;
    Aerodynamic m_aero;
    BioChemical m_bio;
    Geometry m_geometry;




};
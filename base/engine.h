#pragma once
#include <iostream>

#include "modelio.h"
#include "thread_pool.hpp"
//#include "rt.h"
//#include "evapo.h"
//#include "eb.h"
//#include "aerodynamic.h"
//#include "biochemical.h"
//#include "thread_pool.hpp"
#include "model.h"
#include "sensor.h"

class Engine
{

public:
    Engine(){
        m_model = std::make_shared<Model>();
        m_modelio = std::make_shared<ModelIO>();
        m_fileio = std::make_shared<FileIO>();
    };
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

    //void ttt(std::shared_ptr<PixelIO> a);
    void initProject(std::string infilepath);
    void initVariable();
    void initVariable(int startWidth,int endWidth, int startHeight, int endHeight);
    int upload(int year, int doy);
    void run();
    void subrun(int startWidth,int endWidth, int startHeight, int endHeight,int theyear, int thedoy, int knode );
    void subrun(int startWidth,int endWidth, int startHeight, int endHeight);
    void sythrun();

    void observe(int knode);
    //void runpixel(std::shared_ptr<PixelIO> pixelio);


    std::shared_ptr<Model> m_model;
    std::shared_ptr<ModelIO> m_modelio;
    std::shared_ptr<FileIO> m_fileio;
    std::shared_ptr<Sensor> m_sensor;
//    int year = 2019;
//    int doy = 253;


};
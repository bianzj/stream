#pragma once
#include "structs.h"
#include "defined.h"
#include "pixelio.h"
#include "scifuns.h"

struct Temperature
{
    float Tss;
    float Tsh;
    float Tcs;
    float Tch;
};

class EB
{
public:
    EB(){};

    void run(PixelIO * pixelio){};


    bool rebalance(std::shared_ptr<PixelIO> m_pPixelio);
    bool rebalance_urban(std::shared_ptr<PixelIO> m_pPixelio);

};

//
//void balance(NetRad netrad, Heatflux heatflux, BioState biostate, Resistance resist,Meteo meteo,Thermal thermal)
//{
//
//
//    int count=0;
//
//    // aerodynamic resistance
//
//    for(int kiter; kiter < N_ITER; kiter ++)
//    {
//        // m_direct upload
//
//        // longwave radiative transfer for netrad;
//
//        // surface resistance for rss
//
//        // evapotranspiration for heatflux
//
//    }
//
//
//}






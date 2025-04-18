
/*

in this case, the soil reflectance will be calculated;

*/
#pragma once

#include "structs.h"
#include "scifuns.h"

class SoilOpt{
public:
    SoilOpt(){};


    void bsm(OptCoeff bsmCoeff, BSMParam bsm, float SMC, Spectral &spectral);


};
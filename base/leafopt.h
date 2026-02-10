/*
in this case, the leaf reflectance and transpiration will be modeled;

*/

#pragma once
#include "defined.h"
#include "structs.h"
#include "scifuns.h"
#include <math.h>



class LeafOpt
{
public:
    LeafOpt(){};


    void fluspect(OptCoeff fluspectCoeff,FluspectParam fluspectParam,Spectral& spectral);

};



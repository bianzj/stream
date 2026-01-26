#pragma once
#include <math.h>




namespace SCI{


    float Planck(float wl, float T);

    float invPlanck(float wl,float T);


    float StefanBoltzmann(float T);

    float calctav(float alfa,float nr);

    //--------------------------------------------------------------------------------------------
    //  SATE VAPOR USING A TEMPERATURE  T 300K
    //--------------------------------------------------------------------------------------------
    float es_fun(float T);


    //slope of the saturated pressure function
    float s_fun(float es, float T);



    //----------------------------------------------------------
    // quadratic formula, root of least magnitude: AX2 + BX + C = 0
    //    for the eqn ax^2 + bx + c, 
    //    if dsign is:
    //       -1, 0: choose the smaller root
    //       +1: choose the larger root
    //----------------------------------------------------------
    float sel_root(float a,float b,float c,float design);


}




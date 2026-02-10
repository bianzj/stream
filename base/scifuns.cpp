#include <math.h>

#include "scifuns.h"


namespace SCI{


    //------------------------------
    //--- Planck and invPlanck
    //------------------------------
    float Planck(float wl, float T)
    {
        float c1=11910.439340652;
        float c2=14388.291040407;
        float ts=T;
        if(wl > 50) wl= wl/1000;

        if(T<100) ts=T+273.15;
        float eb = c1/((pow(wl,5))*(exp(c2/ts/wl)-1))*10000;
        // eb[i+npoly]=eb[i];
        return eb;
    }
    float invPlanck(float wl, float rad)
    {
        float c1 = 11910.439340652 * 10000;
        float c2 = 14388.291040407;
        if(wl > 50) wl = wl/1000.0;
        float temp = c1 / (rad * pow(wl, 5)) + 1;
        float Ts = c2 / (wl * log(temp));
        return Ts;
    }

    //----------------------------------------
    //--- StefanBoltzmann law
    //----------------------------------------
    float StefanBoltzmann(float T)
    {
        float sigma = 5.6696e-8;
        float eb = sigma * T*T*T*T;
        return eb;
    }
    float invStefanBoltzmann(float eb)
    {
        float sigma = 5.6696e-8;
        float T4 = eb /sigma;
        return pow(T4,0.25);
    }


    //--------------------------------------------------------------------------------------------
    //  SATE VAPOR USING A TEMPERATURE  T 300K
    //--------------------------------------------------------------------------------------------
    float es_fun(float T)
    {
        float a = 7.5;
        float b = 237.3;
        float temp = a*T/(b+T);
        return 6.107*pow(10,temp);
    }

    //slope of the saturated pressure function
    float s_fun(float es, float T)
    {
        return es*2.3026*7.5*237.3/((237.3+T)*(237.3+T));
    }


    //----------------------------------------------------------
    // quadratic formula, root of least magnitude: AX2 + BX + C = 0
    //    for the eqn ax^2 + bx + c, 
    //    if dsign is:
    //       -1, 0: choose the smaller root
    //       +1: choose the larger root
    //----------------------------------------------------------
    float sel_root(float a,float b,float c,float design)
    {
        float x;
        if(a ==0)
        {
            x = -c/b;
        }else
        {
            if(design ==0)
            {
                design = -1;
            }
            x = (-b + design *sqrt(b*b - 4*a*c))/(2*a);
        }
        return x;
    }


    float calctav(float alfa,float nr)
    {
        float rd,pi,n2,np,nm,a,k,sa,b1,b2,b,a3,b3,tp1,tp2,tp3,tp4,ts,tp5,tp,tav;
        pi=3.1415926;
        rd          = pi/180;
        n2          = nr*nr;
        np          = n2+1;
        nm          = n2-1;
        a           = (nr+1)*(nr+1)/2;
        k           = -(n2-1)*(n2-1)/4;
        sa          = sin(alfa*rd);
        b1 = 0;
        if(alfa !=90) b1          = sqrt((sa*sa-np/2)*(sa*sa-np/2)+k);
        b2          = sa*sa-np/2;
        b           = b1-b2;
        b3          = b*b*b;
        a3          = a*a*a;
        ts          = (k*k/(6*b3)+k/b-b/2)-(k*k/(6*a3)+k/a-a/2);

        tp1         = -2*n2*(b-a)/(np*np);
        tp2         = -2*n2*np*log(b/a)/(nm*nm);
        tp3         = n2*(1/b-1/a)/2;
        tp4         = 16*n2*n2*(n2*n2+1)*log((2*np*b-nm*nm)/(2*np*a-nm*nm))/(np*np*np*nm*nm);
        tp5         = 16*n2*n2*n2*(1/(2*np*b-nm*nm)-1/(2*np*a-nm*nm))/(np*np*np);
        tp          = tp1+tp2+tp3+tp4+tp5;
        tav         = (ts+tp)/(2*sa*sa);

        return tav;
    }

}




#pragma once
#include <iostream>
#include <math.h>

#include "structs.h"
#include "defined.h"
#include "pixelio.h"
#include "leafopt.h"
#include "scifuns.h"
#include "soilopt.h"
/*

In this class, the shortwave and longwave radiation can be calculated;

*/

class RT
{
public:
    RT() {};

   // void run(PixelIO * m_pPixelio){};
    void optical(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);
    void netrad_shortwave(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO> m_pPixelio);
    void netrad_longwave(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO>  m_pPixelio);
    void netrad_shortwave_urban(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO> m_pPixelio);
    void netrad_longwave_urban(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO>  m_pPixelio);
    void nadirTir(std::shared_ptr<PixelIO>  &m_pPixelio);

    void satTirt(std::shared_ptr<PixelIO>  &m_pPixelio);

    void tirt_direct(Canopy canopy, Angle angle, float &fss,float &fsh,float &fcs,float &fch);
    void tirt_scatter(Canopy canopy, Angle angle,Spectral spectral, float &mss, float &msh, float &mcs, float &mch);
    void tirt_direct_canopy(Canopy canopy, Angle angle, float &fss,float &fsh,float &fcs,float &fch);

    void calculate_direct_emissivity(Canopy canopy, Angle angle, Building building, float &fss, float &fsh, float &frs, float &frh, float &fws, float &fwh, int ifP);
    void calculate_scattering_emissivity(Canopy canopy, Angle angle, Building building, float &mss, float &msh, float &mrs, float &mrh, float &mws, float &mwh, int ifP);

    void sample_hom(std::shared_ptr<PixelIO> &m_pPixelio);
    void sample_canopy(std::shared_ptr<PixelIO> &m_pPixelio);
    void sample_urban(std::shared_ptr<PixelIO> &m_pPixelio);
private:

//    int m_type = 0; // vegetaion type,which lead to differnt mode;
//    int isMountain = 0; // to check if the moutain should be considered, the dem should be induced;
    LeafOpt m_leafopt;
    SoilOpt m_soilopt;


};






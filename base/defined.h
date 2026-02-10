#pragma once
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "structs.h"
#include "utils.h"
#include "leafopt.h"
#include "soilopt.h"

//---------------------------------------
//---
//--------------------------------------


class Defined
{




public:
    Defined() {

        defineCanopy();
        defineLeafbio();
        defineCanopy_plus();
        defineLeafbio_plus();
    };


    void input(std::string path);
    void destroy();


    std::string predifineDir="./predefine/";
    // unique
    OptCoeff m_optCoeff;
    AeroCoeff m_aerocoeff;

    Canopy m_canopy;
    MeteoMeta m_meta;
    Spectral m_spectral;

    AtomCond m_atomcond;

//    float lrho_ir;
//    float ltau_ir;
//    float rs_ir;

    LeafBio m_leafbio;
    SoilSet m_soilset;

    //function
    LeafOpt m_leafopt;
    SoilOpt m_soilopt;

    void defineCanopy();
    void defineLeafbio();
    void defineCanopy_plus();
    void defineLeafbio_plus();
//    void defineSpectral(); //添加mspectral元素到defined对象

    int m_year;
    int m_doy;
    //int k_node;
//    float* m_direct;
//    float* m_diffuse;
//    float* wl;
    std::map<int,Canopy> m_mCanopy;
    std::map<int,LeafBio> m_mLeafbio;
    std::map<int,Spectral> m_mSpectral;

    std::map<int,Canopy> m_mCanopy_sub;
    std::map<int,LeafBio> m_mLeafbio_sub;
    std::map<int,Spectral> m_mSpectral_sub;

};

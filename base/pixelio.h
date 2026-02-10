#pragma once
#include <memory>
#include "structs.h"
#include "defined.h"

/*

variables: dynamic statc
different variables attached to each pixel/calculation unit,
in which the shortwave and longwave radiation, the leaf state, the soil state

prospect parameters ? the can be the input or the inverted result, for these, they are parameters;
parameters: driven input 

*/


class PixelIO
{
public:
    PixelIO(){
        //m_pDefined = std::make_shared<Defined>();
        //m_pAngle = std::make_shared<Angle>();
        m_pInputset = std::make_shared<InputSet>();
        m_pDynamicVariable = std::make_shared<DynamicVariable>();
        m_pStaticVariable = std::make_shared<StaticVariable>();


    };

//    void inputMeta();
//    void dynamicUpdate();
//    void staticUpdate();
public:
    //std::shared_ptr<Defined> m_pDefined;
    //std::shared_ptr<Angle> m_pAngle;
    std::shared_ptr<InputSet> m_pInputset;
    std::shared_ptr<DynamicVariable> m_pDynamicVariable;
    std::shared_ptr<StaticVariable> m_pStaticVariable;
   // void inputSoilRefl();
    Angle m_angle;

    int k_width;
    int k_height;
    int k_workwidth;
    int k_workheight;
    int n_node;
    int k_node;
    int k_pixel;
    std::vector<float> m_vSkt = std::vector<float>(24,0);
    std::vector<float> m_vTss= std::vector<float>(24,0);
    std::vector<float> m_vTsh= std::vector<float>(24,0);
    std::vector<float> m_vTcs= std::vector<float>(24,0);
    std::vector<float> m_vTch= std::vector<float>(24,0);

    std::vector<float> m_vTrs= std::vector<float>(24,0);
    std::vector<float> m_vTrh= std::vector<float>(24,0);
    std::vector<float> m_vTws= std::vector<float>(24,0);
    std::vector<float> m_vTwh= std::vector<float>(24,0);
    std::vector<float> m_vTts= std::vector<float>(24,0);  //光照街道
    std::vector<float> m_vTth= std::vector<float>(24,0);  //阴影街道

    float m_DBT;
//    float sza;
//    float saa;
    float lat;
    float lon;

    float emis_s;
    float emis_v;
};


// void PixelIO::inputMeta()
// {
   
// }
#pragma once
#include <iostream>

#include "pixelio.h"

class ModelIO
{

public:
    ModelIO(){

        m_pDefined = std::make_shared<Defined>();
        m_vPixelio.clear();
    };


//
//    void upload();

   // std::shared_ptr<FileIO> m_pFIleIO;
    std::vector<std::shared_ptr<PixelIO>> m_vPixelio;
    std::shared_ptr<Defined> m_pDefined;
    //std::vector<float> m_vSkt;

    bool m_isfirst = true;
    bool m_isnodefirst = true;

};
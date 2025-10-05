#pragma once

#include "structs.h"
#include "defined.h"
#include "scifuns.h"
#include "pixelio.h"

class Evapo
{
public:
    Evapo(){}
    void evapotranspiration(std::shared_ptr<PixelIO> m_pPixelio);
    void evapotranspiration_urban(std::shared_ptr<PixelIO> m_pPixelio);
};


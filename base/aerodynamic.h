#pragma once
/*
aerodynamic resistance for differet surface types
*/
#include <math.h>
#include "structs.h"
#include "defined.h"
#include "pixelio.h"

class Aerodynamic
{
public:
    Aerodynamic(){};

    void aeresist(std::shared_ptr<Defined> defined,std::shared_ptr<PixelIO> pixelio);
    void aeresist_urban(std::shared_ptr<Defined> defined,std::shared_ptr<PixelIO> pixelio);
};


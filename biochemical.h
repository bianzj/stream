#pragma once

/*
calculate the vegetation surface resistance in the bio process;
*/


#include "structs.h"
#include "defined.h"
#include "utils.h"
#include "scifuns.h"
#include "pixelio.h"


class BioChemical
{
public:
    BioChemical(){};
    void suresist(std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio);
    void suresist_urban(std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio);
};



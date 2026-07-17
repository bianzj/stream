#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

#include "base/structs.h"
#include "base/utils.h"
#include "base/fileio.h"
#include "base/modelio.h"
#include "base/model.h"
#include "base/engine.h"
#include "base/geometry.h"

#include <fstream>
#include <iostream>
#include <pthread.h>
#include "gdal.h"

#include <ctime>

int main(int argc,char* argv[])
{
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    time_t now = time(0);
    char *dt = ctime(&now);
    std::cout<<"begin time:"<<dt<<std::endl;

//-----------------------------------------------
// huabei area: 1km: width 400-500 180-280 *25
//-----------------------------------------------
    std::string infilepath = "/home/hero/data/stream/input-25km-global-local.txt";

    Engine engine;
    engine.initProject(infilepath);
    engine.run();
    // engine.observe(-1); //这里的observe只会输出最后一天，所以注释掉

    now = time(0);
    dt = ctime(&now);
    std::cout<<"end time:"<<dt<<std::endl;

return 0;

}






 
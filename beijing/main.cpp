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
    const std::string defaultInput = "/home/hero/data/stream/input-100m-beijing-local.txt";
    const std::string infilepath = argc > 1 ? argv[1] : defaultInput;
    if (argc > 2) {
        setenv("STREAM_OUTPUT_DIR", argv[2], 1);
    }
    std::ifstream inputCheck(infilepath);
    if (!inputCheck) {
        std::cerr << "Unable to open input metadata file: " << infilepath << std::endl;
        std::cerr << "Usage: " << argv[0] << " [metadata-file] [output-directory]" << std::endl;
        return 1;
    }

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    time_t now = time(0);
    char *dt = ctime(&now);
    std::cout<<"begin time:"<<dt<<std::endl;

//-----------------------------------------------
// huabei area: 1km: width 400-500 180-280 *25
//-----------------------------------------------
    Engine engine;
    engine.initProject(infilepath);
    engine.run();
    // engine.observe(-1); //这里的observe只会输出最后一天，所以注释掉

    now = time(0);
    dt = ctime(&now);
    std::cout<<"end time:"<<dt<<std::endl;

return 0;

}

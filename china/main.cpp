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
#include <cstdlib>
#include <filesystem>

int main(int argc, char* argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    time_t now = time(0);
    char *dt = ctime(&now);
    std::cout<<"begin time:"<<dt<<std::endl;

//-----------------------------------------------
// huabei area: 1km: width 400-500 180-280 *25
//-----------------------------------------------
    const char* configuredInput = std::getenv("STREAM_INPUT_FILE");
    std::string infilepath = argc > 1
        ? argv[1]
        : (configuredInput ? configuredInput : "/home/hero/data/stream/input-1km-china-local.txt");

    if (!std::filesystem::is_regular_file(infilepath)) {
        std::cerr << "[STREAM] input configuration not found: " << infilepath << std::endl;
        std::cerr << "usage: " << argv[0] << " /absolute/path/to/input.txt" << std::endl;
        return 2;
    }

    std::cout << "[STREAM] input=" << infilepath << std::endl;

    try {
        Engine engine;
        engine.initProject(infilepath);
        engine.run();
    } catch (const std::exception& error) {
        std::cerr << "[STREAM] fatal error: " << error.what() << std::endl;
        return 1;
    }
    // engine.observe(-1); //这里的observe只会输出最后一天，所以注释掉

    now = time(0);
    dt = ctime(&now);
    std::cout<<"end time:"<<dt<<std::endl;

return 0;

}






 
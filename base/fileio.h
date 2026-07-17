#pragma once

#include "structs.h"
#include "defined.h"
#include "modelio.h"
#include <iomanip>
#include <filesystem>

//----------------------------------
//--- data and run
//----------------------------------

class FileIO
{
public:
    FileIO(){};
    void readMeta(std::string infilepath);
    void readGeodata();
    void readExtradata();
    int readMeteodata(int year, int doy);
    int readSatdata(int year, int doy);
    void readVegdata(int year, int doy);
    void saveSkt(int year, int doy, int knode);
    void saveDBT(int year, int doy, int knode);
    void readDefined(std::shared_ptr<ModelIO> &modelio);
    void varupdate();
    void varoutput();
    void destroy();


public:


    // map info, the dimension should be the same with input and output

    // std::string proj; // projection info
    // std::string geog; // geometric info

    double trans_meteo[6];
    std::string proj_meteo;

    double trans_global[6];
    std::string proj_global;

    double trans_regional[6];
    std::string proj_regional;


    double trans_sat[6];
    std::string proj_sat;

    // 图像范围长度相关
    int m_width;  // global range
    int m_height; // global range
    int m_node; // time node

    int m_width_region; // regional range
    int m_height_region; // regional range

    int m_width_sat; // satellite range
    int m_height_sat; // satellite range

    int m_width_meteo; // meteo range
    int m_height_meteo; // meteo range

    // int m_workwidth;
    // int m_workheight;
    int m_islai;
    int m_issm;
    int m_areakey;
    int m_startwidth;
    int m_endwidth;
    int m_startheight;
    int m_endheight;

    // 计算得到global->era5和sat->global的比例
    float step_global2meteo;
    float step_global2sat;

    int m_issubtype;
    int m_isvcmax;
    int m_satmode;

    //图像范围经纬度相关
    float m_startlat_meteo;
    float m_endlat_meteo;
    float m_startlon_meteo;
    float m_endlon_meteo;
    float m_startlat;
    float m_endlat;
    float m_startlon;
    float m_endlon;
    float m_startlat_region;
    float m_endlat_region;
    float m_startlon_region;
    float m_endlon_region;
    float m_startlat_sat;
    float m_endlat_sat;
    float m_startlon_sat;
    float m_endlon_sat;

    std::string m_inputDir;
    std::string m_outputDir;
    std::string m_geoinfoDir;
    std::string m_definedDir;
    std::string m_demfile;
    std::string m_typefile;
    std::string m_latfile;
    std::string m_lonfile;
    std::string m_era5file;

    std::string m_tfile;
    std::string m_tafile;
    std::string m_eafile;
    std::string m_ufile;
    std::string m_Rinfile;
    std::string m_Rlifile;
    std::string m_pfile;

    std::string m_smfile;
    std::string m_laifile;
    std::string m_laidir;
    std::string m_smdir;
    std::string m_smfile1;
    std::string m_laifile1;
    std::string m_subtypefile;
    std::string m_globaldir;
    std::string m_vcmaxfile;
    std::string m_satdir;
    std::string m_satName;
    std::string m_nadirobliq;

    // property image
    std::vector<float> m_vLat;
    std::vector<float> m_vLon;
    std::vector<float> m_vDem;
    std::vector<float> m_vType;
    std::vector<float> m_meteoType;

    // static image
    std::vector<float> m_vLai;   // wind speed
    std::vector<float> m_vSM;
    std::vector<float> m_subType;
    std::vector<float> m_regionType;
    std::vector<float> m_Vcmax;

    // dynamic image
    std::vector<std::vector<float>> m_vU;   // wind speed
    std::vector<std::vector<float>> m_vRin;
    std::vector<std::vector<float>> m_vRli;

    // satllite tif
    std::vector<float> m_vza;
    std::vector<float> m_sza;
    std::vector<float> m_vaa;
    std::vector<float> m_saa;
    std::vector<float> m_time;
    std::vector<float> m_treedensity;
    std::vector<float> m_canopyheight;
    std::vector<float> m_emis_v;
    std::vector<float> m_emis_s;

//    std::vector<std::vector<float>> m_vT;
    std::vector<std::vector<float>> m_vEa;
    std::vector<std::vector<float>> m_vP;
    std::vector<std::vector<float>> m_vCa;
    std::vector<std::vector<float>> m_vOa;
    std::vector<std::vector<float>> m_vTa;
    std::vector<uint32_t> m_vPos;
    std::vector<float> m_vT;



    std::vector<std::vector<float>> m_vTsk;
    std::vector<std::vector<float>> m_vTss;
    std::vector<std::vector<float>> m_vTsh;
    std::vector<std::vector<float>> m_vTcs;
    std::vector<std::vector<float>> m_vTch;

    std::vector<std::vector<float>> m_vTroofsunlit;
    std::vector<std::vector<float>> m_vTroofshaded;
    std::vector<std::vector<float>> m_vTwallsunlit;
    std::vector<std::vector<float>> m_vTwallshaded;
    std::vector<std::vector<float>> m_vTstreetsunlit;
    std::vector<std::vector<float>> m_vTstreetshaded;

    std::vector<std::vector<float>> m_vDBT;
//    std::vector<float> m_vTsk;
    int startYear,endYear,startDoy,endDoy;


};





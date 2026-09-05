#include "fileio.h"
#include <algorithm>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {
bool ensureOutputDirectory(const std::string& path)
{
    std::error_code error;
    if (std::filesystem::exists(path, error)) {
        if (error || !std::filesystem::is_directory(path, error)) {
            std::cerr << "Output path is not a directory: " << path << std::endl;
            return false;
        }
        return true;
    }

    if (!std::filesystem::create_directories(path, error) &&
        !std::filesystem::is_directory(path, error)) {
        std::cerr << "Unable to create output directory " << path
                  << ": " << error.message() << std::endl;
        return false;
    }
    return true;
}

std::string trim(const std::string& value)
{
    const auto first = std::find_if_not(value.begin(), value.end(),
                                        [](unsigned char c) { return std::isspace(c); });
    const auto last = std::find_if_not(value.rbegin(), value.rend(),
                                       [](unsigned char c) { return std::isspace(c); }).base();
    if (first >= last) return {};
    return {first, last};
}

std::vector<std::string> splitCsv(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;
    for (char c : line) {
        if (c == '"') {
            quoted = !quoted;
        } else if (c == ',' && !quoted) {
            fields.push_back(trim(field));
            field.clear();
        } else {
            field.push_back(c);
        }
    }
    fields.push_back(trim(field));
    return fields;
}

bool parseInt(const std::string& text, int& result)
{
    const std::string value = trim(text);
    if (value.empty()) return false;
    char* end = nullptr;
    const long parsed = std::strtol(value.c_str(), &end, 10);
    if (end == value.c_str() || *end != '\0') return false;
    result = static_cast<int>(parsed);
    return true;
}

bool parseFloat(const std::string& text, float& result)
{
    const std::string value = trim(text);
    if (value.empty()) return false;
    char* end = nullptr;
    const float parsed = std::strtof(value.c_str(), &end);
    if (end == value.c_str() || *end != '\0') return false;
    result = parsed;
    return true;
}

bool parseDouble(const std::string& text, double& result)
{
    const std::string value = trim(text);
    if (value.empty()) return false;
    char* end = nullptr;
    const double parsed = std::strtod(value.c_str(), &end);
    if (end == value.c_str() || *end != '\0') return false;
    result = parsed;
    return true;
}

int environmentInt(const char* name, int fallback)
{
    const char* value = std::getenv(name);
    int parsed = 0;
    return value != nullptr && parseInt(value, parsed) ? parsed : fallback;
}

float environmentFloat(const char* name, float fallback)
{
    const char* value = std::getenv(name);
    float parsed = 0.0f;
    return value != nullptr && parseFloat(value, parsed) ? parsed : fallback;
}

unsigned int environmentUnsigned(const char* name, unsigned int fallback)
{
    const char* value = std::getenv(name);
    int parsed = 0;
    if (value == nullptr || !parseInt(value, parsed) || parsed < 0) return fallback;
    return static_cast<unsigned int>(parsed);
}

bool environmentBool(const char* name, bool fallback)
{
    const char* value = std::getenv(name);
    if (value == nullptr) return fallback;
    const std::string parsed = trim(value);
    if (parsed == "1" || parsed == "true" || parsed == "TRUE" || parsed == "yes") {
        return true;
    }
    if (parsed == "0" || parsed == "false" || parsed == "FALSE" || parsed == "no") {
        return false;
    }
    return fallback;
}

StateSource parseStateSource(const std::string& raw, StateSource fallback)
{
    std::string value = trim(raw);
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (value == "0" || value == "observation" || value == "observed" ||
        value == "measurement" || value == "measured" || value == "era5") {
        return StateSource::Observation;
    }
    if (value == "1" || value == "model" || value == "simulation" ||
        value == "simulated") {
        return StateSource::Model;
    }
    if (value == "2" || value == "assimilation" || value == "assimilated" ||
        value == "analysis" || value == "enkf") {
        return StateSource::Assimilation;
    }
    return fallback;
}

StateSource environmentStateSource(const char* primary, const char* alias,
                                   StateSource fallback)
{
    const char* value = std::getenv(primary);
    if (value == nullptr || *value == '\0') value = std::getenv(alias);
    return value == nullptr ? fallback : parseStateSource(value, fallback);
}

const char* stateSourceName(StateSource source)
{
    switch (source) {
    case StateSource::Model: return "model";
    case StateSource::Assimilation: return "assimilation";
    case StateSource::Observation:
    default: return "measurement/ERA5";
    }
}

std::string replaceToken(std::string value, const std::string& token,
                         const std::string& replacement)
{
    std::size_t position = 0;
    while ((position = value.find(token, position)) != std::string::npos) {
        value.replace(position, token.size(), replacement);
        position += replacement.size();
    }
    return value;
}

std::string resolvePrecipitationPath(const std::string& configured,
                                     const std::string& inputDir,
                                     int year, int doy)
{
    std::ostringstream day;
    day << std::setw(3) << std::setfill('0') << doy;
    const std::string yearText = std::to_string(year);
    const std::string dayText = day.str();

    std::string path = configured;
    path = replaceToken(path, "{year}", yearText);
    path = replaceToken(path, "{YYYY}", yearText);
    path = replaceToken(path, "{doy}", dayText);
    path = replaceToken(path, "{DDD}", dayText);

    std::error_code error;
    if (std::filesystem::is_directory(path, error)) {
        if (!path.empty() && path.back() != '/') path.push_back('/');
        path += yearText + "/" + dayText + "/precipitation.h5";
    } else if (!path.empty() && path.back() == '/') {
        path += yearText + "/" + dayText + "/precipitation.h5";
    }

    // A relative setting is interpreted relative to the project input root
    // when the direct path does not exist.
    if (!std::filesystem::exists(path, error) && !inputDir.empty() &&
        !std::filesystem::path(path).is_absolute()) {
        std::string candidate = inputDir;
        if (candidate.back() != '/') candidate.push_back('/');
        candidate += path;
        if (std::filesystem::exists(candidate, error)) path = candidate;
    }
    return path;
}
}

void FileIO::readMeta(std::string infilepath)
{
     //std::string infilename="/home/airt/work/Stream/project/fileInput.txt";

   // m_projectDir = Utils::getDirectoryPath(infilepath) + "/";

     std::string line;  // 用于存储每行读取的内容
     std::vector<std::string> fields;  // 用于存储分割后的字段
     std::string deli(" ");  // 分隔符，默认为空格
     std::ifstream infile(infilepath.c_str());
     if(infile.is_open())
     {
         // 逐行读取文件内容，并解析相关配置

         // 第一段：读取输入目录及相关子目录
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_inputDir = fields[0];
         m_definedDir = m_inputDir + "/defined/";
         m_geoinfoDir = m_inputDir +"/geoinfo/";

         // 第二段：读取输出目录
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_outputDir = fields[0];

         if (const char* outputOverride = std::getenv("STREAM_OUTPUT_DIR");
             outputOverride != nullptr && *outputOverride != '\0') {
             m_outputDir = outputOverride;
         }
         if (!m_outputDir.empty() && m_outputDir.back() != '/' &&
             m_outputDir.back() != '\\') {
             m_outputDir.push_back('/');
         }

         // 第三段：读取 DEM 文件路径
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_demfile = fields[0];

         // 第四段：读取地表类型文件路径
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_era5file = fields[0];
         m_typefile = fields[1];

         // 第五段：读取纬度和经度文件路径
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_latfile = fields[0];
         m_lonfile = fields[1];

         // 第六段：读取多个气象文件路径
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_tfile = fields[0]; //t 表示一天中的时间点，以小数形式表示一天中的比例
         m_tafile = fields[1]; //空气温度
         m_eafile = fields[2]; //空气湿度
         m_pfile = fields[3]; //气压
         m_ufile = fields[4]; //风速
         m_Rinfile = fields[5]; //入射短波辐射
         m_Rlifile = fields[6]; //入射长波辐射
         m_laifile = fields[7];
         m_smfile = fields[8];

         // 第九段：读取开始和结束日期
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         startYear = atoi(fields[0].c_str());
         startDoy = atoi(fields[1].c_str());
         endYear = atoi(fields[2].c_str());
         endDoy = atoi(fields[3].c_str());

         // 第十段：读取区域相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_areakey = atoi(fields[0].c_str());
         m_startwidth = atoi(fields[1].c_str());
         m_endwidth = atoi(fields[2].c_str());
         m_startheight = atoi(fields[3].c_str());
         m_endheight = atoi(fields[4].c_str());


         // 第七段：读取 LAI 相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_islai = atoi(fields[0].c_str()); //判断是否存在lai文件，如果有则使用后续文件设置路径
         // m_laifile = fields[1];
         m_laidir = fields[1];
         m_laifile1 = fields[2];

         // 第八段：读取土壤水分相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_issm = atoi(fields[0].c_str());
         m_smdir = fields[1];
         m_smfile1 = fields[2];


         // 第十一段：读取全局辅助数据相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_globaldir = fields[0];

         // 第十二段：读取地表子分类相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_issubtype = atoi(fields[0].c_str()); //判断是否存在子类别文件，如果有则使用后续文件设置路径
         m_subtypefile = fields[1];

         // 第十三段：读取最大羧化速率相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_isvcmax = atoi(fields[0].c_str()); //判断是否存在子类别文件，如果有则使用后续文件设置路径
         m_vcmaxfile = fields[1];

         m_balanceEnabled = environmentBool("STREAM_BALANCE_ENABLED", false);
         m_balanceDtSeconds = environmentFloat("STREAM_BALANCE_DT_SECONDS", 3600.0f);
         m_balanceRootDepthMeters = environmentFloat("STREAM_BALANCE_ROOT_DEPTH_M", 1.0f);
         m_balanceSpecificLeafArea = environmentFloat("STREAM_BALANCE_SLA", 0.02f);
         m_balanceLeafCarbonFraction = environmentFloat("STREAM_BALANCE_LEAF_CARBON_FRACTION", 0.45f);
         m_balanceLeafAllocation = environmentFloat("STREAM_BALANCE_LEAF_ALLOCATION", 0.40f);
         m_balanceLeafTurnover = environmentFloat("STREAM_BALANCE_LEAF_TURNOVER", 0.01f);
         m_precipitationFile = std::getenv("STREAM_PRECIPITATION_FILE") != nullptr
                                   ? std::getenv("STREAM_PRECIPITATION_FILE") : "";
         m_laiSource = environmentStateSource("STREAM_LAI_SOURCE", "STREAM_STATE_SOURCE",
                                              StateSource::Observation);
         m_soilMoistureSource = environmentStateSource(
             "STREAM_SOIL_MOISTURE_SOURCE", "STREAM_SM_SOURCE", StateSource::Observation);
         std::cout << "Water/carbon balance "
                   << (m_balanceEnabled ? "enabled" : "disabled") << std::endl;
         std::cout << "State sources: LAI=" << stateSourceName(m_laiSource)
                   << ", soil moisture=" << stateSourceName(m_soilMoistureSource)
                   << std::endl;
         // Optional EnKF configuration.  It is intentionally configured
         // through environment variables so existing metadata files remain
         // compatible.  The filter is disabled unless explicitly enabled.
         const char* enkfEnabled = std::getenv("STREAM_ENKF_ENABLED");
         m_assimilationEnabled = enkfEnabled != nullptr &&
                                 (std::string(enkfEnabled) == "1" ||
                                  std::string(enkfEnabled) == "true" ||
                                  std::string(enkfEnabled) == "TRUE");
         m_assimilationReadOnly = !m_assimilationEnabled;
         m_enkf.configure(
             environmentInt("STREAM_ENKF_ENSEMBLE_SIZE", 20),
             environmentFloat("STREAM_ENKF_LAI_OBS_ERROR", 0.30f),
             environmentFloat("STREAM_ENKF_SM_OBS_ERROR", 0.05f),
             environmentFloat("STREAM_ENKF_LAI_PROCESS_NOISE", 0.05f),
             environmentFloat("STREAM_ENKF_SM_PROCESS_NOISE", 0.01f),
             environmentUnsigned("STREAM_ENKF_SEED", 20250906U));

         // Optional observation archive.  It is always read-only; the EnKF
         // itself currently consumes the configured LAI and SM rasters below.
         m_assimilationFile.clear();
         m_assimilationObservations.clear();
         if (const char* assimilationPath = std::getenv("STREAM_ASSIMILATION_FILE");
             assimilationPath != nullptr && *assimilationPath != '\0') {
             m_assimilationFile = assimilationPath;
             readAssimilationData(m_assimilationFile);
         } else {
             std::cout << "Assimilation observation archive not configured" << std::endl;
         }
         std::cout << "EnKF " << (m_assimilationEnabled ? "enabled" : "disabled")
                   << ", LAI/SM observations remain available for assimilation" << std::endl;
         if (!m_assimilationEnabled &&
             (m_laiSource == StateSource::Assimilation ||
              m_soilMoistureSource == StateSource::Assimilation)) {
             std::cout << "Assimilation source requested while EnKF is disabled; "
                          "that variable will fall back to measurement/ERA5" << std::endl;
         }

         // 第十四段：读取卫星观测方向亮温相关配置
         // getline(infile,line);
         // getline(infile,line);
         // fields = Utils::splitt(line,deli);
         // m_satmode = atoi(fields[0].c_str()); //判断是否运行卫星观测模拟并设置，0为不进行卫星观测模拟，1为先模拟研究区域再搜索卫星观测像元位置，2为先搜索卫星观测像元位置再进行模拟
         // m_satdir = fields[1];
         // m_satName = fields[2];
         // m_nadirobliq = fields[3];


         std::cout<<"Finish Reading Meta"<<std::endl;

     }else std::cout<<"Unable to open the fileinput "<<std::endl;
     infile.close();
}

bool FileIO::readAssimilationData(const std::string& filepath)
{
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "Unable to open assimilation observation archive: "
                  << filepath << std::endl;
        return false;
    }

    m_assimilationObservations.clear();
    std::string line;
    std::size_t lineNumber = 0;
    std::size_t invalidRows = 0;
    while (std::getline(infile, line)) {
        ++lineNumber;
        const std::string cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') continue;

        const std::vector<std::string> fields = splitCsv(cleaned);
        // Schema: year,doy,hour,variable,value,uncertainty,row,col,window_hours,quality
        if (fields.size() < 5) {
            ++invalidRows;
            continue;
        }

        AssimilationObservation observation;
        double hour = 0.0;
        if (!parseInt(fields[0], observation.year) ||
            !parseInt(fields[1], observation.doy) ||
            !parseDouble(fields[2], hour) ||
            !parseFloat(fields[4], observation.value) ||
            observation.year < 1 || observation.doy < 1 || observation.doy > 366 ||
            hour < 0.0 || hour >= 24.0 || !std::isfinite(observation.value)) {
            // A header row is expected and is ignored without a warning.
            if (lineNumber != 1) ++invalidRows;
            continue;
        }
        observation.hour = hour;
        observation.variable = trim(fields[3]);
        if (observation.variable.empty()) {
            ++invalidRows;
            continue;
        }

        if (fields.size() > 5 && !fields[5].empty() &&
            !parseFloat(fields[5], observation.uncertainty)) {
            ++invalidRows;
            continue;
        }
        if (!std::isfinite(observation.uncertainty) || observation.uncertainty <= 0.0f) {
            ++invalidRows;
            continue;
        }
        if (fields.size() > 6 && !fields[6].empty() &&
            !parseInt(fields[6], observation.row)) {
            ++invalidRows;
            continue;
        }
        if (fields.size() > 7 && !fields[7].empty() &&
            !parseInt(fields[7], observation.col)) {
            ++invalidRows;
            continue;
        }
        if (fields.size() > 8 && !fields[8].empty() &&
            !parseDouble(fields[8], observation.windowHours)) {
            ++invalidRows;
            continue;
        }
        if (fields.size() > 9 && !fields[9].empty() &&
            !parseInt(fields[9], observation.quality)) {
            ++invalidRows;
            continue;
        }
        if (observation.windowHours < 0.0 || observation.quality < 0) {
            ++invalidRows;
            continue;
        }
        m_assimilationObservations.push_back(observation);
    }

    std::sort(m_assimilationObservations.begin(), m_assimilationObservations.end(),
              [](const AssimilationObservation& lhs, const AssimilationObservation& rhs) {
                  if (lhs.year != rhs.year) return lhs.year < rhs.year;
                  if (lhs.doy != rhs.doy) return lhs.doy < rhs.doy;
                  return lhs.hour < rhs.hour;
              });

    std::cout << "Read-only assimilation observations: "
              << m_assimilationObservations.size() << " records from " << filepath;
    if (invalidRows > 0) std::cout << " (ignored " << invalidRows << " invalid rows)";
    std::cout << std::endl;
    return true;
}

void FileIO::assimilatePixelState(int pixelIndex,
                                  float laiObservation,
                                  float soilMoistureObservation,
                                  float laiForecast,
                                  float soilMoistureForecast,
                                  float& laiAnalysis,
                                  float& soilMoistureAnalysis)
{
    if (!m_assimilationEnabled) {
        laiAnalysis = laiForecast;
        soilMoistureAnalysis = soilMoistureForecast;
        return;
    }

    // Keep missing/sentinel product values from becoming a zero-valued
    // initial state.  The defaults are only used when the corresponding
    // forecast is invalid; they are not observations.
    const float safeLaiForecast = std::isfinite(laiForecast) &&
                                  laiForecast >= 0.0f && laiForecast <= 25.0f
                                      ? laiForecast : 0.0f;
    const float safeSoilMoistureForecast = std::isfinite(soilMoistureForecast) &&
                                           soilMoistureForecast >= 0.0f &&
                                           soilMoistureForecast <= 1.0f
                                               ? soilMoistureForecast : 0.2f;
    const bool hasLaiObservation = std::isfinite(laiObservation) &&
                                   laiObservation >= 0.0f && laiObservation <= 25.0f;
    const bool hasSoilMoistureObservation = std::isfinite(soilMoistureObservation) &&
                                            soilMoistureObservation >= 0.0f &&
                                            soilMoistureObservation <= 1.0f;
    m_enkf.assimilate(pixelIndex, safeLaiForecast, safeSoilMoistureForecast,
                      laiObservation, soilMoistureObservation,
                      hasLaiObservation, hasSoilMoistureObservation,
                      laiAnalysis, soilMoistureAnalysis);
}

void FileIO::readGeodata()
{

    int width, height, nband;

    // 读取ERA5数据的地理范围和投影信息数据
    std::string infilename0= m_geoinfoDir + m_era5file;
    Utils::readImageinout11(infilename0,m_meteoType,m_width_meteo,m_height_meteo,nband, trans_meteo, proj_meteo);
    std::cout<<"read meteoType.tif"<<std::endl;

    // 读取全局的地理范围和投影信息数据
    std::string infilename1= m_geoinfoDir + m_latfile;
    Utils::readImageinout1(infilename1,m_vLat,width,height,nband);
    std::cout<<"read lat.tif"<<std::endl;

    std::string infilename2= m_geoinfoDir + m_lonfile;
    Utils::readImageinout1(infilename2,m_vLon,width,height,nband);
    std::cout<<"read lon.tif"<<std::endl;

    std::string infilename3= m_geoinfoDir + m_demfile;
    Utils::readImageinout1(infilename3,m_vDem,width,height,nband);
    std::cout<<"read dem.tif"<<std::endl;

    std::string infilename4= m_geoinfoDir + m_typefile;
    // Utils::readImageinout1(infilename3,m_vType,m_width,m_height,nband);
    Utils::readImageinout11(infilename4,m_vType,m_width,m_height,nband, trans_global, proj_global);
    std::cout<<"read type.tif"<<std::endl;

}

void FileIO::readExtradata()
{

    //-------------------------------------------------------
    //------ 读取研究区域的辅助数据，包括子地表类型，Vcmax等
    //-------------------------------------------------------
    int width_global;
    int height_global;
    int nband_global;
    double trans_global[6];
    std::string proj_global;

    if(m_issubtype == 0) {
        std::cout<<"No subtype file exist"<<std::endl;
    }else
    {
        std::string infilename4 = m_globaldir + m_subtypefile;
        Utils::readImageinout11(infilename4, m_subType, width_global, height_global, nband_global, trans_global, proj_global);
        std::cout<<"read cropType.tif"<<std::endl;
    }

    // if(m_isvcmax == 0) {
    //     std::cout<<"No vcmax file exist"<<std::endl;
    // }else
    // {
    //     std::string infilename5 = m_globaldir + m_vcmaxfile;
    //     Utils::readImageinout11(infilename5, m_Vcmax, width_global, height_global, nband_global, trans_global, proj_global);
    //     std::cout<<"read vcmax.tif"<<std::endl;
    // }

    if(m_satmode == 0) {
        std::cout<<"No satellite file exist"<<std::endl;
    }else
    {
        std::string infileCanopyHeight = m_globaldir + "treeCanopyheight_China.tif";
        std::string infileTreeDensity = m_globaldir + "treeDensity_China.tif";
        std::string infileemis_s = m_globaldir + "emis_s_" + m_satName + "_China.tif";
        std::string infileemis_v = m_globaldir + "emis_v_" + m_satName + "_China.tif";

        Utils::readImageinout1(infileCanopyHeight, m_canopyheight, width_global, height_global, nband_global);
        Utils::readImageinout1(infileTreeDensity, m_treedensity, width_global, height_global, nband_global);
        Utils::readImageinout1(infileemis_s, m_emis_s, width_global, height_global, nband_global);
        Utils::readImageinout1(infileemis_v, m_emis_v, width_global, height_global, nband_global);

    }

}


int FileIO::readMeteodata(int year, int doy) {

    int width,height,nband;
    //--------------------
    //--- Read structure and meteo data
    //--------------------

    std::ostringstream  oss_doy;
    std::string year_str = std::to_string(year);
    oss_doy << std::setw(3)<<std::setfill('0')<<doy;
    std::string m_wdirnew = m_inputDir + "/force/"+year_str + "/" + oss_doy.str() + "/";


    //-------------------------------------------------------
    //------ 1440 * 720 <-> 36000*18000
    //-------------------------------------------------------

    if(m_islai == 0) {
        std::string infilename4 = m_wdirnew + m_laifile;
        Utils::readImageinout1(infilename4, m_vLai, width, height, nband);
    }else if(m_islai == 1)
    {
        int ind_doy = (doy-1)/4;
        int stddoy = ind_doy*4+1;
        std::ostringstream  oss_doy_std;
        oss_doy_std << std::setw(3)<<std::setfill('0')<<stddoy;
        std::string infilename4 = m_laidir + year_str + "/LAI_" + year_str + oss_doy_std.str() + ".h5";
        Utils::readHdf5image1(infilename4, "data", m_vLai, width, height, nband);

    }else if(m_islai ==2) {
        std::string infilename5 = m_laifile1;
        Utils::readImageinout1(infilename5,m_vLai, width, height, nband);


    }


    //-------------------------------------------------------
    //------ 1440 * 720 <-> 36000*18000
    //-------------------------------------------------------

    if(m_issm == 0) {
        std::string infilename12= m_wdirnew + m_smfile;
        Utils::readImageinout1(infilename12,m_vSM,width,height,nband);

    }else if(m_issm == 1)
    {
        std::cout<<"soil moisture error!!!"<<std::endl;
    }

    //-------------------------------------------------------
    //------ 1440 * 720 <-> 36000*18000
    //-------------------------------------------------------

    std::string infilename13 = m_wdirnew + m_tfile;
    Utils::readascfileinout(infilename13,0,0,m_vT,m_node);
    std::cout<<"read t.txt"<<std::endl;

    std::string infilename7= m_wdirnew + m_ufile;
    Utils::readHdf5image(infilename7,"data",m_vU,width,height,nband);
    std::cout<<"read u.h5"<<std::endl;

    std::string infilename6= m_wdirnew + m_tafile;
    Utils::readHdf5image(infilename6,"data",m_vTa,m_width_meteo,m_height_meteo,nband);
    std::cout<<"read ta.h5"<<std::endl;

    std::string infilename66= m_wdirnew + m_eafile;
    Utils::readHdf5image(infilename66,"data",m_vEa,width,height,nband);
    std::cout<<"read ea.h5"<<std::endl;

    std::string infilename8= m_wdirnew + m_Rinfile;
    Utils::readHdf5image(infilename8,"data",m_vRin,width,height,nband);
    std::cout<<"read rin.h5"<<std::endl;

    std::string infilename9= m_wdirnew + m_Rlifile;
    Utils::readHdf5image(infilename9,"data",m_vRli,width,height,nband);
    std::cout<<"read rli.h5"<<std::endl;

    std::string infilename10= m_wdirnew + m_pfile;
    Utils::readHdf5image(infilename10,"data",m_vP,width,height,nband);
    std::cout<<"read p.h5"<<std::endl;

    if (m_isvcmax == 1) {
        std::vector<float> m_vcmax1, m_vcmax2;
        const int base_doy = 1, doy_interval = 8;
        const int vcmax_doy1 = base_doy + ((doy - base_doy) / doy_interval) * doy_interval;
        const int vcmax_doy2 = std::min(vcmax_doy1 + doy_interval, 365);

        // Calculate weights
        const bool exact1 = (doy == vcmax_doy1), exact2 = (doy == vcmax_doy2);
        const float weight1 = exact1 ? 1.0f : exact2 ? 0.0f :
                             (1.0f / fabsf(doy - vcmax_doy1));
        const float weight2 = exact2 ? 1.0f : exact1 ? 0.0f :
                             (1.0f / fabsf(doy - vcmax_doy2));
        const float norm = 1.0f / (weight1 + weight2);

        // Read and interpolate
        auto readVcmax = [&](int day) {
            return m_globaldir + "vcmax/" + std::to_string(year) + "/InterpVcmax.A" +
                   std::to_string(year) + "001.Vcmax" +
                   (day < 100 ? (day < 10 ? "00" : "0") : "") + std::to_string(day) + ".tif";
        };
        Utils::readImageinout1(readVcmax(vcmax_doy1), m_vcmax1, width, height, nband);
        Utils::readImageinout1(readVcmax(vcmax_doy2), m_vcmax2, width, height, nband);

        // Interpolate
        m_Vcmax.resize(m_vcmax1.size());
        std::transform(m_vcmax1.begin(), m_vcmax1.end(), m_vcmax2.begin(), m_Vcmax.begin(),
            [=](float a, float b) { return (weight1 * a + weight2 * b) * norm; });
    }

    readPrecipitationData(year, doy);

    return 0;
}

bool FileIO::readPrecipitationData(int year, int doy)
{
    m_vPrecipitation.clear();
    m_precipitationWidth = 0;
    m_precipitationHeight = 0;
    m_precipitationAvailable = false;

    if (m_precipitationFile.empty()) return false;

    const std::string path = resolvePrecipitationPath(m_precipitationFile,
                                                      m_inputDir, year, doy);
    int bands = 0;
    if (!Utils::readHdf5image(path, "data", m_vPrecipitation,
                              m_precipitationWidth, m_precipitationHeight, bands)) {
        std::cerr << "Precipitation file unavailable for " << year << " " << doy
                  << ": " << path << "; using P=0" << std::endl;
        m_vPrecipitation.clear();
        return false;
    }

    m_precipitationAvailable = m_precipitationWidth > 0 &&
                               m_precipitationHeight > 0 && bands > 0;
    if (m_precipitationAvailable) {
        std::cout << "Read precipitation forcing: " << path << std::endl;
    }
    return m_precipitationAvailable;
}

int FileIO::readSatdata(int year, int doy)
{
    //--------------------
    //--- Read structure and sat data
    //--------------------

    std::ostringstream oss_doy;
    std::string year_str = std::to_string(year);
    oss_doy << std::setw(3) << std::setfill('0') << doy;
    std::string m_wdirnew = m_inputDir + year_str + "/" + oss_doy.str() + "/";

    int width_sat;
    int height_sat;
    int nband_sat;

    std::string indir_sat = m_satdir + m_satName + "/tif/";
    if ((m_satName == "S3A") or (m_satName == "S3B"))
    {
        std::string infile_vza = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_vza_" + m_nadirobliq +
            ".tif";
        std::string infile_sza = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_sza_" + m_nadirobliq +
            ".tif";
        std::string infile_vaa = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_vaa_" + m_nadirobliq +
            ".tif";
        std::string infile_saa = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_saa_" + m_nadirobliq +
            ".tif";
        std::string infile_time = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_time.tif";
        Utils::readImageinout11(infile_vza, m_vza, m_width_sat, m_height_sat, nband_sat, trans_sat, proj_sat);
        Utils::readImageinout1(infile_sza, m_sza, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_vaa, m_vaa, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_saa, m_saa, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_time, m_time, width_sat, height_sat, nband_sat);
    }
    else
    {
        std::string infile_vza = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_vza_" + ".tif";
        std::string infile_sza = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_sza_" + ".tif";
        std::string infile_vaa = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_vaa_" + ".tif";
        std::string infile_saa = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_saa_" + ".tif";
        std::string infile_time = indir_sat + m_satName + "_" + year_str + oss_doy.str() + "_day_time.tif";
        Utils::readImageinout11(infile_vza, m_vza, m_width_sat, m_height_sat, nband_sat, trans_sat, proj_sat);
        Utils::readImageinout1(infile_sza, m_sza, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_vaa, m_vaa, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_saa, m_saa, width_sat, height_sat, nband_sat);
        Utils::readImageinout1(infile_time, m_time, width_sat, height_sat, nband_sat);
    }

    return 0;
}


void FileIO::saveSkt(int year, int doy, int knode) {

    std::ostringstream  oss;
    std::string yearstr = std::to_string(year);
    oss << std::setw(3)<<std::setfill('0')<<doy;
    std::string m_wdirnew = m_outputDir + yearstr + "/" + oss.str() + "/";

    if (!ensureOutputDirectory(m_wdirnew)) return;

    int height = m_height_region;
    int width = m_width_region;
    double trans[6];
    int n = m_node;
    if(knode >= 0) n = 1;
    std::string proj = "";

    Utils::writeHdf5image(m_wdirnew+"skt_sim.h5","data",m_vTsk, width,height,n);
    Utils::writeHdf5image(m_wdirnew+"tss_sim.h5","data",m_vTss, width,height,n);
    Utils::writeHdf5image(m_wdirnew+"tsh_sim.h5","data",m_vTsh, width,height,n);
    Utils::writeHdf5image(m_wdirnew+"tcs_sim.h5","data",m_vTcs, width,height,n);
    Utils::writeHdf5image(m_wdirnew+"tch_sim.h5","data",m_vTch, width,height,n);

    // Utils::writeHdf5image(m_wdirnew+"troofsunlit_sim.h5","data",m_vTroofsunlit, width,height,n);
    // Utils::writeHdf5image(m_wdirnew+"troofshaded_sim.h5","data",m_vTroofshaded, width,height,n);
    // Utils::writeHdf5image(m_wdirnew+"twallsunlit_sim.h5","data",m_vTwallsunlit, width,height,n);
    // Utils::writeHdf5image(m_wdirnew+"twallshaded_sim.h5","data",m_vTwallshaded, width,height,n);
    // Utils::writeHdf5image(m_wdirnew+"tstreetsunlit_sim.h5","data",m_vTstreetsunlit, width,height,n);
    // Utils::writeHdf5image(m_wdirnew+"tstreetshaded_sim.h5","data",m_vTstreetshaded, width,height,n);

}

void FileIO::saveDBT(int year, int doy, int knode) {

    std::ostringstream  oss;
    std::string yearstr = std::to_string(year);
    oss << std::setw(3)<<std::setfill('0')<<doy;
    std::string m_wdirnew = m_outputDir + yearstr + "/" + oss.str() + "/";

    if (!ensureOutputDirectory(m_wdirnew)) return;

    int height = m_height_region;
    int width = m_width_region;
    double trans[6];
    int n = m_node;
    if(knode >= 0) n = 1;
    std::string proj = "";

    std::string satName = m_satName;
//    Utils::saveGeoImage(m_wdirnew + satName + "_dbt_sim.tif", m_vDBT, width, height, n, proj_sat, trans_sat);

    Utils::writeHdf5image(m_wdirnew + satName + "_dbt_sim.h5","data",m_vDBT, width,height,n);
    Utils::saveGeoImage(m_wdirnew + satName + "_dbt_sim.tif", m_vDBT,
                         width, height, n, proj_sat, trans_sat);
}

void FileIO::saveOptical(int year, int doy, int knode) {
    std::ostringstream oss;
    std::string yearstr = std::to_string(year);
    oss << std::setw(3) << std::setfill('0') << doy;
    std::string m_wdirnew = m_outputDir + yearstr + "/" + oss.str() + "/";

    if (!ensureOutputDirectory(m_wdirnew)) return;

    int width = m_width_region;
    int height = m_height_region;
    int n = m_node;
    if (knode >= 0) n = 1;

    // Integrated visible/near-infrared radiation components.
    Utils::writeHdf5image(m_wdirnew + "direct_vrad_leaf_sim.h5", "data",
                          m_vDirectVradLeaf, width, height, n);
    Utils::writeHdf5image(m_wdirnew + "diffuse_vrad_leaf_sim.h5", "data",
                          m_vDiffuseVradLeaf, width, height, n);
    Utils::writeHdf5image(m_wdirnew + "direct_vrad_soil_sim.h5", "data",
                          m_vDirectVradSoil, width, height, n);
    Utils::writeHdf5image(m_wdirnew + "diffuse_vrad_soil_sim.h5", "data",
                          m_vDiffuseVradSoil, width, height, n);

    // Leaf photon flux used by the photosynthesis module (400--700 nm).
    Utils::writeHdf5image(m_wdirnew + "direct_prad_leaf_sim.h5", "data",
                          m_vDirectPradLeaf, width, height, n);
    Utils::writeHdf5image(m_wdirnew + "diffuse_prad_leaf_sim.h5", "data",
                          m_vDiffusePradLeaf, width, height, n);
}

void FileIO::saveBalance(int year, int doy)
{
    if (!m_balanceEnabled || m_vDailyEt.empty()) return;

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << doy;
    const std::string yearstr = std::to_string(year);
    const std::string outputDirectory = m_outputDir + yearstr + "/" + oss.str() + "/";
    if (!ensureOutputDirectory(outputDirectory)) return;

    const int width = m_width_region;
    const int height = m_height_region;
    const int bands = 1;
    auto save = [&](const std::string& filename,
                    std::vector<std::vector<float>>& values) {
        Utils::writeHdf5image(outputDirectory + filename, "data", values,
                              width, height, bands);
    };

    // Units: ET/precipitation/runoff/soil water are mm; carbon fluxes are
    // g C m-2 for the day; LAI is dimensionless.
    save("et_daily_sim.h5", m_vDailyEt);
    save("precipitation_daily_sim.h5", m_vDailyPrecipitation);
    save("runoff_daily_sim.h5", m_vDailyRunoff);
    save("gpp_daily_sim.h5", m_vDailyGpp);
    save("plant_respiration_daily_sim.h5", m_vDailyPlantRespiration);
    save("npp_daily_sim.h5", m_vDailyNpp);
    save("soil_water_sim.h5", m_vSoilWater);
    save("lai_state_sim.h5", m_vLaiState);
}


void FileIO::readDefined(std::shared_ptr<ModelIO> &modelio) {


        //--------------------------------
        // import the fluspect parameters;
        //--------------------------------

        auto & definedio = modelio->m_pDefined;

        std::string infileName = m_definedDir + "/optipar_new.csv";
        int num = 1;

        // 从 CSV 文件中读取光谱参数到 definedio 的 m_optCoeff 成员
        Utils::readcsvfileinout(infileName,1,0,definedio->m_optCoeff.wl_,num);
        Utils::readcsvfileinout(infileName,1,1,definedio->m_optCoeff.nr_,num);
        Utils::readcsvfileinout(infileName,1,2,definedio->m_optCoeff.kab_,num);
        Utils::readcsvfileinout(infileName,1,3,definedio->m_optCoeff.kca_,num);
        Utils::readcsvfileinout(infileName,1,4,definedio->m_optCoeff.ks_,num);
        Utils::readcsvfileinout(infileName,1,5,definedio->m_optCoeff.kw_,num);
        Utils::readcsvfileinout(infileName,1,6,definedio->m_optCoeff.kdm_,num);
        Utils::readcsvfileinout(infileName,1,7,definedio->m_optCoeff.phiI_,num);
        Utils::readcsvfileinout(infileName,1,8,definedio->m_optCoeff.phiII_,num);
        Utils::readcsvfileinout(infileName,1,9,definedio->m_optCoeff.kcaV_,num);
        Utils::readcsvfileinout(infileName,1,10,definedio->m_optCoeff.kcaZ_,num);
        Utils::readcsvfileinout(infileName,1,11,definedio->m_optCoeff.kcant_,num);
        Utils::readcsvfileinout(infileName,1,12,definedio->m_optCoeff.kcaV2_,num);
        Utils::readcsvfileinout(infileName,1,13,definedio->m_optCoeff.phi_,num);
        Utils::readcsvfileinout(infileName,1,14,definedio->m_optCoeff.gsv1_,num);
        Utils::readcsvfileinout(infileName,1,15,definedio->m_optCoeff.gsv2_,num);
        Utils::readcsvfileinout(infileName,1,16,definedio->m_optCoeff.gsv3_,num);
        Utils::readcsvfileinout(infileName,1,17,definedio->m_optCoeff.nw_,num);
        // return false;


        //----------------------------------
        //-- import definedio->m_leafbio and definedio->m_soilset
        //----------------------------------

        std::string line;
        std::vector<std::string> fields;
        std::string definedpath = m_definedDir + "/defined.txt";
        std::string deli(" ");
        std::ifstream infile(definedpath.c_str());
        if(infile.is_open())
        {
            // 读取并解析 defined.txt 文件中的数据
            // 读取、解析和设置冠层参数（canopy parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_canopy.lai = std::atof(fields[0].c_str());       // 叶面积指数
            definedio->m_canopy.stand = std::atof(fields[1].c_str());   // 冠层密度
            definedio->m_canopy.height = std::atof(fields[2].c_str());    // 冠层高度
            definedio->m_canopy.width = std::atof(fields[3].c_str());     // 冠层宽度
            definedio->m_canopy.Gleaf = std::atof(fields[4].c_str());     //
            definedio->m_canopy.LIDFa = std::atof(fields[5].c_str());     // 叶片角度分布参数a
            definedio->m_canopy.LIDFb = std::atof(fields[6].c_str());     // 叶片角度分布参数b
            definedio->m_canopy.hspot = std::atof(fields[7].c_str());     // 热点效应参数
            definedio->m_canopy.leafwidth = std::atof(fields[8].c_str()); // 叶片宽度
            definedio->m_canopy.type = std::atof(fields[9].c_str());      // 冠层类型
            definedio->m_canopy.dist = std::atof(fields[10].c_str());     // 冠层分布类型

            // 读取、解析和设置叶片生物物理参数（leaf biophysical parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_leafbio.fp.Cab = std::atof(fields[0].c_str());   // 叶绿素含量
            definedio->m_leafbio.fp.Cw = std::atof(fields[1].c_str());    // 叶片含水量
            definedio->m_leafbio.fp.Cdm = std::atof(fields[2].c_str());   // 干物质含量
            definedio->m_leafbio.fp.Cs = std::atof(fields[3].c_str());    // 其他含量（可能是硝酸盐或其他）
            definedio->m_leafbio.fp.N = std::atof(fields[4].c_str());     // 氮含量
            definedio->m_spectral.leafRefl_ir = std::atof(fields[5].c_str()); // 叶片红外反射率
            definedio->m_spectral.leafTran_ir = std::atof(fields[6].c_str()); // 叶片红外透射率

            // 读取、解析和设置叶片生理参数（leaf physiological parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            //        definedio->m_leafbio.Fqe[0] = std::atof(fields[0].c_str());
            //        definedio->m_leafbio.Fqe[1] = std::atof(fields[1].c_str());
            definedio->m_leafbio.Vcmax = std::atof(fields[0].c_str());    // 最大羧化速率
            definedio->m_leafbio.m = std::atof(fields[1].c_str());        // 气孔敏感度
            definedio->m_leafbio.Type = std::atof(fields[2].c_str());     // 植物类型
            definedio->m_leafbio.Tparam[0] = std::atof(fields[3].c_str()); // 温度参数1
            definedio->m_leafbio.Tparam[1] = std::atof(fields[4].c_str()); // 温度参数2
            definedio->m_leafbio.Tparam[2] = std::atof(fields[5].c_str()); // 温度参数3
            definedio->m_leafbio.Tparam[3] = std::atof(fields[6].c_str()); // 温度参数4
            definedio->m_leafbio.Tparam[4] = std::atof(fields[7].c_str()); // 温度参数5
            definedio->m_leafbio.Rdparam = std::atof(fields[8].c_str());   // 叶片呼吸参数
            definedio->m_leafbio.Tyear = std::atof(fields[9].c_str());     // 年均温度
            definedio->m_leafbio.beta = std::atof(fields[10].c_str());     // 光合效率参数
            definedio->m_leafbio.kNPQs = std::atof(fields[11].c_str());    // 非光化学猝灭参数
            definedio->m_leafbio.qLs = std::atof(fields[12].c_str());      // 光保护参数
            definedio->m_leafbio.kV = std::atof(fields[13].c_str());       // 其他生理参数
            definedio->m_leafbio.stressfactor = std::atof(fields[14].c_str()); // 应激因子

            // 读取、解析和设置土壤参数（soil parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_soilset.bsm.BSMBrightness = std::atof(fields[0].c_str()); // 土壤亮度
            definedio->m_soilset.bsm.BSMlat = std::atof(fields[1].c_str());        // 土壤纬度
            definedio->m_soilset.bsm.BSMlon = std::atof(fields[2].c_str());        // 土壤经度
            definedio->m_spectral.soilRefl_ir = std::atof(fields[3].c_str());      // 土壤红外反射率

            // 读取、解析和设置土壤状态参数（soil state parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_soilset.rss = std::atof(fields[0].c_str());   // 土壤表面阻抗
            definedio->m_soilset.cs = std::atof(fields[1].c_str());    // 土壤碳储量
            definedio->m_soilset.rhos = std::atof(fields[2].c_str());  // 土壤密度
            definedio->m_soilset.lambdas = std::atof(fields[3].c_str()); // 导热系数
            definedio->m_soilset.rbs = std::atof(fields[4].c_str());   // 基础呼吸
            definedio->m_soilset.SMC = std::atof(fields[5].c_str());   // 土壤含水量
            definedio->m_soilset.Tsoil = std::atof(fields[6].c_str()); // 土壤温度
            definedio->m_soilset.satwater = std::atof(fields[7].c_str()); // 饱和含水量


            // 读取、解析和设置气溶胶参数（aerosol parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_aerocoeff.zo = std::atof(fields[0].c_str());   // 动力粗糙度长度
            definedio->m_aerocoeff.d = std::atof(fields[1].c_str());    // 零平面位移高度
            definedio->m_aerocoeff.rbc = std::atof(fields[2].c_str());  // 地表反射比
            definedio->m_aerocoeff.CR = std::atof(fields[3].c_str());   // 反射率
            definedio->m_aerocoeff.Cd = std::atof(fields[4].c_str());   // 气动阻力系数
            definedio->m_aerocoeff.CD1 = std::atof(fields[5].c_str());  // 空气动力系数
            definedio->m_aerocoeff.Psicor = std::atof(fields[6].c_str()); // 气溶胶修正系数
            definedio->m_aerocoeff.CSSOIL = std::atof(fields[7].c_str()); // 土壤表面蒸发系数
            definedio->m_aerocoeff.rwc = std::atof(fields[8].c_str());   // 叶片含水量
            definedio->m_aerocoeff.rbs = std::atof(fields[9].c_str());   // 根部呼吸


            // 读取、解析和设置元数据参数（metadata parameters）
            getline(infile,line);
            getline(infile,line);
            fields = Utils::splitt(line, deli);
            definedio->m_meta.z = atof(fields[0].c_str());
            definedio->m_meta.sm = atof(fields[1].c_str());            // 土壤湿度
            definedio->m_meta.ea = atof(fields[2].c_str());            // 蒸散发
            definedio->m_meta.Ca = atof(fields[3].c_str());            // CO2浓度
            definedio->m_meta.Oa = atof(fields[4].c_str());            // O2浓度
            definedio->m_meta.Tsold = atof(fields[5].c_str());         // 土壤温度
            definedio->m_meta.SatWater = atof(fields[6].c_str());      // 饱和含水量
            definedio->m_meta.dTime = atof(fields[7].c_str());         // 时间步长

        }else std::cout<<"Unable to open the fileinput "<<std::endl;
        infile.close();




        //分类别计算植被和土壤谱数据
        for (int i = IGBP::unclassified; i < IGBP::fill_value + 1; i++)
        {
            int IGBPtype = i;
            definedio->m_leafopt.fluspect(definedio->m_optCoeff, modelio->m_pDefined->m_mLeafbio[IGBPtype].fp, definedio->m_mSpectral[IGBPtype]);
            definedio->m_soilopt.bsm(definedio->m_optCoeff,definedio->m_soilset.bsm, definedio->m_soilset.SMC,definedio->m_mSpectral[IGBPtype]);
        }

        //分类别计算植被和土壤谱数据
        for (int i = IGBPsubtype::non_cropland; i < IGBPsubtype::winter_wheat_and_rice + 1; i++)
        {
            int IGBPsubtype = i;
            definedio->m_leafopt.fluspect(definedio->m_optCoeff, modelio->m_pDefined->m_mLeafbio_sub[IGBPsubtype].fp, definedio->m_mSpectral_sub[IGBPsubtype]);
            definedio->m_soilopt.bsm(definedio->m_optCoeff,definedio->m_soilset.bsm, definedio->m_soilset.SMC,definedio->m_mSpectral_sub[IGBPsubtype]);
        }

    //-----------------------------------
        // import leaf or soil reflectance
        //------------------------------------
        // 初始化波长数组
        for (int i = 0; i <  2001 ; i++)
        {
            definedio->m_atomcond.wl[i] = 400 + i;
            definedio->m_spectral.wl_[i] = 400 + i;
        }
        for (int i = 0; i < 126; i++)
        {
            definedio->m_atomcond.wl[i + 2001] = 2500 + i * 100;
        }
        for (int i = 0; i < 35; i++)
        {
            definedio->m_atomcond.wl[i + 2127] = 16000 + i * 1000;
        }


        // 读取辐射数据
        float  *esun_, *esky_, *fesky_, *fesun_;
        esun_ = Utils::readascfile(m_definedDir+"/Esun_.dat", 0, 0, num);
        esky_ = Utils::readascfile(m_definedDir+"/Esky_.dat", 0, 0, num);
        fesky_ = new float[num];
        fesun_ = new float[num];

        float TsEsky = 0, TlEsky = 0, TlEsun = 0, TsEsun = 0, tstot = 0, tltot = 0, temp1, temp2, step;
        int b1 = N1;
        int b2 = N1+N2;

        // 计算太阳辐射和天空辐射的总和
        for (int i = 0; i < b1 - 1; i++)
        {
            temp1 = (esky_[i] + esky_[i + 1]) / 2.0;
            step = definedio->m_atomcond.wl[i + 1] - definedio->m_atomcond.wl[i];
            temp2 = (esun_[i] + esun_[i + 1]) / 2.0;
            TsEsky += temp1 * step;
            TsEsun += temp2 * step;
        }
        tstot = (TsEsky + TsEsun) * 0.001;
        for (int i = 0; i < b1; i++)
        {
            fesky_[i] = esky_[i] / tstot;
            fesun_[i] = esun_[i] / tstot;
        }
        for (int j = b1; j < b2 - 1; j++)
        {
            temp1 = (esky_[j] + esky_[j + 1]) / 2.0;
            step = definedio->m_atomcond.wl[j + 1] - definedio->m_atomcond.wl[j];
            temp2 = (esun_[j] + esun_[j + 1]) / 2.0;
            TlEsky += temp1 * step;
            TlEsun += temp2 * step;
        }
        tltot = (TlEsky + TlEsun) * 0.001;
        for (int i = b1; i < b2; i++)
        {
            fesky_[i] = esky_[i] / tltot;
            fesun_[i] = esun_[i] / tltot;
        }

        // 将计算的辐射数据存储到 definedio 的 m_atomcond 成员中
        for (int i = 0; i < b2; i++)
        {
            definedio->m_atomcond.fesun[i] = fesun_[i];
            definedio->m_atomcond.fesky[i] = fesky_[i];
        }

        // 释放动态分配的内存
        delete[] fesky_;
        delete[] fesun_;
        delete[] esun_;
        delete[] esky_;

    std::cout<<"Finished Reading Defined Data"<<std::endl;


}

void FileIO::readVegdata(int year, int doy) {





}


void FileIO::destroy()
{

    // static image 
//    delete [] m_vDem;
//    delete [] m_vType;
//    delete [] m_vLat;
//    delete [] m_vLon;
//
//    // dynamic image
//    delete [] m_pLai; // lai
//    delete [] m_pU;   // wind speed
//    delete [] m_pRin;
//    delete [] m_pRli;
//    delete [] m_pSM;
//    delete [] m_pT;
//    delete [] m_pEa;
//    delete [] m_pP;
//    delete [] m_pCa;
//    delete [] m_pOa;
}

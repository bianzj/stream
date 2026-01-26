#include "fileio.h"

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
         m_eafile = fields[2]; //蒸散发
         m_pfile = fields[3]; //气压
         m_ufile = fields[4]; //风速
         m_Rinfile = fields[5]; //入射辐射
         m_Rlifile = fields[6]; //出射辐射

         // 第七段：读取 LAI 相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_islai = atoi(fields[0].c_str()); //判断是否存在lai文件，如果有则使用后续文件设置路径
         m_laifile = fields[1];
         m_laidir = fields[2];

         // 第八段：读取土壤水分相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_issm = atoi(fields[0].c_str());
         m_smfile = fields[1];

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

         // 第十四段：读取卫星观测方向亮温相关配置
         getline(infile,line);
         getline(infile,line);
         fields = Utils::splitt(line,deli);
         m_satmode = atoi(fields[0].c_str()); //判断是否运行卫星观测模拟并设置，0为不进行卫星观测模拟，1为先模拟研究区域再搜索卫星观测像元位置，2为先搜索卫星观测像元位置再进行模拟
         m_satdir = fields[1];
         m_satName = fields[2];
         m_nadirobliq = fields[3];


         std::cout<<"Finish Reading Meta"<<std::endl;

     }else std::cout<<"Unable to open the fileinput "<<std::endl;
     infile.close();
}

void FileIO::readGeodata()
{

    int width, height, nband;

    // 读取ERA5数据的地理范围和投影信息数据
    std::string infilename0= m_geoinfoDir + m_era5file;
    Utils::readImageinout11(infilename0,m_meteoType,m_meteowidth,m_meteoheight,nband, trans_meteo, proj_meteo);
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
    std::string m_wdirnew = m_inputDir + year_str + "/" + oss_doy.str() + "/";


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
    Utils::readHdf5image(infilename6,"data",m_vTa,m_meteowidth,m_meteoheight,nband);
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

    return 0;
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

    if(access(m_wdirnew.c_str(),0)==-1){
        int flag = mkdir(m_wdirnew.c_str(),S_IRWXU);
        if (flag == -1) {
            perror("mkdir");
        }

    }

    int height = m_workheight;
    int width = m_workwidth;
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

    if(access(m_wdirnew.c_str(),0)==-1){
        int flag = mkdir(m_wdirnew.c_str(),S_IRWXU);
        if (flag == -1) {
            perror("mkdir");
        }

    }

    int height = m_workheight;
    int width = m_workwidth;
    double trans[6];
    int n = m_node;
    if(knode >= 0) n = 1;
    std::string proj = "";

    std::string satName = m_satName;
    int temp_size = m_vDBT[0].size();
//    Utils::saveGeoImage(m_wdirnew + satName + "_dbt_sim.tif", m_vDBT, width, height, n, proj_sat, trans_sat);

    Utils::writeHdf5image(m_wdirnew + satName + "_dbt_sim.h5","data",m_vDBT, width,height,n);
    Utils::saveGeoImage(m_wdirnew + satName + "_dbt_sim.tif", m_vDBT,
                         width, height, n, proj_sat, trans_sat);
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

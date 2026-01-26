
#include <iostream>
#include "model.h"
#include <complex>

#include "fileio.h"



void Model::init() {


//    m_modelio.inputMeta();
//    m_modelio.inputGeoData();
//    m_modelio.inputMeteoData();


}


void Model::solarAngle(std::shared_ptr<Defined> m_defined,std::shared_ptr<PixelIO> m_pixelio)
{
    //---------------------------------------------------
    // for chinese area
    //------------------------------------------------------
    int doy = m_defined->m_doy;
    int k_node = m_pixelio->k_node;
    float t = m_pixelio->m_pInputset->vMeteo[k_node].t;
    float lat = m_pixelio->lat;
    float lon = m_pixelio->lon;
    float sza = 0,saa = 0;
    Utils::sunAngle(t+doy,lon,lat,&sza,&saa);
    m_pixelio->m_angle.sza = sza;
    m_pixelio->m_angle.saa = saa;
    m_pixelio->m_angle.vaa = 0;
    m_pixelio->m_angle.vza = 0;
}

//void Model::runpixel(std::shared_ptr<PixelIO> m_pPixelio) {
//
////    auto & m_pDefined = m_modelio.m_pDefined;
////
////    m_rt.prepare(m_pDefined,m_pPixelio);
////    m_rt.netrad_shortwave(m_pPixelio);
////    m_rt.netrad_longwave(m_pPixelio);
////
////    for(int kiter =0;kiter<N_ITER;kiter++) {
////        m_aero.aeresist(m_pPixelio);
////        m_bio.suresist(m_pPixelio);
////        m_evapo.heatflux(m_pPixelio);
////        m_eb.rebalance(m_pPixelio);
////    }
//
//}

void Model::runpixel(std::shared_ptr<Defined> define, std::shared_ptr<PixelIO> pixelio) {


    m_geometry.calcSolarAngle(define,pixelio);
    //solarAngle(define,pixelio);
    m_rt.optical(define,pixelio);
    m_rt.netrad_shortwave(define,pixelio);
    m_rt.netrad_longwave(define,pixelio);

    bool iscloused= false;
    int kiter =0;
    for(kiter =0;kiter<N_ITER;kiter++) {
        m_aero.aeresist(define,pixelio);
        m_bio.suresist(define,pixelio);
        m_evapo.evapotranspiration(pixelio);
        iscloused = m_eb.rebalance(pixelio);
        if(iscloused) break;
    }
    std::cout<<kiter<<std::endl;

}

//void runpixel(std::shared_ptr<Model> model,std::shared_ptr<Defined> define, std::shared_ptr<PixelIO> pixelio) {
//
//    auto &m_rt = model->m_rt;
//    auto &m_aero = model->m_aero;
//    auto &m_bio = model->m_bio;
//    auto &m_evapo = model->m_evapo;
//    auto &m_eb = model->m_eb;
//
//    model->solarAngle(pixelio);
//    m_rt.optical(define,pixelio);
//    m_rt.netrad_shortwave(define,pixelio);
//    m_rt.netrad_longwave(define,pixelio);
//
//    bool iscloused= false;
//    int kiter =0;
//    for(kiter =0;kiter<N_ITER;kiter++) {
//        m_aero.aeresist(define,pixelio);
//        m_bio.suresist(define,pixelio);
//        m_evapo.evapotranspiration(pixelio);
//        iscloused = m_eb.rebalance(pixelio);
//        if(iscloused) break;
//    }
//    std::cout<<kiter<<std::endl;
//
//}


//void Model::run()
//{
//
////   pool.async(std::function<void(std::shared_ptr<PixelIO>)>(runpixel), tests[0]);
//   // pool.async(std::function<void(std::shared_ptr<PixelIO>)>(Model::ttt), tts[0]);
//}






//void Model::inputMeta(std::string infilepath)
//{
//    fileio->readMeta(infilepath);
//}

void Model::updateVegData(std::shared_ptr<PixelIO> &pixelio, std::shared_ptr<ModelIO> &modelio, int ksubType){
    int vegtype = pixelio->m_pInputset->canopy.type;
    if ((ksubType != 0) * (vegtype == 12)){
        auto & temp_canopy = modelio->m_pDefined->m_mCanopy_sub[ksubType];
        pixelio->m_pInputset->canopy = temp_canopy;
        auto  & temp_leafbio = modelio->m_pDefined->m_mLeafbio_sub[ksubType];
        pixelio->m_pInputset->leafbio = temp_leafbio;
        auto & temp_spectral = modelio->m_pDefined->m_mSpectral_sub[ksubType];
        pixelio->m_pStaticVariable->spectal = temp_spectral;

    }else{
        auto & temp_canopy = modelio->m_pDefined->m_mCanopy[vegtype];
        pixelio->m_pInputset->canopy = temp_canopy;
        auto  & temp_leafbio = modelio->m_pDefined->m_mLeafbio[vegtype];
        pixelio->m_pInputset->leafbio = temp_leafbio;
        auto & temp_spectral = modelio->m_pDefined->m_mSpectral[vegtype];
        pixelio->m_pStaticVariable->spectal = temp_spectral;
    }

}


void Model::inputGeoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio) {

    fileio->readGeodata();
    int width = fileio->m_width;
    int height = fileio->m_height;
    long k_pos = 0;
    long n_pixel = 0;

    //根据 m_areakey 确定是否使用整个图像或仅使用部分区域。如果 m_areakey 为 0，则处理整个图像；否则，仅处理指定的子区域。
    int height1,height2,width1,width2;
    if (fileio->m_areakey == 0) {
        fileio->m_workwidth = width;
        fileio->m_workheight = height;
        height1 = 0;
        height2 = height;
        width1 = 0;
        width2 = width;
    } else {
        // //-------------------------------------------------------
        // //------ 计算研究区域角点在全局范围的行列号范围
        // //-------------------------------------------------------
        // Utils::calculatePixelRange(fileio->trans_regional, fileio->proj_regional, fileio->m_width_region, fileio->m_height_region,
        //                   fileio->trans_global, fileio->proj_global, fileio->m_width, fileio->m_height,
        //                   fileio->m_startwidth, fileio->m_startheight, fileio->m_endwidth, fileio->m_endheight);

        //-------------------------------------------------------
        //------ 可以采用上面计算结果，也可以将上面计算过程注释，直接通过命令设置实现
        //-------------------------------------------------------
        fileio->m_workwidth = fileio->m_endwidth - fileio->m_startwidth;
        fileio->m_workheight = fileio->m_endheight - fileio->m_startheight;
        height1 = fileio->m_startheight;
        height2 = fileio->m_endheight;
        width1 = fileio->m_startwidth;
        width2 = fileio->m_endwidth;
    }

    int pos_leftup = height1 * width + width1;
    int pos_rightdown = height2 * width + width2;
    fileio->m_startlat_region = fileio->m_vLat[pos_leftup];
    fileio->m_endlat_region = fileio->m_vLat[pos_rightdown];
    fileio->m_startlon_region = fileio->m_vLon[pos_leftup];
    fileio->m_endlon_region = fileio->m_vLon[pos_rightdown];

    //-------------------------------------------------------
    //------ 读取研究区域的辅助数据，包括子地表类型，Vcmax等
    //-------------------------------------------------------
    int width_subType;
    int height_subType;
    int nband_subType;
    double trans_subType[6];
    std::string proj_subType;

    int width_vcmax;
    int height_vcmax;
    int nband_vcmax;
    double trans_vcmax[6];
    std::string proj_vcmax;

    int width_region;
    int height_region;
    int nband_region;

    if(fileio->m_issubtype == 0) {
        std::cout<<"No subtype file exist"<<std::endl;
    }else
    {
        std::string infilename4 = fileio->m_globaldir + fileio->m_subtypefile;
        Utils::readImageinout11(infilename4, fileio->m_subType, width_subType, height_subType, nband_subType, trans_subType, proj_subType);
        float startlon_subtype = trans_subType[0];
        float endlon_subtype = trans_subType[0] + width1 * trans_subType[1];
        float startlat_subtype = trans_subType[3];
        float endlat_subtype = trans_subType[3] + width1 * trans_subType[5];
        //获取regional和subtype的交点经纬度并转化为region中的行列号
        float overlapLonMin = std::max(startlon_subtype, fileio->m_startlon_region);
        float overlapLonMax = std::min(endlon_subtype, fileio->m_endlon_region);
        float overlapLatMin = std::max(endlat_subtype, fileio->m_endlat_region);
        float overlapLatMax = std::min(startlat_subtype, fileio->m_startlat_region);
        int startWidth_subtype = static_cast<int>((overlapLonMin - startlon_subtype) / fileio->trans_regional[1]);
        int startHeight_subtype = static_cast<int>((overlapLatMax - startlat_subtype) / fileio->trans_regional[5]); // 注意这里是 LatMax
        int endWidth_subtype = static_cast<int>((overlapLonMax - endlat_subtype) / fileio->trans_regional[1]);
        int endHeight_subtype = static_cast<int>((overlapLatMin - startlat_subtype) / fileio->trans_regional[5]); // 注意这里是 LatMin
    }

    if(fileio->m_isvcmax == 0) {
        std::cout<<"No vcmax file exist"<<std::endl;
    }else
    {
        std::string infilename5 = fileio->m_globaldir + fileio->m_vcmaxfile;
        Utils::readImageinout11(infilename5, fileio->m_Vcmax, width_vcmax, height_vcmax, nband_vcmax, trans_vcmax, proj_vcmax);
    }

    if(fileio->m_satmode == 0) {
        std::cout<<"Sat mode 0: No DBT Calculation"<<std::endl;
    }else
    {
        std::string infileCanopyHeight = fileio->m_satdir + "veg_data/treeCanopyheight_huabei.tif";
        std::string infileTreeDensity = fileio->m_satdir + "veg_data/treeDensity_huabei.tif";
        std::string infileemis_s = fileio->m_satdir + "emis/emis_s_" + fileio->m_satName + ".tif";
        std::string infileemis_v = fileio->m_satdir + "emis/emis_v_" + fileio->m_satName + ".tif";

        Utils::readImageinout1(infileCanopyHeight, fileio->m_canopyheight, width_region, height_region, nband_region);
        Utils::readImageinout1(infileTreeDensity, fileio->m_treedensity, width_region, height_region, nband_region);
        Utils::readImageinout1(infileemis_s, fileio->m_emis_s, width_region, height_region, nband_region);
        Utils::readImageinout1(infileemis_v, fileio->m_emis_v, width_region, height_region, nband_region);

    }

    //初始化 m_vPos 向量，长度为图像的总像素数，初始值为 -1。
    fileio->m_vPos = std::vector<uint32_t>(fileio->m_width * fileio->m_height, -1);
    for (int kwidth = width1; kwidth < width2; kwidth++) {
        for (int kheight = height1; kheight < height2; kheight++) {

            k_pos = kheight * width + kwidth;
            if (fileio->m_vType[k_pos] > 16) {
                fileio->m_vPos[k_pos] = -1;
                continue;
            }

            //为每个有效像素创建一个 PixelIO 对象，并初始化其各个属性：
            std::shared_ptr<PixelIO> pixelio = std::make_shared<PixelIO>();
            pixelio->m_pInputset->canopy.type = fileio->m_vType[k_pos];
            pixelio->k_width = kwidth;
            pixelio->k_height = kheight;
            pixelio->k_workheight = kheight-height1;
            pixelio->k_workwidth = kwidth-width1;
            pixelio->k_pixel = n_pixel;
            pixelio->m_pInputset->terrain.dem = fileio->m_vDem[k_pos];
            pixelio->lat = fileio->m_vLat[k_pos];
            pixelio->lon = fileio->m_vLon[k_pos];
            pixelio->m_pInputset->soilset =  modelio->m_pDefined->m_soilset;
            pixelio->m_pInputset->meta =  modelio->m_pDefined->m_meta;

            // 判断像元经纬度是否在
            bool bool_subtype = (pixelio->lat <= trans_subType[3]) * (pixelio->lat >= (trans_subType[3] + height1 * trans_subType[5])) * (pixelio->lon >= trans_subType[0]) * (pixelio->lon <= (trans_subType[0] + width1 * trans_subType[1]));
            bool bool_vcmax = (pixelio->lat <= trans_vcmax[3]) * (pixelio->lat >= (trans_vcmax[3] + height1 * trans_vcmax[5])) * (pixelio->lon >= trans_vcmax[0]) * (pixelio->lon <= (trans_vcmax[0] + width1 * trans_vcmax[1]));

            int ksubtype = 0;
            if ((fileio->m_issubtype == 1) * bool_subtype){
                long ksubpos = pixelio->k_workheight * width_region + pixelio->k_workwidth;
                ksubtype = fileio->m_subType[ksubpos];
            }
            updateVegData(pixelio, modelio, ksubtype);

            if (fileio->m_isvcmax == 1){
                long ksubpos = pixelio->k_workheight * width_region + pixelio->k_workwidth;
                auto & temp_vcmax = fileio->m_Vcmax[ksubpos];
                pixelio->m_pInputset->leafbio.Vcmax = temp_vcmax;
            }

            auto & definedio = modelio->m_pDefined;
            pixelio->m_pStaticVariable->spectal.leafRefl_ir = definedio->m_spectral.leafRefl_ir;
            pixelio->m_pStaticVariable->spectal.leafTran_ir = definedio->m_spectral.leafTran_ir;
            pixelio->m_pStaticVariable->spectal.soilRefl_ir = definedio->m_spectral.soilRefl_ir;

            if (fileio->m_satmode == 1){
                long ksubpos = pixelio->k_workheight * width_region + pixelio->k_workwidth;
                pixelio->m_pInputset->canopy.treeStand = fileio->m_treedensity[ksubpos] / 1000000.0;
                pixelio->m_pInputset->canopy.canopyHeight = fileio->m_canopyheight[ksubpos];
                pixelio->emis_s = fileio->m_emis_s[ksubpos];
                pixelio->emis_v = fileio->m_emis_v[ksubpos];
            }

            //将创建的 PixelIO 对象添加到 modelio 的 m_vPixelio 向量中，并更新 fileio 的 m_vPos 向量
            modelio->m_vPixelio.push_back(pixelio);
            fileio->m_vPos[k_pos] = n_pixel;
            n_pixel++;

        }
    }


}


void Model::inputGeoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio,
                         int startWidth, int endWidth, int startHeight, int endHeight)
{

    fileio->readGeodata();

    fileio->readExtradata();

    int width = fileio->m_width;
    int height = fileio->m_height;
    int meteowidth = fileio->m_meteowidth;
    int meteoheight = fileio->m_meteoheight;

    //根据 m_areakey 确定是否使用整个图像或仅使用部分区域。如果 m_areakey 为 0，则处理整个图像；否则，仅处理指定的子区域。

    auto calculateGeoBounds = [](auto& fileio) {
        fileio->m_startlon = fileio->trans_global[0];
        fileio->m_startlat = fileio->trans_global[3];
        fileio->m_endlon = fileio->trans_global[0] + fileio->m_width * fileio->trans_global[1];
        fileio->m_endlat = fileio->trans_global[3] + fileio->m_height * fileio->trans_global[5];
        fileio->m_startlon_meteo = fileio->trans_meteo[0];
        fileio->m_startlat_meteo = fileio->trans_meteo[3];
        fileio->m_endlon_meteo = fileio->trans_meteo[0] + fileio->m_meteowidth * fileio->trans_meteo[1];
        fileio->m_endlat_meteo = fileio->trans_meteo[3] + fileio->m_meteoheight * fileio->trans_meteo[5];
        fileio->step_global2meteo = ((fileio->m_endlon - fileio->m_startlon) / fileio->m_width) /
                                  ((fileio->m_endlon_meteo - fileio->m_startlon_meteo) / fileio->m_meteowidth);
    };

    calculateGeoBounds(fileio);  // 统一计算地理边界

    if (fileio->m_areakey == 0) {
        startHeight = 0;
        endHeight = height;
        startWidth = 0;
        endWidth = width;
    }

    // 统一处理公共部分
    fileio->m_workwidth = endWidth - startWidth;
    fileio->m_workheight = endHeight - startHeight;
    fileio->m_width_region = (fileio->m_areakey == 0) ? width : endWidth - startWidth;
    fileio->m_height_region = (fileio->m_areakey == 0) ? height : endHeight - startHeight;

    fileio->m_vPos = std::vector<uint32_t>(width * height, -1);

    // 统一计算区域边界
    const int pos_leftup = (fileio->m_areakey == 0) ?
        startHeight * width + startWidth :
        fileio->m_startheight * width + fileio->m_startwidth;
    const int pos_rightdown = (fileio->m_areakey == 0) ?
        (endHeight - 1) * width + (endWidth - 1) :
        fileio->m_endheight * width + fileio->m_endwidth;

    fileio->m_startlat_region = fileio->m_vLat[pos_leftup];
    fileio->m_endlat_region = fileio->m_vLat[pos_rightdown];
    fileio->m_startlon_region = fileio->m_vLon[pos_leftup];
    fileio->m_endlon_region = fileio->m_vLon[pos_rightdown];

    long k_pos=0;
    long n_pixel = 0;
    for (int kheight = startHeight; kheight < endHeight; kheight++) {
        for(int kwidth =startWidth; kwidth < endWidth; kwidth++) {
            k_pos = kheight * width + kwidth;
            if ( (fileio->m_vType[k_pos] > 16) || (fileio->m_vType[k_pos] == 13) )  {
                //fileio->m_vPos[k_pos] = -1;
                continue;
            }

            std::shared_ptr<PixelIO> pixelio = std::make_shared<PixelIO>();

            pixelio->m_pInputset->canopy.type = fileio->m_vType[k_pos];
            pixelio->k_width = kwidth;
            pixelio->k_height = kheight;
            pixelio->k_workheight = kheight - startHeight;
            pixelio->k_workwidth = kwidth - startWidth;
            pixelio->k_pixel = n_pixel;
            pixelio->m_pInputset->terrain.dem = fileio->m_vDem[k_pos];
            pixelio->lat = fileio->m_vLat[k_pos];
            pixelio->lon = fileio->m_vLon[k_pos];
            pixelio->m_pInputset->soilset =  modelio->m_pDefined->m_soilset;
            pixelio->m_pInputset->meta =  modelio->m_pDefined->m_meta;

            int ksubtype = 0;
            if (fileio->m_issubtype == 1) {
                ksubtype = fileio->m_subType[k_pos];
            }
            updateVegData(pixelio, modelio, ksubtype);

            // if (fileio->m_isvcmax == 1){
            //     long ksubpos = pixelio->k_workheight * fileio->m_width_region + pixelio->k_workwidth;
            //     auto & temp_vcmax = fileio->m_Vcmax[ksubpos];
            //     if (temp_vcmax != 0) {
            //         pixelio->m_pInputset->leafbio.Vcmax = temp_vcmax;
            //     }
            // }

            auto & definedio = modelio->m_pDefined;
            pixelio->m_pStaticVariable->spectal.leafRefl_ir = definedio->m_spectral.leafRefl_ir;
            pixelio->m_pStaticVariable->spectal.leafTran_ir = definedio->m_spectral.leafTran_ir;
            pixelio->m_pStaticVariable->spectal.soilRefl_ir = definedio->m_spectral.soilRefl_ir;

            if (fileio->m_satmode == 1){
                pixelio->m_pInputset->canopy.treeStand = fileio->m_treedensity[k_pos] / 1000000.0;
                pixelio->m_pInputset->canopy.canopyHeight = fileio->m_canopyheight[k_pos];
                pixelio->emis_s = fileio->m_emis_s[k_pos] / 1000.0;;
                pixelio->emis_v = fileio->m_emis_v[k_pos] / 1000.0;;
            }
            modelio->m_vPixelio.push_back(pixelio);
            fileio->m_vPos[k_pos] = n_pixel;
            n_pixel++;
        }
    }
}


int Model::inputMeteoData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio, int year, int doy)
{
    fileio->m_vLai.clear();
    fileio->m_vSM.clear();
    fileio->m_vRin.clear();
    fileio->m_vRli.clear();
    fileio->m_vT.clear();
    fileio->m_vU.clear();
    fileio->m_vP.clear();
    fileio->m_vTa.clear();
    fileio->m_vEa.clear();

    fileio->readMeteodata(year, doy);

    int width = fileio->m_width;
    int height = fileio->m_height;
    int meteowidth = fileio->m_meteowidth;

    float step = fileio->step_global2meteo;
    int offset_width = (int)ceil((fileio->m_startlon - fileio->m_startlon_meteo) / ((fileio->m_endlon_meteo - fileio->m_startlon_meteo) / fileio->m_meteowidth));
    int offset_height = (int)ceil((fileio->m_startlat - fileio->m_startlat_meteo) / ((fileio->m_endlat_meteo - fileio->m_startlat_meteo) / fileio->m_meteoheight));

    for(int kpixel=0; kpixel < modelio->m_vPixelio.size(); kpixel++)
    {
        std::shared_ptr<PixelIO> &pixelio = modelio->m_vPixelio[kpixel];

        int kwidth = pixelio->k_width;
        int kheight = pixelio->k_height;
        int kwidth_meteo = kwidth * step + offset_width;
        int kheight_meteo = kheight * step + offset_height;
        int k_pos = kheight * width + kwidth;   //高分辨率中的位置
        int k_meteopos = kheight_meteo * meteowidth + kwidth_meteo;   //低分辨率中的位置

        if(fileio->m_islai==1){
            double trans_lai[6];
            std::string proj_lai = fileio->proj_meteo;
            std::copy(std::begin(fileio->trans_meteo), std::end(fileio->trans_meteo), trans_lai);
            trans_lai[1] = fileio->trans_global[1];
            trans_lai[5] = fileio->trans_global[5];
            int width_lai = 36000;
            int col_global2lai = static_cast<int>((fileio->trans_global[0] - trans_lai[0]) / trans_lai[1]);  // global在lai图像的列号
            int row_global2lai = static_cast<int>((fileio->trans_global[3] - trans_lai[3]) / trans_lai[5]);  // global在lai图像的行号
            int col_lai = col_global2lai + kwidth; // 研究区在lai图像的列号
            int row_lai = row_global2lai + kheight; // 研究区在lai图像的行号
            int k_pos_lai = row_lai * width_lai + col_lai;
            pixelio->m_pInputset->canopy.lai = fileio->m_vLai[k_pos_lai]/1000.0;
        }else{
            pixelio->m_pInputset->canopy.lai = fileio->m_vLai[k_meteopos];
        }

        if(fileio->m_issm ==1){
            pixelio->m_pInputset->soilset.SMC = fileio->m_vSM[k_pos];
        }else{
            pixelio->m_pInputset->soilset.SMC = fileio->m_vSM[k_meteopos];
        }


        modelio->m_pDefined->m_soilopt.bsm(modelio->m_pDefined->m_optCoeff, modelio->m_pDefined->m_soilset.bsm,
                                           pixelio->m_pInputset->soilset.SMC,pixelio->m_pStaticVariable->spectal);

        if(pixelio->m_pInputset->canopy.lai>25){
            pixelio->m_pInputset->canopy.lai = 0;
        }

        if (pixelio->m_pInputset->canopy.lai <= 0) {
            switch (pixelio->m_pInputset->canopy.type) {
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 12:
                case 14:
                    pixelio->m_pInputset->canopy.type = 16;
                    auto &temp_leafbio = modelio->m_pDefined->m_mLeafbio[16];
                    pixelio->m_pInputset->leafbio = temp_leafbio;
                    auto &temp_canopy = modelio->m_pDefined->m_mCanopy[16];
                    pixelio->m_pInputset->canopy = temp_canopy;
                    auto & temp_spectral = modelio->m_pDefined->m_mSpectral[16];
                    pixelio->m_pStaticVariable->spectal = temp_spectral;
            }
        }

        if((pixelio->m_pInputset->canopy.type == 11) * (pixelio->m_pInputset->canopy.lai<=0))
        {
            pixelio->m_pInputset->canopy.type = 17;
            auto  & temp_leafbio = modelio->m_pDefined->m_mLeafbio[17];
            pixelio->m_pInputset->leafbio = temp_leafbio;
        }

        // if(fileio->m_issubtype == 1){
        //     int ksubtype_temp = 0;
        //     long ksubpos_temp = pixelio->k_height * fileio->m_width + pixelio->k_width;
        //     ksubtype_temp = fileio->m_subType[ksubpos_temp];
        //     if(ksubtype_temp == 2)
        //     {
        //         if((doy > 105) & (doy < 235)){
        //             auto & temp_canopy = modelio->m_pDefined->m_mCanopy[16];
        //             auto lai_backup = pixelio->m_pInputset->canopy.lai;
        //             pixelio->m_pInputset->canopy = temp_canopy;
        //             pixelio->m_pInputset->canopy.lai = lai_backup;
        //             auto  & temp_leafbio = modelio->m_pDefined->m_mLeafbio[16];
        //             pixelio->m_pInputset->leafbio = temp_leafbio;
        //         }
        //     }
        //
        //     if(ksubtype_temp == 3){
        //         if((doy > 150) * (doy < 300)){
        //             auto & temp_canopy = modelio->m_pDefined->m_mCanopy[16];
        //             auto lai_backup = pixelio->m_pInputset->canopy.lai;
        //             pixelio->m_pInputset->canopy = temp_canopy;
        //             pixelio->m_pInputset->canopy.lai = lai_backup;
        //             auto  & temp_leafbio = modelio->m_pDefined->m_mLeafbio[16];
        //             pixelio->m_pInputset->leafbio = temp_leafbio;
        //         }
        //     }
        //
        // }

        if(fileio->m_isvcmax == 1)
        {
            auto & temp_vcmax = fileio->m_Vcmax[k_pos];
            if (temp_vcmax != 0) {
                    pixelio->m_pInputset->leafbio.Vcmax = temp_vcmax;
                }
        }

        modelio->m_isnodefirst = true;
        pixelio->m_pInputset->vMeteo.clear();
        for (int knode = 0; knode < fileio->m_node; knode++) {
            if (fileio->step_global2meteo < 0.1) {
                float metau = 0.0, metap = 0.0, metata = 0.0, metaea = 0.0, metarin = 0.0, metarli = 0.0, weight = 0.0;
                int krange = 1;
                float sigma = 1.0;
                float weight_sum = 0.0;
                for (int i = -krange; i <= krange; ++i) {
                    for (int j = -krange; j <= krange; ++j) {
                        int ni = kwidth_meteo + i;
                        int nj = kheight_meteo + j;

                        if (ni >= 0 && ni < meteowidth && nj >= 0 && nj < meteowidth) {
                            float distance = sqrt(
                                pow(kwidth * step + offset_width - ni, 2) +
                                pow(kheight * step + offset_height - nj, 2));

                            int pos_temp = nj * meteowidth + ni;
                            weight = exp(-pow(distance, 2) / (2 * pow(sigma, 2)));

                            metau += fileio->m_vU[knode][pos_temp] * weight;
                            metap += fileio->m_vP[knode][pos_temp] * weight;
                            metaea += SCI::es_fun(fileio->m_vEa[knode][pos_temp] - 273.15) * weight;
                            metata += fileio->m_vTa[knode][pos_temp] * weight;
                            metarin += fileio->m_vRin[knode][pos_temp] * weight;
                            metarli += fileio->m_vRli[knode][pos_temp] * weight;
                            weight_sum += weight;
                        }
                    }
                }
                // 归一化结果
                if (weight_sum > 1e-6f) {
                    metau /= weight_sum;
                    metap /= weight_sum;
                    metata /= weight_sum;
                    metaea /= weight_sum;
                    metarin /= weight_sum;
                    metarli /= weight_sum;
                } else {
                    // Fallback to center point
                    int center_pos = kheight_meteo * meteowidth + kwidth_meteo;
                    metau = fileio->m_vU[knode][center_pos];
                    metap = fileio->m_vP[knode][center_pos];
                    metaea = SCI::es_fun(fileio->m_vEa[knode][center_pos] - 273.15);
                    metata = fileio->m_vTa[knode][center_pos];
                    metarin = fileio->m_vRin[knode][center_pos];
                    metarli = fileio->m_vRli[knode][center_pos];
                }

                // 测量高度处的ta，p转化
                float height_temp = pixelio->m_pInputset->canopy.height - 2;
                if (knode <= 10 || knode >= 22) {
                    metata -= 0.006f * height_temp;
                    metap = metap * exp(-(9.80665 * pixelio->m_pInputset->canopy.height) / (287.05 * metata));
                } else {
                    metata += 0.003f * height_temp;
                    metap = metap * exp(-(9.80665 * pixelio->m_pInputset->canopy.height) / (287.05 * metata));
                }

                Meteo meteo{
                    fileio->m_vT[knode], metau, metata, metaea,
                    metap, metarin, metarli
                };
                pixelio->m_pInputset->vMeteo.push_back(meteo);
            } else {
                float ea = SCI::es_fun(fileio->m_vEa[knode][k_meteopos] - 273.15);
                Meteo meteo{
                    fileio->m_vT[knode], fileio->m_vU[knode][k_meteopos], fileio->m_vTa[knode][k_meteopos], ea,
                    fileio->m_vP[knode][k_meteopos], fileio->m_vRin[knode][k_meteopos],
                    fileio->m_vRli[knode][k_meteopos]
                };
                pixelio->m_pInputset->vMeteo.push_back(meteo);
            }

            if (modelio->m_isfirst && modelio->m_isnodefirst) {
                pixelio->m_pDynamicVariable->thermal.Tleafsunlit = 273.17;
                pixelio->m_pDynamicVariable->thermal.Tleafshaded = 273.17;
                pixelio->m_pDynamicVariable->thermal.Tsoilsunlit = 273.17;
                pixelio->m_pDynamicVariable->thermal.Tsoilshaded = 273.17;

                pixelio->m_pDynamicVariable->biostate.cs = modelio->m_pDefined->m_meta.Ca;
                pixelio->m_pDynamicVariable->biostate.ci = modelio->m_pDefined->m_meta.Ca;
                pixelio->m_pDynamicVariable->biostate.es = modelio->m_pDefined->m_meta.ea;
                modelio->m_isnodefirst = false;
            }
        }
        pixelio->n_node = fileio->m_node;

    }
    modelio->m_isfirst = false;
    return 0;
}


int Model::inputSatData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio,
                           int year, int doy)
{
    fileio->readSatdata(year, doy);

    //// 获取region和sat的交点坐标，并计算step
    // 读取sat的四个角点坐标
    fileio->m_startlon_sat = fileio->trans_sat[0];
    fileio->m_startlat_sat = fileio->trans_sat[3];
    fileio->m_endlon_sat = fileio->trans_sat[0] + fileio->m_width_sat * fileio->trans_sat[1]; // 右下角经度
    fileio->m_endlat_sat = fileio->trans_sat[3] + fileio->m_height_sat * fileio->trans_sat[5]; // 右下角纬度
    //获取regional和sat的交点经纬度并转化为region中的行列号

    float overlapLonMin = std::max(fileio->m_startlon_sat, float(fileio->m_startlon + fileio->trans_global[1] * fileio->m_startwidth));
    float overlapLonMax = std::min(fileio->m_endlon_sat, float(fileio->m_startlon + fileio->trans_global[1] * (fileio->m_startwidth + fileio->m_workwidth)));
    float overlapLatMin = std::max(fileio->m_endlat_sat, float(fileio->m_startlat + fileio->trans_global[5] * (fileio->m_startheight + fileio->m_workheight)));
    float overlapLatMax = std::min(fileio->m_startlat_sat, float(fileio->m_startlat + fileio->trans_global[5] * fileio->m_startheight));
    int startWidth_sat = static_cast<int>((overlapLonMin - fileio->m_startlon_sat) / fileio->trans_sat[1]);
    int startHeight_sat = static_cast<int>((overlapLatMax - fileio->m_startlat_sat) / fileio->trans_sat[5]); // 注意这里是 LatMax
    int endWidth_sat = static_cast<int>((overlapLonMax - fileio->m_startlon_sat) / fileio->trans_sat[1]);
    int endHeight_sat = static_cast<int>((overlapLatMin - fileio->m_startlat_sat) / fileio->trans_sat[5]); // 注意这里是 LatMin
//    int startWidth_sat = static_cast<int>((overlapLonMin - fileio->m_startlon_region) / fileio->trans_global[1]);
//    int startHeight_sat = static_cast<int>((overlapLatMax - fileio->m_startlat_region) / fileio->trans_global[5]); // 注意这里是 LatMax
//    int endWidth_sat = static_cast<int>((overlapLonMax - fileio->m_startlon_region) / fileio->trans_global[1]);
//    int endHeight_sat = static_cast<int>((overlapLatMin - fileio->m_startlat_region) / fileio->trans_global[5]); // 注意这里是 LatMin


    fileio->step_global2sat = ((fileio->m_endlon_region - fileio->m_startlon_region) / (fileio->m_width_region)) / ((fileio->m_endlon_sat - fileio->m_startlon_sat) / (fileio->m_width_sat)) ;
    float step = fileio->step_global2sat ;
    // float offset_width = (fileio->m_startlon_region - fileio->m_startlon_sat) / ((fileio->m_endlon_sat - fileio->m_startlon_sat) / fileio->m_width_sat);
    // float offset_height = (fileio->m_startlat_region - fileio->m_startlat_sat) / ((fileio->m_endlat_sat - fileio->m_startlat_sat) / fileio->m_height_sat);

    float pixel_lon = (fileio->m_endlon_sat - fileio->m_startlon_sat) / fileio->m_width_sat;
    float pixel_lat = (fileio->m_endlat_sat - fileio->m_startlat_sat) / fileio->m_height_sat;
    int offset_width = (fileio->m_startlon + fileio->trans_global[1] * fileio->m_startwidth - fileio->m_startlon_sat) / pixel_lon;
    int offset_height = (fileio->m_startlat + fileio->trans_global[5] * fileio->m_startheight - fileio->m_startlat_sat) / pixel_lat;

    for(int kpixel=0; kpixel < modelio->m_vPixelio.size(); kpixel++)
    {
        //遍历每个像素并获取 PixelIO 对象。将定义的卫星观测角度事件信息复制到每个像元的输入设置中。
        std::shared_ptr<PixelIO> &pixelio = modelio->m_vPixelio[kpixel];

        int kwidth = pixelio->k_workwidth;
        int kheight = pixelio->k_workheight;
        int workwidth = kwidth * step + offset_width;
        int workheight = kheight * step + offset_height;
        int k_satpos = workheight * fileio->m_width_sat + workwidth;   //region像元在sat中的位置

        if (workwidth >= startWidth_sat && workwidth <= endWidth_sat && workheight >= startHeight_sat && workheight <= endHeight_sat)
        {
            pixelio->m_pDynamicVariable->satellite.vza = fileio->m_vza[k_satpos] / 100.0;
            pixelio->m_pDynamicVariable->satellite.sza = fileio->m_sza[k_satpos] / 100.0;
            pixelio->m_pDynamicVariable->satellite.vaa = fileio->m_vaa[k_satpos];
            pixelio->m_pDynamicVariable->satellite.saa = fileio->m_saa[k_satpos];
            std::ostringstream oss_time;
            oss_time << std::setw(4) << std::setfill('0') << fileio->m_time[k_satpos];
            pixelio->m_pDynamicVariable->satellite.time = oss_time.str();
        }

    }
    modelio->m_isfirst = false;
    return 0;
}


void Model::inputDefinedData(std::shared_ptr<FileIO> &fileio, std::shared_ptr<ModelIO> &modelio) {

    fileio->readDefined(modelio);
  //  m_pDefined->input(fileio->m_projectDir);
}


void Model::flash(std::shared_ptr<PixelIO> &pixelio) {

    int knode = pixelio->k_node;
    pixelio->m_vTch[knode] = (pixelio->m_pDynamicVariable->thermal.Tleafshaded);
    pixelio->m_vTss[knode] = (pixelio->m_pDynamicVariable->thermal.Tsoilsunlit);
    pixelio->m_vTsh[knode] = (pixelio->m_pDynamicVariable->thermal.Tsoilshaded);
    pixelio->m_vTcs[knode] = (pixelio->m_pDynamicVariable->thermal.Tleafsunlit);

    pixelio->m_vTrh[knode] = (pixelio->m_pDynamicVariable->thermal.Troofshaded);
    pixelio->m_vTrs[knode] = (pixelio->m_pDynamicVariable->thermal.Troofsunlit);
    pixelio->m_vTwh[knode] = (pixelio->m_pDynamicVariable->thermal.Twallshaded);
    pixelio->m_vTws[knode] = (pixelio->m_pDynamicVariable->thermal.Twallsunlit);
    pixelio->m_vTth[knode] = (pixelio->m_pDynamicVariable->thermal.Tstreetshaded);
    pixelio->m_vTts[knode] = (pixelio->m_pDynamicVariable->thermal.Tstreetsunlit);
}



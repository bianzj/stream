#include <iostream>

#include "engine.h"



void Engine::initProject(std::string infilepath) {


    //m_modelio->inputMeta(infilepath);
    //m_modelio->inputDefinedData();
    m_fileio->readMeta(infilepath);


}


void Engine::initVariable() {

   // m_modelio->inputGeoData();
   // m_fileio->readGeodata();
    m_model->inputDefinedData(m_fileio,m_modelio);
    m_model->inputGeoData(m_fileio, m_modelio);
}

void Engine::initVariable(int startWidth, int endWidth, int startHeight, int endHeight) {

  //  m_modelio->inputGeoData(startWidth,endWidth,startHeight,endHeight);
    m_model->inputDefinedData(m_fileio,m_modelio);
    m_model->inputGeoData(m_fileio, m_modelio, startWidth,endWidth,startHeight,endHeight);
}



int Engine::upload(int year, int doy) {

    //m_modelio->inputMeteoData(year,doy);

    std::ostringstream  oss_doy;
    std::string year_str = std::to_string(year);
    oss_doy << std::setw(3)<<std::setfill('0')<<doy;
    std::string m_wdirnew = m_fileio->m_inputDir + year_str + "/" + oss_doy.str() + "/";

    if (access(m_wdirnew.c_str(),0)==-1){
        return -1;
    }

    m_model->inputMeteoData(m_fileio, m_modelio, year, doy);

    // 如果需要模拟方向亮温，则读取卫星观测数据
    if(m_fileio->m_satmode != 0)
    {
        m_model->inputSatData(m_fileio, m_modelio, year, doy);
    }

    return 1;

}

int runpixel(std::shared_ptr<Model> model, std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio, std::shared_ptr<FileIO> fileio) {

    auto & m_bio = model->m_bio;
    auto & m_evapo = model->m_evapo;
    auto & m_eb = model->m_eb;
    auto & m_rt = model->m_rt;
    auto & m_aero = model->m_aero;
    auto & m_geometry = model->m_geometry;

    for(int knode = 0;knode<pixelio->n_node;knode++) {

        pixelio->k_node = knode;

        m_geometry.calcSolarAngle(defined,pixelio);  // 计算太阳角度
        bool iscloused = false;
        int kiter = 0;
        switch (pixelio->m_pInputset->canopy.type)
        {
        case 13:
            m_rt.netrad_shortwave_urban(defined, pixelio);
            for (kiter = 0; kiter < N_ITER; kiter++)
            {
                m_rt.netrad_longwave_urban(defined, pixelio);
                m_aero.aeresist_urban(defined, pixelio);
                m_bio.suresist_urban(defined, pixelio);
                m_evapo.evapotranspiration_urban(pixelio);
                iscloused = m_eb.rebalance_urban(pixelio);
                if (iscloused)
                {
                    break;
                }
            }
            break;
        default:
            m_rt.netrad_shortwave(defined, pixelio);
            for (kiter = 0; kiter < N_ITER; kiter++)
            {
                m_rt.netrad_longwave(defined, pixelio);
                m_aero.aeresist(defined, pixelio);
                m_bio.suresist(defined, pixelio);
                m_evapo.evapotranspiration(pixelio);
                iscloused = m_eb.rebalance(pixelio);
                if (iscloused)
                {
                    break;
                }
            }
            break;
        }

        m_rt.nadirTir(pixelio);  //调用 nadirTir 计算天顶方向的热红外辐射
        model->flash(pixelio);
    }

    switch (fileio->m_satmode)
    {
    case 0:
        break;
    case 1:
        m_rt.satTirt(pixelio);
        break;
    case 2:
        m_rt.satTirt(pixelio);
        break;
    default:
        break;
    }

    return 1;
}

int subrunpixel(std::shared_ptr<Model> model, std::shared_ptr<Defined> defined,std::shared_ptr<PixelIO> pixelio, int thenode) {

//    auto & m_pDefined = m_modelio.m_pDefined;

    auto & m_bio = model->m_bio;
    auto & m_evapo = model->m_evapo;
    auto & m_eb = model->m_eb;
    auto & m_rt = model->m_rt;
    auto & m_aero = model->m_aero;
    auto & m_geometry = model->m_geometry;

    pixelio->m_pStaticVariable->spectal = defined->m_spectral;
    //m_rt.optical(defined, pixelio);

    int startNode,endNode;
    if(thenode >= 0) {startNode= thenode; endNode = thenode+1;}
    else{startNode = 0; endNode = pixelio->n_node;}
    for(int knode = startNode;knode< endNode;knode++) {

        pixelio->k_node = knode;

        m_geometry.calcSolarAngle(defined,pixelio);
        //model->solarAngle(defined,pixelio);

        m_rt.netrad_shortwave(defined, pixelio);
        m_rt.netrad_longwave(defined, pixelio);

        bool iscloused = false;
        int kiter = 0;
        for (kiter = 0; kiter < N_ITER; kiter++) {
            m_aero.aeresist(defined, pixelio);
            m_bio.suresist(defined, pixelio);
            m_evapo.evapotranspiration(pixelio);
            iscloused = m_eb.rebalance(pixelio);
            if (iscloused) break;

        }
        m_rt.nadirTir(pixelio);
        model->flash(pixelio);


    }
    return 1;
}


void Engine::observe(int knode) {
    std::cout<<"begin observing"<<std::endl;
    //---------------------------------------
    //----- Extra time ???
    //---------------------------------------
    // 获取工作区的宽度和高度
    int width = m_fileio->m_width_region;
    int height = m_fileio->m_height_region;
    int startNode,endNode;

    // 确定开始和结束节点,根据 knode 的值，确定是处理单个节点还是所有节点。
    if(knode >= 0) {startNode= knode;
        endNode = knode + 1;
        //   m_fileio->m_vTsk.resize(1);
    }
    else{startNode = 0;
        endNode = m_fileio->m_node;}

    // 清空温度向量
    m_fileio->m_vTsk.clear();
    m_fileio->m_vTss.clear();
    m_fileio->m_vTsh.clear();
    m_fileio->m_vTcs.clear();
    m_fileio->m_vTch.clear();

    m_fileio->m_vTroofsunlit.clear();
    m_fileio->m_vTroofshaded.clear();
    m_fileio->m_vTwallsunlit.clear();
    m_fileio->m_vTwallshaded.clear();
    m_fileio->m_vTstreetsunlit.clear();
    m_fileio->m_vTstreetshaded.clear();

    m_fileio->m_vDBT.clear();

    // 初始化温度向量，大小为工作区的宽度 * 高度
    for(int k=startNode;k<endNode;k++) {
        std::vector<float> temptsk = std::vector<float>(width * height, 0);
        std::vector<float> temptss = std::vector<float>(width * height, 0);
        std::vector<float> temptsh = std::vector<float>(width * height, 0);
        std::vector<float> temptcs = std::vector<float>(width * height, 0);
        std::vector<float> temptch = std::vector<float>(width * height, 0);

        std::vector<float> temptroofsunlit = std::vector<float>(width * height, 0);
        std::vector<float> temptroofshaded = std::vector<float>(width * height, 0);
        std::vector<float> temptwallsunlit = std::vector<float>(width * height, 0);
        std::vector<float> temptwallshaded = std::vector<float>(width * height, 0);
        std::vector<float> temptstreetsunlit = std::vector<float>(width * height, 0);
        std::vector<float> temptstreetshaded = std::vector<float>(width * height, 0);

        m_fileio->m_vTsk.push_back(temptsk);
        m_fileio->m_vTss.push_back(temptss);
        m_fileio->m_vTsh.push_back(temptsh);
        m_fileio->m_vTcs.push_back(temptcs);
        m_fileio->m_vTch.push_back(temptch);
        m_fileio->m_vTroofsunlit.push_back(temptroofsunlit);
        m_fileio->m_vTroofshaded.push_back(temptroofshaded);
        m_fileio->m_vTwallsunlit.push_back(temptwallsunlit);
        m_fileio->m_vTwallshaded.push_back(temptwallshaded);
        m_fileio->m_vTstreetsunlit.push_back(temptstreetsunlit);
        m_fileio->m_vTstreetshaded.push_back(temptstreetshaded);

    }

    std::vector<float> tempdbt = std::vector<float>(width * height, 0);
    m_fileio->m_vDBT.push_back(tempdbt);

    // 遍历所有的 PixelIO 对象
    for (int i = 0; i < m_modelio->m_vPixelio.size(); i++) {
        int kwidth = m_modelio->m_vPixelio[i]->k_workwidth;
        int kheight = m_modelio->m_vPixelio[i]->k_workheight;
        int kworldwidth = m_modelio->m_vPixelio[i]->k_width;
        int kworldheight = m_modelio->m_vPixelio[i]->k_height;
        int worldpos = kworldheight *0.04* m_fileio->m_width*0.04 + kworldwidth*0.04;  // 计算全局位置
        long pos = kheight * width + kwidth;  // 计算本地位置

        // 更新温度向量
        for(int k=startNode;k<endNode;k++) {
            //   m_fileio->m_vTsk[pos] = 100;
            int kk = 0;
            if(knode < 0) kk = k;
            m_fileio->m_vTsk[kk][pos] = m_modelio->m_vPixelio[i]->m_vSkt[k];
            m_fileio->m_vTch[kk][pos] = m_modelio->m_vPixelio[i]->m_vTch[k];
            m_fileio->m_vTss[kk][pos] = m_modelio->m_vPixelio[i]->m_vTss[k];
            m_fileio->m_vTsh[kk][pos] = m_modelio->m_vPixelio[i]->m_vTsh[k];
            m_fileio->m_vTcs[kk][pos] = m_modelio->m_vPixelio[i]->m_vTcs[k];

            m_fileio->m_vTroofsunlit[kk][pos] = m_modelio->m_vPixelio[i]->m_vTrs[k];
            m_fileio->m_vTroofshaded[kk][pos] = m_modelio->m_vPixelio[i]->m_vTrh[k];
            m_fileio->m_vTwallsunlit[kk][pos] = m_modelio->m_vPixelio[i]->m_vTws[k];
            m_fileio->m_vTwallshaded[kk][pos] = m_modelio->m_vPixelio[i]->m_vTwh[k];
            m_fileio->m_vTstreetsunlit[kk][pos] = m_modelio->m_vPixelio[i]->m_vTts[k];
            m_fileio->m_vTstreetshaded[kk][pos] = m_modelio->m_vPixelio[i]->m_vTth[k];
        }

        m_fileio->m_vDBT[0][pos] = m_modelio->m_vPixelio[i]->m_DBT;
    }


    // 调用 saveSkt 函数保存温度数据，并输出完成消息。
    m_fileio->saveSkt(m_modelio->m_pDefined->m_year, m_modelio->m_pDefined->m_doy,knode);
    m_fileio->saveDBT(m_modelio->m_pDefined->m_year, m_modelio->m_pDefined->m_doy,0);
    std::cout<<"finish observing"<<std::endl;
}


void Engine::sythrun() {


}

void Engine::run()
{

    // 初始化变量
    initVariable(m_fileio->m_startwidth, m_fileio->m_endwidth, m_fileio->m_startheight, m_fileio->m_endheight);

    for(int kyear = m_fileio->startYear;kyear <= m_fileio->endYear;kyear++) {

        int startDoy = 1;
        int endDoy = 366;

        if (kyear == m_fileio->startYear) startDoy = m_fileio->startDoy;
        if (kyear == m_fileio->endYear) endDoy = m_fileio->endDoy;

        for (int kdoy = startDoy; kdoy <= endDoy; kdoy++) {

            std::cout<<"---------------------"<<kyear<<" "<<
                      kdoy<<"-----------------------"<<std::endl;
            int flag = upload(kyear, kdoy);
            if(flag < 0) continue;
            std::shared_ptr<Defined> definedio = m_modelio->m_pDefined;
            definedio->m_year = kyear;
            definedio->m_doy = kdoy;
            bool isok = true;
            if(isok == true){
                thread_pool pool(N_THREAD);
                for (int i = 0; i < m_modelio->m_vPixelio.size(); i++) {
                    std::shared_ptr<Defined> definedio = m_modelio->m_pDefined;
                    std::shared_ptr<PixelIO> pixelio = m_modelio->m_vPixelio[i];
                    pool.async(runpixel, m_model, definedio, pixelio, m_fileio);
                }
            }
            observe(-1);
            std::cout<<"finish simulation"<<std::endl;
        }
    }

}

//-------------------------------------
//--- specific region, year, doy, and node or nodes
//-------------------------------------
void Engine::subrun(int startWidth, int endWidth, int startHeight, int endHeight, int theyear, int thedoy, int knode) {


    initVariable(startWidth,endWidth,startHeight,endHeight);


    int width = m_fileio->m_width;
    int height = m_fileio->m_height;
    int num = m_modelio->m_vPixelio.size();
    thread_pool pool(N_THREAD);



    upload(theyear,thedoy);

    std::shared_ptr<Defined> definedio = m_modelio->m_pDefined;
    definedio->m_year = theyear;
    definedio->m_doy = thedoy;

    std::cout<<"begin simulation ..."<<std::endl;
    for(int i=0;i< m_modelio->m_vPixelio.size();i++) {

        std::shared_ptr<PixelIO> &pixelio = m_modelio->m_vPixelio[i];
        pool.async(subrunpixel, m_model, definedio, pixelio, knode);
       // int pos = m_modelio->m_vPixelio[i]->k_pos;
     //   m_fileio->m_vSkt[pos] =m_modelio->m_vPixelio[i]->m_sk t;

    }
    std::cout<<"finish simulation"<<std::endl;

}


//-------------------------------------
//--- specific region
//-------------------------------------
void Engine::subrun(int startWidth, int endWidth, int startHeight, int endHeight) {


    initVariable(startWidth,endWidth,startHeight,endHeight);

    for(int kyear = m_fileio->startYear;kyear < m_fileio->endYear;kyear) {

        int startDoy = 1;
        int endDoy = 366;

        if(kyear == m_fileio->startYear) startDoy = m_fileio->startDoy;
        if(kyear == m_fileio->endYear) endDoy = m_fileio->endDoy;

        for(int kdoy = startDoy;kdoy<endDoy;kdoy++) {
            int width = m_fileio->m_width;
            int height = m_fileio->m_height;
            long num = m_modelio->m_vPixelio.size();
            thread_pool pool(N_THREAD);


            upload(kyear, kdoy);

            std::shared_ptr<Defined> definedio = m_modelio->m_pDefined;
            definedio->m_year = kyear;
            definedio->m_doy = kdoy;

            for (int i = 0; i < m_modelio->m_vPixelio.size(); i++) {

                std::shared_ptr<PixelIO> &pixelio = m_modelio->m_vPixelio[i];
                pool.async(subrunpixel, m_model, definedio, pixelio, -1);
                // int pos = m_modelio->m_vPixelio[i]->k_pos;
                //   m_fileio->m_vSkt[pos] =m_modelio->m_vPixelio[i]->m_sk t;

            }
        }

    }

}


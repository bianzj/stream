#include "evapo.h"




void Evapo::evapotranspiration(std::shared_ptr<PixelIO> pixelio) {
    // 获取当前网格单元的关键参数
    int k_node = pixelio->k_node;
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node];  // 气象数据

    // 获取动态变量引用
    NetRad &netrad = pixelio->m_pDynamicVariable->netrad;  // 净辐射
    Thermal &thermal = pixelio->m_pDynamicVariable->thermal;  // 温度状态
    BioState &biostate = pixelio->m_pDynamicVariable->biostate;  // 生物状态
    Resistance &resist = pixelio->m_pDynamicVariable->resistance;  // 阻抗参数
    Heatflux &heatflux = pixelio->m_pDynamicVariable->heatflux;  // 热通量存储
    MeteoMeta &meteod = pixelio->m_pInputset->meta;  // 气象元数据
    Canopy &canopy = pixelio->m_pInputset->canopy;  // 冠层参数

    // 水汽压转比湿换算系数
    float e_to_q = MH20 / MAIR / meteo.p;  // p为大气压力(Pa)
    float ea = meteo.ea;           // 实际水汽压(kPa)
    float qa = ea * e_to_q;        // 实际比湿(kg/kg)
    float lambda, Tc, ra, rs, ei, qi;
    float Ta = meteo.ta;           // 空气温度(K)

    // 当叶面积指数>0时计算叶片通量
    if(canopy.lai >0) {
        //----------------------------
        // 阳生叶片计算
        //----------------------------
        Tc = thermal.Tleafsunlit;         // 阳生叶片温度(K)
        ra = resist.raa_leaf;             // 叶片边界层阻力(s/m)
        rs = biostate.rssunlit;           // 阳生叶片气孔阻力(s/m)
                ra = std::max(ra, 1e-3f);
rs = std::max(rs, 1e-3f);
        lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 汽化潜热(J/kg)
        ei = SCI::es_fun(Tc - 273.15);    // 叶片温度下的饱和水汽压(kPa)
        qi = ei * e_to_q;                 // 饱和比湿(kg/kg)
        heatflux.LEleafsunlit = RHOA / (ra + rs) * lambda * (qi - qa); // Assign to LEleafsunlit
        heatflux.Hleafsunlit = (RHOA * CP) / ra * (Tc - Ta); // Assign to Hleafsunlit
        heatflux.Nleafsunlit = netrad.directVrad_leaf + netrad.diffuseVrad_leaf +
                               netrad.directTrad_leaf + netrad.diffuseTrad_leaf -
                               SCI::StefanBoltzmann(Tc) * 2.0;  // 叶片双面辐射

        //----------------------------
        // 阴生叶片计算
        //----------------------------
        Tc = thermal.Tleafshaded;     // 阴生叶片温度(K)
        ra = resist.raa_leaf;         // 叶片边界层阻力(s/m)
        rs = biostate.rsshaded;       // 阴生叶片气孔阻力(s/m)
        ra = std::max(ra, 1e-3f);
        rs = std::max(rs, 1e-3f);
        lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
        ei = SCI::es_fun(Tc - 273.15);
        qi = ei * e_to_q;
        heatflux.LEleafshaded = RHOA / (ra + rs) * lambda * (qi - qa);
        heatflux.Hleafshaded = (RHOA * CP) / ra * (Tc - Ta);
        heatflux.Nleafshaded = netrad.diffuseVrad_leaf + netrad.diffuseTrad_leaf -
                               SCI::StefanBoltzmann(Tc) * 2.0;
    }

    //----------------------------
    // 光照土壤计算
    //----------------------------
    Tc = thermal.Tsoilsunlit;       // 光照土壤温度(K)
    ra = resist.raa_soil;           // 土壤边界层阻力(s/m)
    rs = biostate.rss;              // 土壤表面阻力(s/m)
    ra = std::max(ra, 1e-3f);
    rs = std::max(rs, 1e-3f);
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);  // 土壤温度饱和水汽压
    qi = ei * e_to_q;
    // 潜热通量 (土壤蒸发)
    heatflux.LEsoilsunlit = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hsoilsunlit = (RHOA * CP) / ra * (Tc - Ta);
    heatflux.Nsoilsunlit = netrad.directVrad_soil + netrad.diffuseVrad_soil +
                           netrad.directTrad_soil + netrad.diffuseTrad_soil -
                           SCI::StefanBoltzmann(Tc);

    //----------------------------
    // 阴影土壤计算
    //----------------------------
    Tc = thermal.Tsoilshaded;       // 阴影土壤温度(K)
    ra = resist.raa_soil;           // 土壤边界层阻力(s/m)
    rs = biostate.rss;               // 土壤表面阻力(s/m)
    ra = std::max(ra, 1e-3f);
    rs = std::max(rs, 1e-3f);
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;
    heatflux.LEsoilshaded = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hsoilshaded = (RHOA * CP) / ra * (Tc - Ta);
    heatflux.Nsoilshaded = netrad.diffuseVrad_soil + netrad.diffuseTrad_soil -
                           SCI::StefanBoltzmann(Tc);

    // 土壤热通量 = 净辐射的25%
    heatflux.Gsunlit = heatflux.Nsoilsunlit * 0.25;
    heatflux.Gshaded = heatflux.Nsoilshaded * 0.25;
}

void Evapo::evapotranspiration_urban(std::shared_ptr<PixelIO> pixelio) {
    int k_node = pixelio->k_node;
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node];

    NetRad &netrad = pixelio->m_pDynamicVariable->netrad;
    Thermal &thermal = pixelio->m_pDynamicVariable->thermal;
    BioState &biostate = pixelio->m_pDynamicVariable->biostate;
    Resistance &resist = pixelio->m_pDynamicVariable->resistance;
    Heatflux &heatflux = pixelio->m_pDynamicVariable->heatflux;
    MeteoMeta &meteod = pixelio->m_pInputset->meta;
    Canopy &canopy = pixelio->m_pInputset->canopy;
    Urban &urban = pixelio->m_pInputset->urban;

    float Trs = thermal.Troofsunlit;
    float Trh = thermal.Troofshaded;
    float Tws = thermal.Twallsunlit;
    float Twh = thermal.Twallshaded;
    float Tss = thermal.Tstreetsunlit;
    float Tsh = thermal.Tstreetshaded;

    // 计算水汽压转换系数
    float e_to_q = MH20 / MAIR / meteo.p;
    float ea = meteo.ea;                // 空气中的水汽压
    float qa = ea * e_to_q;             // 空气中的比湿度
    float lambda, Tc, ra, rs, ei, qi;   // 蒸发热、温度、空气阻力、表面阻力、饱和水汽压、比湿度
    float Ta = meteo.ta;                // 气温

    //----------------------------
    // Roof sunlit (阳生屋顶)
    //----------------------------
    Tc = Trs;            // 温度
    ra = resist.raa_roof;                // 空气阻力
    rs = biostate.rssroof;               // 表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度

    // [已修复] 潜热通量 LE (Latent Heat): 使用 lambda 和湿度差
    heatflux.LEroofsunlit = RHOA / (ra + rs) * lambda * (qi - qa);
    // [已修复] 显热通量 H (Sensible Heat): 使用 CP 和温度差
    heatflux.Hroofsunlit = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nroofsunlit = netrad.directVrad_roof + netrad.diffuseVrad_roof +
                           netrad.directTrad_roof + netrad.diffuseTrad_roof -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Roof shaded (阴生屋顶)
    //----------------------------
    Tc = Trh;
    ra = resist.raa_roof;
    rs = biostate.rssroof;
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;

    // [已修复]
    heatflux.LEroofshaded = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hroofshaded = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nroofshaded = netrad.diffuseVrad_roof + netrad.diffuseTrad_roof -
                           SCI::StefanBoltzmann(Tc);

    //----------------------------
    // Wall sunlit (阳生墙壁)
    //----------------------------
    Tc = Tws;
    ra = resist.raa_wall;
    rs = biostate.rsswall;
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;

    // [已修复]
    heatflux.LEwallsunlit = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hwallsunlit = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nwallsunlit = netrad.directVrad_wall + netrad.diffuseVrad_wall +
                           netrad.directTrad_wall + netrad.diffuseTrad_wall -
                           SCI::StefanBoltzmann(Tc);

    //----------------------------
    // Wall shaded (阴生墙壁)
    //----------------------------
    Tc = Twh;
    ra = resist.raa_wall;
    rs = biostate.rsswall;
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;

    // [已修复]
    heatflux.LEwallshaded = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hwallshaded = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nwallshaded = netrad.directTrad_wall + netrad.diffuseTrad_wall -
                           SCI::StefanBoltzmann(Tc);

    //----------------------------
    // Street sunlit (阳生街道)
    //----------------------------
    Tc = Tss;
    ra = resist.raa_street;
    rs = biostate.rssstreet;
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;

    // [已修复]
    heatflux.LEstreetsunlit = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hstreetsunlit = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nstreetsunlit = netrad.directVrad_street + netrad.diffuseVrad_street +
                           netrad.directTrad_street + netrad.diffuseTrad_street -
                           SCI::StefanBoltzmann(Tc);

    //----------------------------
    // Street shaded (阴生街道)
    //----------------------------
    Tc = Tsh;
    ra = resist.raa_street;
    rs = biostate.rssstreet;
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;
    ei = SCI::es_fun(Tc - 273.15);
    qi = ei * e_to_q;

    // [已修复]
    heatflux.LEstreetshaded = RHOA / (ra + rs) * lambda * (qi - qa);
    heatflux.Hstreetshaded = (RHOA * CP) / ra * (Tc - Ta);

    heatflux.Nstreetshaded = netrad.diffuseVrad_street + netrad.diffuseTrad_street -
                           SCI::StefanBoltzmann(Tc);


    // ec          = ea + (ei-ea)*ra./(ra+rs);         % [W m-2] vapour pressure at the leaf surface
    // Cc          = Ca - (Ca-Ci).*ra./(ra+rs);        % [umol m-2 s-1] CO2 concentration at the leaf surface
    // Analysis of urban heat-island effect using ASTER and ETM+ Data: Separation of anthropogenic heat discharge and natural heat radiation from sensible heat flux
    // 计算土壤热通量,城市不透水面系数Cg系数春天0.7,夏天0.4,冬天1.0
    heatflux.Groofsunlit = heatflux.Nroofsunlit * 0.4;
    heatflux.Groofshaded = heatflux.Nroofshaded * 0.4;
    heatflux.Gwallsunlit = heatflux.Nwallsunlit * 0.4;
    heatflux.Gwallshaded = heatflux.Nwallshaded * 0.4;
    heatflux.Gstreetsunlit = heatflux.Nstreetsunlit * 0.4;
    heatflux.Gstreetshaded = heatflux.Nstreetshaded * 0.4;
}
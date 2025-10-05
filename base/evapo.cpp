#include "evapo.h"




void Evapo::evapotranspiration(std::shared_ptr<PixelIO> pixelio) {
    int k_node = pixelio->k_node;
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node];

    NetRad &netrad = pixelio->m_pDynamicVariable->netrad;
    Thermal &thermal = pixelio->m_pDynamicVariable->thermal;
    BioState &biostate = pixelio->m_pDynamicVariable->biostate;
    Resistance &resist = pixelio->m_pDynamicVariable->resistance;
    Heatflux &heatflux = pixelio->m_pDynamicVariable->heatflux;
    MeteoMeta &meteod = pixelio->m_pInputset->meta;
    Canopy &canopy = pixelio->m_pInputset->canopy;

    // 提取温度信息
    float Tcs = thermal.Tleafshaded;    // 遮荫叶片温度
    float Tch = thermal.Tleafsunlit;    // 阳光直射叶片温度
    float Tsh = thermal.Tsoilshaded;    // 遮荫土壤温度
    float Tss = thermal.Tsoilsunlit;    // 阳光直射土壤温度

    // 计算水汽压转换系数
    float e_to_q = MH20 / MAIR / meteo.p;
    float ea = meteo.ea;                // 空气中的水汽压
    float qa = ea * e_to_q;             // 空气中的比湿度
    float lambda, Tc, ra, rs, ei, qi;   // 蒸发热、温度、空气阻力、表面阻力、饱和水汽压、比湿度
    float Ta = meteo.ta;                // 气温


    if(canopy.lai >0) {
        //----------------------------
        // Leaf sunlit
        //----------------------------

        Tc = thermal.Tleafsunlit;           // 阳光直射叶片温度
        ra = resist.raa_leaf;               // 叶片空气阻力
        rs = biostate.rssunlit;             // 阳光直射叶片表面阻力
        lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
        ei = SCI::es_fun(Tc - 273.15);      // 计算饱和水汽压
        qi = ei * e_to_q;                   // 计算比湿度
        heatflux.Hleafsunlit = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
        heatflux.LEleafsunlit = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
        heatflux.Nleafsunlit = netrad.directVrad_leaf + netrad.diffuseVrad_leaf +
                               netrad.directTrad_leaf + netrad.diffuseTrad_leaf -
                               SCI::StefanBoltzmann(Tc) * 2.0;         // 计算净辐射

        //----------------------------
        // Leaf shaded
        //----------------------------
        Tc = thermal.Tleafshaded;           // 遮荫叶片温度
        ra = resist.raa_leaf;               // 叶片空气阻力
        rs = biostate.rsshaded;             // 遮荫叶片表面阻力
        lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
        ei = SCI::es_fun(Tc - 273.15);      // 计算饱和水汽压
        qi = ei * e_to_q;                   // 计算比湿度
        heatflux.Hleafshaded = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
        heatflux.LEleafshaded = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
        heatflux.Nleafshaded = netrad.diffuseVrad_leaf + netrad.diffuseTrad_leaf -
                               SCI::StefanBoltzmann(Tc) * 2.0;         // 计算净辐射
    }

    //----------------------------
    // Soil sunlit
    //----------------------------

    Tc = thermal.Tsoilsunlit;            // 阳光直射土壤温度
    ra = resist.raa_soil;                // 土壤空气阻力
    rs = biostate.rss;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hsoilsunlit = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEsoilsunlit = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nsoilsunlit = netrad.directVrad_soil + netrad.diffuseVrad_soil +
                           netrad.directTrad_soil + netrad.diffuseTrad_soil -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射


    //----------------------------
    // Soil shaded
    //----------------------------
    Tc = thermal.Tsoilshaded;            // 遮荫土壤温度
    ra = resist.raa_soil;                // 土壤空气阻力
    rs = biostate.rss;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hsoilshaded = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEsoilshaded = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nsoilshaded = netrad.diffuseVrad_soil + netrad.diffuseTrad_soil -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    // ec          = ea + (ei-ea)*ra./(ra+rs);         % [W m-2] vapour pressure at the leaf surface
    // Cc          = Ca - (Ca-Ci).*ra./(ra+rs);        % [umol m-2 s-1] CO2 concentration at the leaf surface
    // 计算土壤热通量
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
    // Roof sunlit
    //----------------------------
    Tc = Trs;            // 阳光直射土壤温度
    ra = resist.raa_roof;                // 土壤空气阻力
    rs = biostate.rssroof;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hroofsunlit = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEroofsunlit = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nroofsunlit = netrad.directVrad_roof + netrad.diffuseVrad_roof +
                           netrad.directTrad_roof + netrad.diffuseTrad_roof -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Roof shaded
    //----------------------------
    Tc = Trh;            // 阳光直射土壤温度
    ra = resist.raa_roof;                // 土壤空气阻力
    rs = biostate.rssroof;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hroofshaded = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEroofshaded = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nroofshaded = netrad.diffuseVrad_roof + netrad.diffuseTrad_roof -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Wall sunlit
    //----------------------------
    Tc = Tws;            // 阳光直射土壤温度
    ra = resist.raa_wall;                // 土壤空气阻力
    rs = biostate.rsswall;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hwallsunlit = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEwallsunlit = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nwallsunlit = netrad.directVrad_wall + netrad.diffuseVrad_wall +
                           netrad.directTrad_wall + netrad.diffuseTrad_wall -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Wall shaded
    //----------------------------
    Tc = Twh;            // 阳光直射土壤温度
    ra = resist.raa_wall;                // 土壤空气阻力
    rs = biostate.rsswall;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hwallshaded = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEwallshaded = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nwallshaded = netrad.directTrad_wall + netrad.diffuseTrad_wall -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Street sunlit
    //----------------------------
    Tc = Tss;            // 阳光直射土壤温度
    ra = resist.raa_street;                // 土壤空气阻力
    rs = biostate.rssstreet;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hstreetsunlit = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    float Hstreetsunlit1 = RHOA / (ra + biostate.rss) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEstreetsunlit = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nstreetsunlit = netrad.directVrad_street + netrad.diffuseVrad_street +
                           netrad.directTrad_street + netrad.diffuseTrad_street -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射

    //----------------------------
    // Street shaded
    //----------------------------
    Tc = Tsh;            // 阳光直射土壤温度
    ra = resist.raa_street;                // 土壤空气阻力
    rs = biostate.rssstreet;                   // 土壤表面阻力
    lambda = (2.501 - 0.002361 * (Tc - 273.15)) * 1E6;  // 蒸发热计算
    ei = SCI::es_fun(Tc - 273.15);       // 计算饱和水汽压
    qi = ei * e_to_q;                    // 计算比湿度
    heatflux.Hstreetshaded = RHOA / (ra + rs) * lambda * (qi - qa);  // 计算潜热通量
    heatflux.LEstreetshaded = (RHOA * CP) / ra * (Tc - Ta);          // 计算显热通量
    heatflux.Nstreetshaded = netrad.diffuseVrad_street + netrad.diffuseTrad_street -
                           SCI::StefanBoltzmann(Tc);               // 计算净辐射


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

#include "eb.h"


bool EB::rebalance(std::shared_ptr<PixelIO> pixelio)
{
    // 获取当前像素位置对应的数据节点索引
    int k_node = pixelio->k_node;

    // 通过引用获取不同类别的动态和静态变量
    NetRad &netrad = pixelio->m_pDynamicVariable->netrad;  // 净辐射数据
    Heatflux &heatflux = pixelio->m_pDynamicVariable->heatflux;  // 热通量数据
    BioState &biostate = pixelio->m_pDynamicVariable->biostate;  // 生物状态数据
    Resistance &resist = pixelio->m_pDynamicVariable->resistance;  // 阻力数据
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node];  // 气象数据
    Thermal &thermal = pixelio->m_pDynamicVariable->thermal;  // 温度数据
    Spectral &spectral = pixelio->m_pStaticVariable->spectal;  // 光谱数据

    // 获取冠层数据
    Canopy &canopy = pixelio->m_pInputset->canopy;

    // 定义用于计算的临时变量
    float innovation;
    float raa, Gleaf, LE, H, rss, net, bulk, Told, res, lambda, s, ei, emis;

    // 计算水汽压转化因子，根据空气中的水汽压和大气压计算
    float e_to_q = MH20 / MAIR / meteo.p;
    float Ta = meteo.ta;  // 气温 (Kelvin)
    float wc = 0.2;  // 调节温度变化的权重系数
    bool isclosed = true;  // 用于判断能量平衡是否关闭（是否达到稳定状态）

    // 判断冠层叶面积指数 (LAI) 是否大于 0，即是否存在植被
    if (canopy.lai > 0) {
        // 处理阳光照射的叶片部分
        raa = resist.raa_leaf;  // 叶片的气动阻力
        rss = biostate.rssunlit;  // 阳光照射叶片的表面阻力
        LE = heatflux.LEleafsunlit;  // 阳光照射叶片的潜热通量
        Told = thermal.Tleafsunlit;  // 阳光照射叶片的温度
        H = heatflux.Hleafsunlit;  // 阳光照射叶片的显热通量
        Gleaf = 0;  // 叶片的导热量，通常设为0
        net = heatflux.Nleafsunlit;  // 阳光照射叶片的净辐射
        bulk = LE + Gleaf;  // 总的能量平衡
        res = net - bulk - H;  // 能量平衡的剩余量（残差）

        // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
        lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;  // 潜热（单位：J/kg）
        ei = SCI::es_fun(Told - 273.15);  // 饱和水汽压 (Pa)
        s = SCI::s_fun(ei, Told - 273.15);  // 饱和水汽压曲线的斜率
        emis = 1.0 - spectral.leafRefl_ir - spectral.leafTran_ir;  // 叶片的发射率

        // 更新阳光照射叶片的温度 (Kelvin)
        thermal.Tleafsunlit = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));

        // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
        if (res > RAD_THRESHOLD) isclosed = false;

        // 处理阴影叶片部分
        raa = resist.raa_leaf;  // 叶片的气动阻力
        rss = biostate.rsshaded;  // 阴影叶片的表面阻力
        LE = heatflux.LEleafshaded;  // 阴影叶片的潜热通量
        Told = thermal.Tleafshaded;  // 阴影叶片的温度
        H = heatflux.Hleafshaded;  // 阴影叶片的显热通量
        Gleaf = 0;  // 阴影叶片的导热量，通常设为0
        net = heatflux.Nleafshaded;  // 阴影叶片的净辐射
        bulk = LE + Gleaf;  // 总的能量平衡
        res = net - bulk - H;  // 能量平衡的剩余量（残差）

        // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
        lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;
        ei = SCI::es_fun(Told - 273.15);
        s = SCI::s_fun(ei, Told - 273.15);
        emis = 1.0 - spectral.leafRefl_ir - spectral.leafTran_ir;

        // 更新阴影叶片的温度 (Kelvin)
        thermal.Tleafshaded = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));

        // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
        if (res > RAD_THRESHOLD) isclosed = false;
    }

    // 处理阳光照射的土壤部分
    raa = resist.raa_soil;  // 土壤的气动阻力
    rss = biostate.rss;  // 阳光照射土壤的表面阻力
    LE = heatflux.LEsoilsunlit;  // 阳光照射土壤的潜热通量
    Told = thermal.Tsoilsunlit;  // 阳光照射土壤的温度
    H = heatflux.Hsoilsunlit;  // 阳光照射土壤的显热通量
    Gleaf = heatflux.Gsunlit;  // 阳光照射土壤的导热量
    net = heatflux.Nsoilsunlit;  // 阳光照射土壤的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）

    // 计算水的潜热 (lambda) 和斜率 (s)，以及土壤的发射率 (emis)
    lambda = (2.501-0.002361*(Told-273.15))*1E6;
    ei = SCI::es_fun(Told-273.15);
    s = SCI::s_fun(ei,Told-273.15);
    emis = 1.0 - spectral.soilRefl_ir;  // 土壤的发射率

    // 更新阳光照射土壤的温度 (Kelvin)
    thermal.Tsoilsunlit = Told + wc*res/((RHOA*CP)/raa + RHOA*lambda*e_to_q*s/(raa+rss) + 4.0*emis*SIGMASB*std::pow(Told,3));

    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if(res > RAD_THRESHOLD) isclosed = false;

    // 处理阴影土壤部分
    raa = resist.raa_soil;  // 土壤的气动阻力
    rss = biostate.rss;  // 阴影土壤的表面阻力
    LE = heatflux.LEsoilshaded;  // 阴影土壤的潜热通量
    Told = thermal.Tsoilshaded;  // 阴影土壤的温度
    H = heatflux.Hsoilshaded;  // 阴影土壤的显热通量
    Gleaf = heatflux.Gshaded;  // 阴影土壤的导热量
    net = heatflux.Nsoilshaded;  // 阴影土壤的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）

    // 计算水的潜热 (lambda) 和斜率 (s)，以及土壤的发射率 (emis)
    lambda = (2.501-0.002361*(Told-273.15))*1E6;
    ei = SCI::es_fun(Told-273.15);
    s = SCI::s_fun(ei,Told-273.15);
    emis = 1.0 - spectral.soilRefl_ir;  // 土壤的发射率

    // 更新阴影土壤的温度 (Kelvin)
    thermal.Tsoilshaded = Told + wc*res/((RHOA*CP)/raa + RHOA*lambda*e_to_q*s/(raa+rss) + 4.0*emis*SIGMASB*std::pow(Told,3));

    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if(res > RAD_THRESHOLD) isclosed = false;

    // 检查是否超过温度阈值，如果超过，则将温度重置为默认值，并将系统标记为不稳定
    if(thermal.Tsoilsunlit > TMAX_THRESHOLD || thermal.Tleafsunlit > TMAX_THRESHOLD)
    {
        thermal.Tsoilsunlit = Ta + 2.0 ;
        thermal.Tleafsunlit = Ta + 1.0 ;
        isclosed = false;
    }
    if(thermal.Tleafshaded < TMIN_THRESHOLD || thermal.Tsoilshaded < TMIN_THRESHOLD)
    {
        thermal.Tleafshaded = Ta + 2.0 ;
        thermal.Tsoilshaded = Ta + 1.0 ;
        isclosed = false;
    }

    // 检查温度是否为非数值 (NaN)，如果是，则重置为默认值
    if(isnan(thermal.Tleafsunlit)) thermal.Tleafsunlit = Ta + 2.0 ;
    if(isnan(thermal.Tleafshaded)) thermal.Tleafshaded = Ta + 1.0 ;
    if(isnan(thermal.Tsoilsunlit)) thermal.Tsoilsunlit = Ta + 2.0 ;
    if(isnan(thermal.Tsoilshaded)) thermal.Tsoilshaded = Ta + 1.0 ;

    // 返回是否已达到稳定状态
    return isclosed;
}

bool EB::rebalance_urban(std::shared_ptr<PixelIO> pixelio)
{
    // 获取当前像素位置对应的数据节点索引
    int k_node = pixelio->k_node;

    // 通过引用获取不同类别的动态和静态变量
    NetRad &netrad = pixelio->m_pDynamicVariable->netrad;  // 净辐射数据
    Heatflux &heatflux = pixelio->m_pDynamicVariable->heatflux;  // 热通量数据
    BioState &biostate = pixelio->m_pDynamicVariable->biostate;  // 生物状态数据
    Resistance &resist = pixelio->m_pDynamicVariable->resistance;  // 阻力数据
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node];  // 气象数据
    Thermal &thermal = pixelio->m_pDynamicVariable->thermal;  // 温度数据
    Spectral &spectral = pixelio->m_pStaticVariable->spectal;  // 光谱数据
    Urban &urban =pixelio->m_pInputset->urban;
    Canopy &canopy = pixelio->m_pInputset->canopy;

    // 定义用于计算的临时变量
    float innovation;
    float raa, Gleaf, LE, H, rss, net, bulk, Told, res, lambda, s, ei, emis;

    // 计算水汽压转化因子，根据空气中的水汽压和大气压计算
    float e_to_q = MH20 / MAIR / meteo.p;
    float Ta = meteo.ta;  // 气温 (Kelvin)
    float wc = 0.2;  // 调节温度变化的权重系数
    bool isclosed = true;  // 用于判断能量平衡是否关闭（是否达到稳定状态）

    float emis_roof = 0.950;
    float emis_wall = 0.920;
    float emis_street = 0.955;

    //----------------------------
    // Roof sunlit
    //----------------------------
    // 处理阳光照射的屋顶部分
    raa = resist.raa_roof;  // 叶片的气动阻力
    rss = biostate.rssroof;  // 阳光照射叶片的表面阻力
    LE = heatflux.LEroofsunlit;  // 阳光照射叶片的潜热通量
    Told = thermal.Troofsunlit;  // 阳光照射叶片的温度
    H = heatflux.Hroofsunlit;  // 阳光照射叶片的显热通量
    Gleaf = heatflux.Groofsunlit;  // 叶片的导热量，通常设为0
    net = heatflux.Nroofsunlit;  // 阳光照射叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;  // 潜热（单位：J/kg）
    ei = SCI::es_fun(Told - 273.15);  // 饱和水汽压 (Pa)
    s = SCI::s_fun(ei, Told - 273.15);  // 饱和水汽压曲线的斜率
    emis = emis_roof;  // 叶片的发射率
    // 更新阳光照射叶片的温度 (Kelvin)
    thermal.Troofsunlit = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));

    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;
    if (res < RAD_THRESHOLD)
    {
        float temp = 0;
    };
    //----------------------------
    // Roof shaded
    //----------------------------
    // 处理阴影叶片部分
    raa = resist.raa_roof;  // 叶片的气动阻力
    rss = biostate.rssroof;  // 阴影叶片的表面阻力
    LE = heatflux.LEroofshaded;  // 阴影叶片的潜热通量
    Told = thermal.Troofshaded;  // 阴影叶片的温度
    H = heatflux.Hroofshaded;  // 阴影叶片的显热通量
    Gleaf = heatflux.Groofshaded;  // 阴影叶片的导热量，通常设为0
    net = heatflux.Nroofshaded;  // 阴影叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;
    ei = SCI::es_fun(Told - 273.15);
    s = SCI::s_fun(ei, Told - 273.15);
    emis = emis_roof;
    // 更新阴影叶片的温度 (Kelvin)
    thermal.Troofshaded = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));
    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;

    //----------------------------
    // Wall sunlit
    //----------------------------
    // 处理阳光照射的屋顶部分
    raa = resist.raa_wall;  // 叶片的气动阻力
    rss = biostate.rsswall;  // 阳光照射叶片的表面阻力
    LE = heatflux.LEwallsunlit;  // 阳光照射叶片的潜热通量
    Told = thermal.Twallsunlit;  // 阳光照射叶片的温度
    H = heatflux.Hwallsunlit;  // 阳光照射叶片的显热通量
    Gleaf = heatflux.Gwallsunlit;  // 叶片的导热量，通常设为0
    net = heatflux.Nwallsunlit;  // 阳光照射叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;  // 潜热（单位：J/kg）
    ei = SCI::es_fun(Told - 273.15);  // 饱和水汽压 (Pa)
    s = SCI::s_fun(ei, Told - 273.15);  // 饱和水汽压曲线的斜率
    emis = emis_wall;  // 叶片的发射率
    // 更新阳光照射叶片的温度 (Kelvin)
    thermal.Twallsunlit = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));
    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;

    //----------------------------
    // Wall shaded
    //----------------------------
    // 处理阴影叶片部分
    raa = resist.raa_wall;  // 叶片的气动阻力
    rss = biostate.rsswall;  // 阴影叶片的表面阻力
    LE = heatflux.LEwallshaded;  // 阴影叶片的潜热通量
    Told = thermal.Twallshaded;  // 阴影叶片的温度
    H = heatflux.Hwallshaded;  // 阴影叶片的显热通量
    Gleaf = heatflux.Gwallshaded;  // 阴影叶片的导热量，通常设为0
    net = heatflux.Nwallshaded;  // 阴影叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;
    ei = SCI::es_fun(Told - 273.15);
    s = SCI::s_fun(ei, Told - 273.15);
    emis = emis_wall;
    // 更新阴影叶片的温度 (Kelvin)
    thermal.Twallshaded = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));
    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;

    //----------------------------
    // Street sunlit
    //----------------------------
    // 处理阳光照射的屋顶部分
    raa = resist.raa_street;  // 叶片的气动阻力
    rss = biostate.rssstreet;  // 阳光照射叶片的表面阻力
    LE = heatflux.LEstreetsunlit;  // 阳光照射叶片的潜热通量
    Told = thermal.Tstreetsunlit;  // 阳光照射叶片的温度
    H = heatflux.Hstreetsunlit;  // 阳光照射叶片的显热通量
    Gleaf = heatflux.Gstreetsunlit;  // 叶片的导热量，通常设为0
    net = heatflux.Nstreetsunlit;  // 阳光照射叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;  // 潜热（单位：J/kg）
    ei = SCI::es_fun(Told - 273.15);  // 饱和水汽压 (Pa)
    s = SCI::s_fun(ei, Told - 273.15);  // 饱和水汽压曲线的斜率
    emis = emis_street;  // 叶片的发射率
    // 更新阳光照射叶片的温度 (Kelvin)
    thermal.Tstreetsunlit = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));
    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;

    //----------------------------
    // Street shaded
    //----------------------------
    // 处理阴影叶片部分
    raa = resist.raa_street;  // 叶片的气动阻力
    rss = biostate.rssstreet;  // 阴影叶片的表面阻力
    LE = heatflux.LEstreetshaded;  // 阴影叶片的潜热通量
    Told = thermal.Tstreetshaded;  // 阴影叶片的温度
    H = heatflux.Hstreetshaded;  // 阴影叶片的显热通量
    Gleaf = heatflux.Gstreetshaded;  // 阴影叶片的导热量，通常设为0
    net = heatflux.Nstreetshaded;  // 阴影叶片的净辐射
    bulk = LE + Gleaf;  // 总的能量平衡
    res = net - bulk - H;  // 能量平衡的剩余量（残差）
    // 计算水的潜热 (lambda) 和斜率 (s)，以及叶片的发射率 (emis)
    lambda = (2.501 - 0.002361 * (Told - 273.15)) * 1E6;
    ei = SCI::es_fun(Told - 273.15);
    s = SCI::s_fun(ei, Told - 273.15);
    emis = emis_street;
    // 更新阴影叶片的温度 (Kelvin)
    thermal.Tstreetshaded = Told + wc * res / ((RHOA * CP) / raa + RHOA * lambda * e_to_q * s / (raa + rss) + 4.0 * emis * SIGMASB * std::pow(Told, 3));
    // 如果能量平衡的剩余量超过预设的辐射阈值，则表明系统不稳定
    if (res > RAD_THRESHOLD) isclosed = false;

    // 检查是否超过温度阈值，如果超过，则将温度重置为默认值，并将系统标记为不稳定
    if(thermal.Troofsunlit > TMAX_THRESHOLD || thermal.Twallsunlit > TMAX_THRESHOLD || thermal.Tstreetsunlit > TMAX_THRESHOLD)
    {
        thermal.Troofsunlit = Ta + 2.0 ;
        thermal.Twallsunlit = Ta + 2.0 ;
        thermal.Tstreetsunlit = Ta + 1.0 ;
        isclosed = false;
    }
    if(thermal.Troofshaded < TMIN_THRESHOLD || thermal.Twallshaded < TMIN_THRESHOLD || thermal.Tstreetshaded < TMIN_THRESHOLD)
    {
        thermal.Troofshaded = Ta + 2.0 ;
        thermal.Twallshaded = Ta + 2.0 ;
        thermal.Tstreetshaded = Ta + 1.0 ;
        isclosed = false;
    }

    // 检查温度是否为非数值 (NaN)，如果是，则重置为默认值
    if(isnan(thermal.Troofsunlit)) thermal.Troofsunlit = Ta + 2.0 ;
    if(isnan(thermal.Twallsunlit)) thermal.Twallsunlit = Ta + 1.0 ;
    if(isnan(thermal.Tstreetsunlit)) thermal.Tstreetsunlit = Ta + 2.0 ;
    if(isnan(thermal.Troofshaded)) thermal.Troofshaded = Ta + 1.0 ;
    if(isnan(thermal.Twallshaded)) thermal.Twallshaded = Ta + 2.0 ;
    if(isnan(thermal.Tstreetshaded)) thermal.Tstreetshaded = Ta + 1.0 ;

    // 返回是否已达到稳定状态
    return isclosed;
}
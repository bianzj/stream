#include "aerodynamic.h"



// 计算动量稳定修正
float psim(float z,float L)
{
    float xz,pm,temp;
    temp = 0.25;
    xz = (1-16*z/L);
    xz = pow(xz,temp);
    pm=0.0;
    if(L<-4) // 不稳定条件
    {
        pm = 2*log(0.5+0.5*xz)+log(0.5+0.5*xz*xz)-2*atan(xz)+PI/2;
    }
    if(L>4e3) // 稳定条件
    {
        pm = -5*z/L;
    }
    return pm;
}

// 计算热稳定修正
float psih(float z,float L)
{
    float xz,ph,temp;
    xz = (1-16*z/L);
    temp = 0.25;
    xz = pow(xz,temp);
    ph=0.0;
    if(L<-4) // 不稳定条件
    {
        ph = 2*log((1+xz*xz)*0.5);
    }
    if(L>4e3) // 稳定条件
    {
        ph = -5*z/L;
    }
    return ph;
}

// 计算额外的稳定性修正
float phstar(float z,float zr,float d,float L)
{
    float xz,phs,temp;
    xz= (1-16.0*z/L);
    temp = 0.25;
    xz = pow(xz,temp);
    phs=0.0;
    if(L<-4) // 不稳定条件
    {
        phs = (z-d)/(zr-d)*(xz*xz-1)/(xz*xz+1);
    }
    if(L>4e3) // 稳定条件
    {
        phs = -5*z/L;
    }
    return phs;
}

void Aerodynamic::aeresist(std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio) {
    // 引用不同的数据结构和变量
    Canopy &canopy = pixelio->m_pInputset->canopy; // 冠层数据
    AeroCoeff &aerocoeff = defined->m_aerocoeff;  // 气动系数
    int k_node = pixelio->k_node; // 当前节点索引
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node]; // 气象数据
    Resistance &resist = pixelio->m_pDynamicVariable->resistance; // 阻力数据
    MeteoMeta &meteod = pixelio->m_pInputset->meta; // 气象元数据

    // 从冠层和气象数据中提取必要的信息
    float lai = canopy.lai;   // 叶面积指数
    float hc = canopy.height; // 冠层高度
    float u = meteo.u;        // 风速
    float z = meteod.z;       // 测量高度
    float psicor = aerocoeff.Psicor; // 心理相关系数

    // 常量定义
    float kappa = 0.4; // von Karman常数
    float cp = 1200;   // 比热容
    float pa = 1205;   // 空气密度

    // 定义变量以存储计算结果
    float d1, z0m1, d2, z0m2, z1, z2, rasoil;
    float d, z0m;
    float L = resist.L; // Monin-Obukhov长度

    // 判断是否有植被
    if (lai <= 0) {
        // 无植被：裸露土壤
        d = 0;
        z0m = 0.01; // 默认土壤粗糙度长度
        float pm_z = psim(z - d, L); // 计算动量稳定修正
        float ustar = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z)); // 计算摩擦速度
        float rasoil = 1.0 / (kappa * ustar) * log(z); // 计算土壤阻力
        resist.ustar = ustar; // 保存摩擦速度
        resist.raa_leaf = 0;  // 没有叶面阻力
        resist.raa_soil = rasoil; // 保存土壤阻力
    } else {
        // 有植被：计算与植被相关的阻力
//        float sq = sqrt(aerocoeff.CD1 * lai); // 计算与叶面积指数相关的参数
        float sq = sqrt(aerocoeff.CD1 * (lai / 2.0)); // 计算与叶面积指数相关的参数
        float g1 = Utils::max(3.3, sqrt(aerocoeff.CSSOIL + aerocoeff.Cd * (lai / 2.0)));
        float n = aerocoeff.Cd * lai / (2.0 * kappa * kappa);
        float zr = 2.5 * hc; // 计算参考高度

        // 计算零平面位移高度和动量粗糙度长度
        d = hc * (1 - (1 - exp(-sq)) / sq);
        z0m = (hc - d) * exp(-kappa * g1 + psicor);

        if (hc > z) {
            float pm_z = psim(z - d, L);

            float ustar_temp = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z));

            float pm_h = psim(hc - d, L);
            u = Utils::max(0.01, ustar_temp / kappa * (log((hc - d) / z0m) - pm_h));
        }

        // 计算动量和热稳定修正
        float pm_z = psim(z - d, L);
        float ph_z = psih(z - d, L);
        float pm_h = psim(hc - d, L);
        float ph_zr = psih(zr - d, L);
        float phs_zr = phstar(zr, zr, d, L);
        float phs_h = phstar(hc, zr, d, L);

        // 计算摩擦速度和热扩散率
        float ustar = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z));
        float kh = kappa * ustar * (hc - d);
        if (L < -4) kh = kappa * ustar * (zr - d) * sqrt(1 - 16 * (hc - d) / L);
        if (L > 4e3) kh = kappa * ustar * (zr - d) / (1 + 5 * (hc - d) / L);

        float uh = Utils::max(ustar / kappa * (log((hc - d) / z0m) - pm_h), 0.01);

        // 计算叶面气动阻力
        float rai, rar;
        if (z > zr) {
            rai = (1.0 / (kappa * ustar) * (log((z - d) / (zr - d)) - ph_z + ph_zr));
        } else {
            rai = 0;
        }
        rar = 1.0 / (kappa * ustar) * ((zr - hc) / (zr - d)) - phs_zr + phs_h;

        kh = kappa * ustar * (zr - d);
        float rac = hc * sinhf(n) / (n * kh) * (log((exp(n) - 1) / (exp(n) + 1)) -
                                                log((exp(n * (z0m + d) / hc) - 1) / (exp(n * (z0m + d) / hc) + 1)));
        float uz0 = uh * exp(n * ((z0m + d) / hc - 1));
        float rbc = 70 / lai * sqrt(canopy.leafwidth / uz0);

        // 计算土壤气动阻力
        float rws = hc * sinh(n) / (n * kh) * (log((exp(n * (z0m + d) / hc) - 1) / (exp(n * (z0m + d) / hc) + 1)) -
                                               log((exp(n * (.01) / hc) - 1) / (exp(n * (.01) / hc) + 1)));
        float rbs = aerocoeff.rbs;

        // 保存计算结果
        resist.ustar = ustar;
        resist.raa_leaf = rai + rar + rac + rbc;
        resist.raa_soil = rai + rar + rac + rws + rbs;
    }
}

void Aerodynamic::aeresist_urban(std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio) {
    // 引用不同的数据结构和变量
    Canopy &canopy = pixelio->m_pInputset->canopy; // 冠层数据
    AeroCoeff &aerocoeff = defined->m_aerocoeff;  // 气动系数
    int k_node = pixelio->k_node; // 当前节点索引
    Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node]; // 气象数据
    Resistance &resist = pixelio->m_pDynamicVariable->resistance; // 阻力数据
    MeteoMeta &meteod = pixelio->m_pInputset->meta; // 气象元数据
    Urban &urban = pixelio->m_pInputset->urban;

    urban.height = 20;
    urban.width = 10;
    urban.length = 10;
    urban.density = 0.0015;

    // 从冠层和气象数据中提取必要的信息
    float bai = urban.bai;
    float hc = urban.height;
    float width = urban.width;
    float length = urban.length;
    float density = urban.density;
    float pixel_area = 1000000; //像元面积为1000m * 1000m
    float u = meteo.u;        // 风速
    float z = meteod.z;       // 测量高度
    float psicor = aerocoeff.Psicor;

    // 常量定义
    float kappa = 0.4; // von Karman常数
    float cp = 1200;   // 比热容
    float pa = 1205;   // 空气密度

    // 定义变量以存储计算结果
    float d1, z0m1, d2, z0m2, z1, z2, rasoil;
    float d, z0m;
    float CD_wall = 0.2;
    float CD_roof = 1.5;
    float L = resist.L; // Monin-Obukhov长度

    // (1) 每栋建筑的占地面积
    float building_area = length * width;
    // (2) 建筑总数
    float N = density * pixel_area;
    // (3) 建筑总占地面积
    float total_building_area = N * building_area;
    // (4) 街道总面积
    float street_area = pixel_area - total_building_area;
    // (5) 平均建筑间距 (假设建筑均匀分布)
    float building_spacing = sqrt(street_area / N) - width;
    // (6) 平均街道宽度
    float street_width = building_spacing - length;

    // 计算城市表面的零平面位移和粗糙度长度
    z0m = 0.15; //屋顶粗糙度长度，Masson(2000)
    float sq = sqrt(aerocoeff.CD1 * bai); // 计算叶面积指数相关参数
    float g1 = Utils::max(3.3, sqrt(aerocoeff.CSSOIL + aerocoeff.Cd * bai / 2.0)); // 计算阻力参数
    float n = aerocoeff.Cd * bai / (2.0 * kappa * kappa); // 风衰减系数
    float zr = 2.5 * hc; // roughness height, 设置为建筑高度均值
//    float zr = 0.5 * ((density * pixel_area * hc * width) / pixel_area) * hc; // roughness height, MACDONALD（1998）
    // 计算零平面位移高度和动量粗糙度长度
    float lambda_p = total_building_area / pixel_area;  //Macdonald, 1998
    float m_a = 4.0; //经验系数取值
    d = (1 + std::pow(m_a, -lambda_p) * (lambda_p - 1)) * hc;
//    zr = d + 0.1 * hc;

    // 计算稳定性修正
    float pm_z = psim(z - d, L);
    float ph_z = psih(z - d, L);
    float pm_h = psim(hc - d, L);
    float ph_zr = psih(zr - d, L);
    float phs_zr = phstar(zr, zr, d, L);
    float phs_h = phstar(hc, zr, d, L);

    // 计算摩擦速度和热扩散率
    float ustar = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z));
    float kh = kappa * ustar * (hc - d); // 初步热扩散率计算

    // 根据稳定性进一步调整热扩散率
    if (L < -4) kh = kappa * ustar * (zr - d) * sqrt(1 - 16 * (hc - d) / L);
    if (L > 4e3) kh = kappa * ustar * (zr - d) / (1 + 5 * (hc - d) / L);

    // 计算冠层顶风速
    float uh = Utils::max(ustar / kappa * (log((hc - d) / z0m) - pm_h), 0.01);

    // 计算叶面气动阻力
    float rai, rar;
    if (z > zr) {
        // 当测量高度大于参考高度时
        rai = (1.0 / (kappa * ustar) * (log((z - d) / (zr - d)) - ph_z + ph_zr));
    } else {
        rai = 0; // 否则，叶面气动阻力为0
    }
    // 计算冠层内部阻力
    rar = 1.0 / (kappa * ustar) * ((zr - hc) / (zr - d)) - phs_zr + phs_h;

    // 计算墙壁和街道空气阻抗，Mason 2000
    float z0m_town = Utils::min(5.0, hc * 0.1); // 动量粗糙度通常为建筑物高度的10%
    float Ua = uh;         // 风速矢量的大小
    float delta_z = 0.0;    // 参考高度与障碍物平均高度的差

    //计算稳定性修正和Cd，Mascart et al. (1995)
    float k = 0.35;
    float R = 0.74;
    float psi_m1 = 0;
    float psi_m2 = 0;
    if (L>4e3) {
        // Stable conditions
        psi_m1 = -4.7 * (z / L);
    }
    if(L<-4) {
        // Unstable conditions
        float x = std::pow(1 - 15 * (z / L), 0.25);
        psi_m1 = std::log(std::pow((1 + x) / 2, 2) * (1 + x * x) / 2) - 2 * std::atan(x) + M_PI / 2;
    }
    if (L>4e3) {
        // Stable conditions
        psi_m2 = -4.7 * (z0m / L) / R;
    }
    if(L<-4) {
        // Unstable conditions
        float y = std::pow(1 - 9 * (z0m / L), 0.5);
        psi_m2 = std::log(std::pow((1 + y) / 2, 2));
    }
    float log_term = std::log(z / z0m);
    float stability_correction = psi_m1 - psi_m2;
    float denominator = log_term - stability_correction;
    float Cd = ((k * k) / R) / std::pow((denominator * denominator), 2);


    // 计算墙壁和街道的阻抗，Masson(2000)
    float W_can, U_can, RES_r, RES_w, RES_s;      // 空气动力学阻抗
    W_can = sqrt(Cd) * std::abs(Ua);
    float exp_term = std::exp(-0.25 * (hc / width));
    float log_term_1 = std::log(hc / 3.0 / z0m_town);
    float log_term_2 = std::log((delta_z + hc / 3.0) / z0m_town);
    U_can = (2.0 / M_PI) * exp_term * (log_term_1 / log_term_2) * std::abs(Ua);

    // RES_r = (std::log(10.0 / 0.01))/ (ustar * kappa);
    // float Zu = 15.0;
    // float Zt = 2.0;
    // float Z0M = 1.5;
    // float Z0H = Z0M / 1000.0;
    float Zu = 10.0;
    float Zt = 2.0;
    float Z0M = hc / 8.0;
    float Z0H = Z0M / 7.0;
    ustar = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z));
    // RES_r = 1.0 / (kappa * ustar) * log(z); // 计算土壤气动阻力
    // RES_r = ((log((Zu - d)/Z0M) - psi_m1) * (log((Zt - d)/Z0H) - psi_m2)) / (kappa * kappa * u);
    RES_r = (std::log(10.0 / 0.01))/ (ustar * kappa);
    RES_s = RES_w = (11.8 + 4.2 * (std::pow(U_can, 2) + std::pow(W_can, 2)));  // 计算阻力并赋值给 RES_r 和 RES_w

    resist.ustar = ustar;
    resist.raa_roof = RES_r;
    resist.raa_street = RES_s;
    resist.raa_wall = RES_w;
}

// void Aerodynamic::aeresist_urban(std::shared_ptr<Defined> defined, std::shared_ptr<PixelIO> pixelio) {
//     // 引用不同的数据结构和变量
//     Canopy &canopy = pixelio->m_pInputset->canopy; // 冠层数据
//     AeroCoeff &aerocoeff = defined->m_aerocoeff;  // 气动系数
//     int k_node = pixelio->k_node; // 当前节点索引
//     Meteo &meteo = pixelio->m_pInputset->vMeteo[k_node]; // 气象数据
//     Resistance &resist = pixelio->m_pDynamicVariable->resistance; // 阻力数据
//     MeteoMeta &meteod = pixelio->m_pInputset->meta; // 气象元数据
//     Urban &urban = pixelio->m_pInputset->urban;
//
//     urban.height = 20;
//     urban.width = 10;
//     urban.length = 10;
//     urban.density = 0.0015;
//
//     // 从冠层和气象数据中提取必要的信息
//     float bai = urban.bai;
//     float height = urban.height;
//     float width = urban.width;
//     float length = urban.length;
//     float density = urban.density;
//     float pixel_area = 1000000; //像元面积为1000m * 1000m
//     float u = meteo.u;        // 风速
//     float z = meteod.z;       // 测量高度
//     float psicor = aerocoeff.Psicor;
//
//     // 常量定义
//     float kappa = 0.4; // von Karman常数
//     float cp = 1200;   // 比热容
//     float pa = 1205;   // 空气密度
//
//     // 定义变量以存储计算结果
//     float d1, z0m1, d2, z0m2, z1, z2, rasoil;
//     float d, z0m;
//     float CD_wall = 0.2;
//     float CD_roof = 1.5;
//     float L = resist.L; // Monin-Obukhov长度
//
//     float building_area = length * width;
//     float building_num = density * pixel_area;
//     float total_building_area = building_num * building_area;
//     float street_area = pixel_area - total_building_area;
//     float building_spacing = sqrt(street_area / building_num) - width;
//     float street_width = building_spacing - length;
//
//     // 计算城市表面的零平面位移和粗糙度长度
//     z0m = 0.15; //屋顶粗糙度长度，Masson(2000)
//     float sq = sqrt(aerocoeff.CD1 * bai); // 计算叶面积指数相关参数
//     float g1 = Utils::max(3.3, sqrt(aerocoeff.CSSOIL + aerocoeff.Cd * bai / 2.0)); // 计算阻力参数
//     float n = aerocoeff.Cd * bai / (2.0 * kappa * kappa); // 风衰减系数
//     float zr = 2.5 * height; // roughness height, 设置为建筑高度均值
//     // 计算零平面位移高度和动量粗糙度长度
//     float lambda_p = total_building_area / pixel_area;  //Macdonald, 1998
//     float m_a = 4.0; //经验系数取值
//     d = (1 + std::pow(m_a, -lambda_p) * (lambda_p - 1)) * height;
//
//     // 计算稳定性修正
//     float pm_z = psim(z - d, L);
//     float ph_z = psih(z - d, L);
//     float pm_h = psim(height - d, L);
//     float ph_zr = psih(zr - d, L);
//     float phs_zr = phstar(zr, zr, d, L);
//     float phs_h = phstar(height, zr, d, L);
//
//     // 计算摩擦速度和热扩散率
//     float ustar = Utils::max(0.001, kappa * u / (log((z - d) / z0m) - pm_z));
//     float kh = kappa * ustar * (height - d); // 初步热扩散率计算
//     // 根据稳定性进一步调整热扩散率
//     if (L < -4) kh = kappa * ustar * (zr - d) * sqrt(1 - 16 * (height - d) / L);
//     if (L > 4e3) kh = kappa * ustar * (zr - d) / (1 + 5 * (height - d) / L);
//     // 计算冠层顶风速
//     float uh = Utils::max(ustar / kappa * (log((height - d) / z0m) - pm_h), 0.01);
//
//     double Cd =  1.0 / log(z / z0m);
//     double delta_z = height / 3.0;
//     double W_can = sqrt(Cd) * fabs(uh);  // 计算公式 W_can = sqrt(C_d) * |U_a|
//
//     double term1 = (2 / M_PI) * exp(-0.25 * (height / width)); // 计算前面的指数部分
//     double term2 = log((height / 3) / z0m) / log((delta_z + height / 3) / z0m); // 计算对数部分
//     double U_can = term1 * term2 * fabs(uh); // 计算最终的水平风速 U_can
//
//     // 计算空气动力学阻抗 RES
//     double RES_w = 1.0/ (11.8 + 4.2 * sqrt(U_can * U_can + W_can * W_can));
//     double RES_s = 1.0 / (11.8 + 4.2 * sqrt(U_can * U_can + W_can * W_can));
//
//     resist.ustar = ustar;
//     resist.raa_wall = RES_w;
//     resist.raa_street = RES_s;
// }
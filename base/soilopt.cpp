
#include "soilopt.h"

/*

in this case, the soil reflectance will be calculated;

*/


void SoilOpt::bsm(OptCoeff bsmCoeff, BSMParam bsm, float SMC, Spectral &spectral) {
    // 定义一个名为 bsm 的 SoilOpt 类成员函数，该函数接受四个参数：
    // bsmCoeff：光学系数
    // bsm：BSM 参数
    // SMC：土壤水分含量
    // spectral：光谱数据

    std::vector<float> &gsv1_ = bsmCoeff.gsv1_;
    std::vector<float> &gsv2_ = bsmCoeff.gsv2_;
    std::vector<float> &gsv3_ = bsmCoeff.gsv3_;
    std::vector<float> &kw_ = bsmCoeff.kw_;
    std::vector<float> &nw_ = bsmCoeff.nw_;
    // 从 bsmCoeff 中引用五个光学系数向量

    float B = bsm.BSMBrightness;
    float lat = bsm.BSMlat;
    float lon = bsm.BSMlon;
    // 从 bsm 中提取亮度、纬度和经度参数

    float SMCp = 0.25;
    float film = 0.015;
    float rd = 3.1415926 / 180.0;
    // 定义常数 SMCp, film 和度数转弧度的转换因子 rd

    float f1 = B * sin(lat * rd);
    float f2 = B * cos(lat * rd) * sin(lon * rd);
    float f3 = B * cos(lat * rd) * cos(lon * rd);
    // 根据亮度和经纬度计算三个因子 f1, f2, f3

    std::vector<float> rdry_, rwet_;
    // 定义两个向量 rdry_ 和 rwet_，用于存储干燥和湿润的反射率

    for (int k = 0; k < gsv1_.size(); k++) {
        rdry_.push_back(gsv1_[k] * f1 + gsv2_[k] * f2 + gsv3_[k] * f3);
    }
    // 计算干燥反射率 rdry_，对于每个波长，基于 gsv1_, gsv2_, gsv3_ 和 f1, f2, f3 进行线性组合

    for (int k = 0; k < gsv1_.size(); k++) {
        float rdry = rdry_[k];
        float tw = exp(-kw_[k] * film);
        float rbac = 1 - (1 - rdry_[k]) * (rdry_[k] * SCI::calctav(90, 2.0 / nw_[k]) / SCI::calctav(90, 2.0) + 1 - rdry_[k]);
        float p = 1 - SCI::calctav(90, nw_[k]) / nw_[k] / nw_[k];
        float Rw = 1 - SCI::calctav(40, nw_[k]);
        float Radd = (1 - Rw) * (1 - p) * rbac / (1 - p * rbac);
        float mu = (SMC - 0.05) / SMCp;
        float fdry = exp(-mu);
        float fmul = (exp(tw * mu) - 1) * fdry;
        float rwet = rdry * fdry + Rw * (1 - fdry) + Radd * fmul;
        spectral.soilRefl_[k] = rwet;
    }
    // 计算湿润反射率 rwet，并存储在 spectral.soilRefl_ 中
    // - 对于每个波长：
    //   - 从 rdry_ 中获取对应的干燥反射率 rdry
    //   - 计算湿润土壤的各项参数：
    //     - tw：薄膜透射率
    //     - rbac：背景反射率调整因子
    //     - p：光学路径因子
    //     - Rw：湿润土壤反射率
    //     - Radd：附加反射率
    //     - mu：土壤水分校正因子
    //     - fdry：干燥因子
    //     - fmul：湿润调整因子
    //   - 综合计算湿润反射率 rwet 并存储在 spectral 的 soilRefl_ 向量中

}



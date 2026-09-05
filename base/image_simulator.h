#pragma once

#include <vector>

#include "scifuns.h"
#include "structs.h"
#include "radiative_lai.h"

// 光学图像的逐波段结果。辐亮度采用 Lambertian 近似：L = pi * rho / pi。
struct OpticalImageResult
{
    std::vector<float> wavelength_nm;
    std::vector<float> leafRadiance;
    std::vector<float> soilRadiance;
    std::vector<float> totalRadiance;
    std::vector<float> totalReflectance;
    std::vector<float> leafAbsorbed;
    std::vector<float> soilAbsorbed;
};

// 基于已有四类组分温度的热红外图像结果。
struct InfraredImageResult
{
    float wavelength_um = 10.5f;
    float radiance = 0.0f;
    float brightnessTemperature = 0.0f;

    float soilSunlitFraction = 0.0f;
    float soilShadedFraction = 0.0f;
    float leafSunlitFraction = 0.0f;
    float leafShadedFraction = 0.0f;
};

struct ImageSimulationResult
{
    OpticalImageResult optical;
    InfraredImageResult infrared;
};

// 独立的图像模拟模块：只读取输入状态，不修改现有能量平衡变量。
class ImageSimulator
{
public:
    ImageSimulationResult simulate(const Canopy& canopy,
                                   const Thermal& thermal,
                                   const Spectral& spectral,
                                   const AtomCond& atomcond,
                                   const Meteo& meteo,
                                   const Angle& angle,
                                   float tirWavelengthUm = 10.5f) const;

private:
    static float clamp01(float value);
};

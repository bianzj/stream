#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

#include "structs.h"
#include "defined.h"
#include "pixelio.h"
#include "leafopt.h"
#include "scifuns.h"
#include "soilopt.h"
#include "radiative_lai.h"


class RT {
public:
    RT() = default;

    // --- 核心光谱与光学计算 ---
    void optical(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);
    // --- 净辐射计算 (植被/森林) ---
    void netrad_shortwave(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);
    void netrad_longwave(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);

    // --- 净辐射计算 (城市) ---
    void netrad_shortwave_urban(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);
    void netrad_longwave_urban(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio);

    // --- 方向性观测与热红外解算 ---
    void satTirt(std::shared_ptr<PixelIO>& m_pPixelio);
    void nadirTir(std::shared_ptr<PixelIO>& m_pPixelio);

    // 不同场景的采样计算
    void sample_hom(std::shared_ptr<PixelIO>& m_pPixelio);
    void sample_canopy(std::shared_ptr<PixelIO>& m_pPixelio);
    void sample_urban(std::shared_ptr<PixelIO>& m_pPixelio);
    void sample_slope(std::shared_ptr<PixelIO>& m_pPixelio);
    void sample_terrain(std::shared_ptr<PixelIO> &m_pPixelio);


    // --- 物理因子计算接口 ---
    void tirt_direct(Canopy canopy, Angle angle, float& fss, float& fsh, float& fcs, float& fch);
    void tirt_scatter(Canopy canopy, Angle angle, Spectral spectral, float& mss, float& msh, float& mcs, float& mch);
    void tirt_direct_canopy(Canopy canopy, Angle angle, float& fss, float& fsh, float& fcs, float& fch);
    void tirt_scatter_terrain(Canopy canopy, Angle angle, float &fss,float &fsh,float &fcs,float &fch);
    void tirt_direct_terrain(float lai, float std, float hspot, float hcr, float rcr, float vza, float sza, float raa, float &fss,float &fsh,float &fcs,float &fch);


    void directional_emissivity_direct(Canopy canopy, Angle angle, Building building, float& fss, float& fsh, float& frs, float& frh, float& fws, float& fwh, int ifP);
    void directional_emissivity_scatter(Canopy canopy, Angle angle, Building building, float& mss, float& msh, float& mrs, float& mrh, float& mws, float& mwh, int ifP);
    void component_emissivity_direct(Canopy canopy, Angle angle, Terrain terrain, std::vector<float>& results, int ifP);
    void component_emissivity_scatter(Canopy canopy, Angle angle, Spectral spectral, std::vector<float>& results, int ifP);

private:
    // --- 内部数学工具函数 ---
    static float Jfunc1(float k, float l, float t);
    static float Jfunc2(float k, float l, float t);
    static float Jfunc3(float k, float l, float t);
    static void volscatt(float sza, float vza, float raa, float ttl, float* chi_s, float* chi_o, float* frho, float* ftau);
    static void cal_LIDF(float ala, float* freq);
    static double calculate_projection(const std::vector<double>& angles, const std::vector<double>& shape, double alpha, double height_diff, double rd);
    static double calculate_overlap(double tantv, double tants, double up);
    static void calculate_component_fraction(float length, float width, float height, float density, float sza, float saa,
                                  float* frs, float* frh, float* fws, float* fwh, float* fss, float* fsh);

    // --- 内部散射物理模型 ---
    void diffuseScatter_VNIR(float lai, float rho, float tau, float rs, float sza, float Esun, float Esky, float* diffuserad_leaf, float* diffuserad_soil, float vza = 0, float raa = 0, float thm = 53);
    void diffuseScatter_TIR(float lai, float rho, float tau, float rs, float sza, float Esun, float Esky, float Tss, float Tsh, float Tvs, float Tvh, float* diffuserad_leaf, float* diffuserad_soil, float vza = 0, float raa = 0, float thm = 53);
    void diffuseScatter_VNIR_Urban(float ref_roof, float ref_wall, float ref_street, float svf_s, float svf_w, float frs, float frh, float fws, float fwh, float fss, float fsh, float BAI, float sza, float Esun, float Esky, float* diffuserad_roof, float* diffuserad_wall, float* diffuserad_street, float vza =0,float raa = 0,float thm=53);
    void diffuseScatter_TIR_urban(float bai, float svf_w, float svf_s, float frs, float frh, float fws, float fwh, float fss, float fsh, float rrho, float wrho, float srho, float sza, float Esun, float Esky, float Trs, float Trh, float Tws, float Twh, float Tss, float Tsh, float* diffuserad_roof, float* diffuserad_wall, float* diffuserad_street, float vza =0,float raa = 0,float thm=53);

    // 辅助计算
    float hotspot_analytical(Canopy canopy, Angle angle);
    float hotspot_analytical_canopy(float lai, float hspot, float vza, float sza, float raa, float CIs, float CIv);
    float hotspot_volume(float lai, float sza);
    float hotspot_layer(Canopy canopy, Angle angle);
    float hotspot_layer_canopy(float lai, float hspot, float vza, float sza, float raa, float CIs, float CIv);
    float gap_probability_hom_analytical(float lai, float vza);
    float gap_probability_hom_hemisphere_analytical(float lai);
    float gap_probability_crown_analytical(float lai, float std, float radi_horizontal, float radi_vertical, float xza);
    float hotspot_vegetation_volume(float lai, float sza);
    void multiple_scattering_analytical_sunlit(float lai, float vza, float sza, float refl_soil, float refl_leaf,
                                               float& ems_out, float& emh_out);
    std::pair<float, float> slope1(float vza, float vaa, float pza, float paa);

    // 成员变量
    LeafOpt m_leafopt;
    SoilOpt m_soilopt;


};

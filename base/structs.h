#pragma once
#include <iostream>
#include <vector>
#include <map>


#define N_THREAD 320
#define N1 2001
#define N2 161
#define PI 3.1415926535
#define RHOA 1.2047      // specific mass of air
#define CP   1004        // specific heat of dry air
#define KAPPA 0.4        // Von Karman constant
#define GEARTH  9.81     // gravity acceleration
#define AHC 119.7117122  //
#define MAIR 28.96       // molecular mass of dry air
#define RGAS 8.31           // Molar gas constant
#define MH20 18          // molecular mass of water
#define MCO2 44          // Molecular mass of carbon dioxide
#define SIGMASB 5.67e-8   // stefan boltzman constant
#define STRESS 1         // 
#define C2K    273.15    // melting point of water
#define KV 0.6396        // 
#define RD 0.01745329   // 3.14/180.0
#define GG 0.5       // leaf projection
#define CI 1.0      // clumping index
#define RAD_THRESHOLD 5    // radiance threshold
#define TMIN_THRESHOLD 220  // -50 C
#define TMAX_THRESHOLD 350  // +80 C
#define N_ITER 50
#define N_NODE 50 // pacth for each solution 48 for one day
#define RSS 50
#define WL 10.5

enum IGBP
{
    unclassified,
    evergreen_needleleaf_forest,
    evergreen_broadleaf_forest,
    deciduous_needleleaf_forest,
    deciduous_broadleaf_forest,
    mixed_forest,
    closed_shrublands,
    open_shrublands,
    woody_savannas,
    savannas,
    grasslands,
    permanent_watlands,
    croplands,
    urban_and_builtup,
    cropland_vegetation_mosaic,
    snow_and_ice,
    barren_sparsely_vegetated,
    water,
    fill_value
};

enum IGBPsubtype
{
    non_cropland,
    cotton,
    spring_maize,
    winter_wheat_and_summer_maize,
    other_one_cropping_systems,
    other_double_cropping_systems,
    winter_wheat_and_rice
};

enum LIDF
{
    planophile,
    erectrophile,
    plagiophile,
    extremophile,
    spherical,
    uniform
};


struct Angle
{
    float vza;
    float vaa;
    float sza;
    float saa;
};


struct FluspectParam
{
    float Cab;
    float Cw;
    float Cdm;
    float Cs;
    float N;
    float refl_tir;
    float trans_tir;
};

struct BSMParam
{
    //  float SMC;
    float BSMBrightness;
    float BSMlat;
    float BSMlon;
    float refl_tir;
    float trans_tir;
};


struct LeafBio
{
    float Vcmax; // maximum carboxylation capacity (at optimum temperature)
    float m; // ball-berry stomatal conductance parameter "m"
    float BallBerry; // "b"
    float Type;
    float kV; // extinction coefficient for a vertical profile
    float Rdparam; // parameter for dark respiration
    float Tparam[5];
    float Tyear;
    float beta;
    float kNPQs; // rate constant of sustained thermal dissipation
    float qLs; // fraction of functional reaction centers
    float stressfactor;
    int Tcor;

    FluspectParam fp;
};

struct SoilSet
{
    int method;
    float rss; // soil resistance for evaporation
    float cs; // volumetric heat capacity of the soil
    float rhos;
    float lambdas;
    float SMC; // volumetric soil moisture content
    float csSoil;
    float rbs;
    float Tsoil;
    float satwater;

    BSMParam bsm;
};

struct Spectral
{
    float wl_[N1];
    float leafRefl_[N1];
    float leafTran_[N1];
    float soilRefl_[N1];
    float leafRefl_ir;
    float soilRefl_ir;
    float leafTran_ir;
};


struct MeteoMeta
{
    float t;
    float Oa;
    float Ca;
    float ea;
    float dTime;
    float z;
    float RIn;
    float Rli;
    float Tsold;
    float SatWater;
    float sm;
    float u;
    float p;
    int startYear;
    int endYear;
    int startDoy;
    int endDoy;
};

struct Thermal
{
    float Tsoilsunlit;
    float Tsoilshaded;
    float Tleafsunlit;
    float Tleafshaded;
    float Tsk;
    float Tlastsunlit[10];
    float Tlastshaded[10];

    float Troofsunlit;
    float Troofshaded;
    float Twallsunlit;
    float Twallshaded;
    float Tstreetsunlit;
    float Tstreetshaded;
};

struct Canopy
{
    float lai;
    float stand;
    float height;
    float width;
    float Gleaf;
    float LIDFa;
    float LIDFb;
    float hspot;
    float leafwidth;
    int type;
    int dist; // 0 barrain, 1 veg, 2 crop, 3 forest, 4 urban
    float treeStand;
    float canopyHeight;
    float b;
};

// Lightweight state for a generic crop prior.  It intentionally uses a
// small number of robust parameters so it can run when cultivar, irrigation,
// and field-level management data are unavailable.
struct CropState
{
    bool initialized = false;
    bool isCrop = false;
    int lastYear = 0;
    int lastDoy = 0;
    int sowingDoy = 90;
    float gdd = 0.0f;
    float stage = 0.0f;
    float lai = 0.0f;
    float biomassGc = 0.0f;
};

struct Urban
{
    float bai;
    float density;
    float height;
    float width;
    float length;
};

typedef struct {
    float length;
    float width;
    float height;
    float alpha;
    float ci=1.0;
} BShape;

struct Building {
    std::vector<std::vector<double>> shapes; // 每个建筑物的形状参数：长度、宽度、高度、密度
    int n_part; // 墙壁分段数量
    //    std::vector<double> Eroof, Ewall, Estreat; // 发射率
    float Eroof, Ewall, Estreat; // 发射率
};


struct Meteo
{
    float t;
    float u;
    float ta;
    float ea;
    float p;
    float rin;
    float rli;
    // Precipitation in mm per forcing time step.  Zero means no product was
    // supplied; it is not an inferred rainfall value.
    float precipitation = 0.0f;
};

struct AeroCoeff
{
    float zo;      // 表面粗糙度长度 (Roughness length)，用于描述地表特性对风速的影响
    float d;       // 零平面位移高度 (Zero-plane displacement height)，通常用于描述植被或建筑物的平均高度
    float Cd;      // 植被的阻力系数 (Drag coefficient for the vegetation)，描述空气流动受植被影响的阻力大小
    float rbc;     // 叶片边界层阻力 (Leaf boundary layer resistance)，描述叶片周围空气边界层的传热或传质阻力
    float CR;      // 独立树木的阻力系数 (Drag coefficient for an isolated tree)，用于描述单棵树木对气流的阻力影响
    float CD1;     // 拟合参数 (Fitting parameter)，通常为经验公式中用来调整阻力模型的参数
    float Psicor;  // 粗糙度层修正项 (Roughness layer correction)，修正空气动力学参数在粗糙层的影响
    float CSSOIL;  // 土壤阻力系数 (Drag coefficient for soil)，描述土壤表面对空气流动的阻力
    float rbs;     // 土壤边界层阻力 (Soil boundary resistance)，描述土壤表面边界层的传热或传质阻力
    float rwc;     // 冠层内空气动力阻力 (Aerodynamic resistance within canopy)，描述冠层内气流受阻力的影响
};

struct Terrain
{
    float dem;
    float aspect;
    float slope;
    float n;
    float h;
    float r;
};

struct InputSet
{
    // topographic feature
    Terrain terrain;
    // vegetation type and structure
    Canopy canopy;
    // vegetation and soil properties
    //FluspectParam fluspectParam;
    // soil moisture

    // vegetation bio properties;
    LeafBio leafbio;
    // soil physical properties
    SoilSet soilset;
    Urban urban;

    std::vector<Meteo> vMeteo;
    MeteoMeta meta;

    //        float t[N_NODE];
    //        float Ta[N_NODE];
    //        float u[N_NODE];
    //        float p[N_NODE];
    //        float Rin[N_NODE];
    //        float Rli[N_NODE];
    //        float Oa[N_NODE];
    //        float Ca[N_NODE];
    //        float Ea[N_NODE];
};

// Rn - H - LE - Gleaf = 0
struct Heatflux
{
    float Nsoilsunlit;
    float Nsoilshaded;
    float Nleafsunlit;
    float Nleafshaded;
    float Hsoilsunlit; // sensible heat flux of sunlit component
    float Hsoilshaded; // sensible heat flux of shaded component
    float LEsoilsunlit; // latent heat flux of sunlit component
    float LEsoilshaded; // latent heat flux of shaded component
    float Hleafsunlit; // sensible heat flux of sunlit component
    float Hleafshaded; // sensible heat flux of shaded component
    float LEleafsunlit; // latent heat flux of sunlit component
    float LEleafshaded; // latent heat flux of shaded component
    float Gsunlit; // change in heat storage of sunlit component
    float Gshaded; // change in heat storage of shaded component

    float Nroofsunlit;
    float Nroofshaded;
    float Nwallsunlit;
    float Nwallshaded;
    float Nstreetsunlit;
    float Nstreetshaded;

    float Hroofsunlit;
    float Hroofshaded;
    float Hwallsunlit;
    float Hwallshaded;
    float Hstreetsunlit;
    float Hstreetshaded;
    float LEroofsunlit;
    float LEroofshaded;
    float LEwallsunlit;
    float LEwallshaded;
    float LEstreetsunlit;
    float LEstreetshaded;
    float Groofsunlit;
    float Groofshaded;
    float Gwallsunlit;
    float Gwallshaded;
    float Gstreetsunlit;
    float Gstreetshaded;
};


struct OptCoeff
{
    //    float * nr_, *kdm_, *kab_, *kw_, *ks_, *phiI_, *phiII_;
    std::vector<float> wl_;
    std::vector<float> nr_;
    std::vector<float> kab_;
    std::vector<float> kca_;
    std::vector<float> kdm_;
    std::vector<float> ks_;
    std::vector<float> kw_;
    std::vector<float> phiI_;
    std::vector<float> phiII_;
    std::vector<float> kcaV_;
    std::vector<float> kcaV2_;
    std::vector<float> kcaZ_;
    std::vector<float> kcant_;
    std::vector<float> gsv1_;
    std::vector<float> gsv2_;
    std::vector<float> gsv3_;
    std::vector<float> nw_;
    std::vector<float> phi_;
};

struct NetRad
{
    float directVrad_leaf; // 在可见近红外（VNIR）波段的直接辐射（直射光）在叶片上的辐射量
    float diffuseVrad_leaf; // 在可见近红外（VNIR）波段的散射辐射（漫射光）在叶片上的辐射量。
    float directTrad_leaf; // 在热红外（TIR）波段的直接辐射在叶片上的辐射量
    float diffuseTrad_leaf; // 在热红外（TIR）波段的散射辐射在叶片上的辐射量。
    float directVrad_soil; // 在可见近红外（VNIR）波段的直接辐射在土壤上的辐射量。
    float diffuseVrad_soil; // 在可见近红外（VNIR）波段的散射辐射在土壤上的辐射量。
    float directTrad_soil; // 在热红外（TIR）波段的直接辐射在土壤上的辐射量。
    float diffuseTrad_soil; // 在热红外（TIR）波段的散射辐射在土壤上的辐射量。
    float directPrad_leaf;
    float diffusePrad_leaf;

    float directVrad_roof;
    float diffuseVrad_roof;
    float directTrad_roof;
    float diffuseTrad_roof;
    float directVrad_wall;
    float diffuseVrad_wall;
    float directTrad_wall;
    float diffuseTrad_wall;
    float directVrad_street;
    float diffuseVrad_street;
    float directTrad_street;
    float diffuseTrad_street;
    float directPrad_roof;
    float diffusePrad_roof;
    float directPrad_wall;
    float diffusePrad_wall;
    float directPrad_street;
    float diffusePrad_street;
};

struct AtomCond
{
    float wl[N1 + N2];
    float fesky[N1 + N2];
    float fesun[N1 + N2];
};


// Air stand
struct BioState
{
    float cs; // Carbon surface
    float ci; // Carbon inside
    float es; // Water Surface
    // Leaf-scale carbon fluxes, in umol CO2 m-2 leaf s-1.
    float grossAssimilationSunlit = 0.0f;
    float grossAssimilationShaded = 0.0f;
    float netAssimilationSunlit = 0.0f;
    float netAssimilationShaded = 0.0f;
    float respirationSunlit = 0.0f;
    float respirationShaded = 0.0f;
    float rssunlit;
    float rsshaded;
    float rss;
    float rssroof;
    float rsswall;
    float rssstreet;
};

// Minimal prognostic water/carbon pools.  These are only updated when the
// balance feature is explicitly enabled.
struct BalanceState
{
    bool initialized = false;
    float soilWaterMm = 0.0f;
    float canopyWaterMm = 0.0f;
    float soilWaterForecastMm = 0.0f;
    float pendingSoilMoisture = -1.0f;
    float leafCarbonGc = 0.0f;

    float etMm = 0.0f;
    float precipitationMm = 0.0f;
    float interceptionMm = 0.0f;
    float throughfallMm = 0.0f;
    float infiltrationMm = 0.0f;
    float runoffMm = 0.0f;
    float drainageMm = 0.0f;
    float assimilationWaterIncrementMm = 0.0f;
    float grossAssimilationGc = 0.0f;
    float plantRespirationGc = 0.0f;
    float netAssimilationGc = 0.0f;

    float dailyEtMm = 0.0f;
    float dailyPrecipitationMm = 0.0f;
    float dailyInterceptionMm = 0.0f;
    float dailyThroughfallMm = 0.0f;
    float dailyInfiltrationMm = 0.0f;
    float dailyRunoffMm = 0.0f;
    float dailyDrainageMm = 0.0f;
    float dailyAssimilationWaterIncrementMm = 0.0f;
    float dailyGrossAssimilationGc = 0.0f;
    float dailyPlantRespirationGc = 0.0f;
    float dailyNetAssimilationGc = 0.0f;
};

struct Resistance
{
    float L;
    float ustar;
    float raa_leaf;
    float raa_soil;
    float raa_roof;
    float raa_wall;
    float raa_street;
};

struct Satellite
{
    float vza;
    float sza;
    float vaa;
    float saa;
    std::string time;
    std::string proj;
    double trans[6];
};




// change at a 10-min step
struct DynamicVariable
{
    BioState biostate;
    CropState crop;
    BalanceState balance;
    Thermal thermal;
    Heatflux heatflux;
    NetRad netrad;
    Resistance resistance;
    Satellite satellite;

    // shortwave radiation
    // longwave radiation
    // temperature profile
    // soil historic temperature
    // sensible heat flux
    // latent heaf flux
    // soil surface storage
    // aerodynamic resistance
    // surface resistance
};

// change at a 1-day + step
struct StaticVariable
{
    // vegetation refl and trans
    // soil reflectance
    Spectral spectal;
};


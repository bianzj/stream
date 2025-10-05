#include "defined.h"




void Defined::destroy()
{

//    delete [] m_optCoeff.nr_;
//    delete [] m_optCoeff.kdm_;
//    delete [] m_optCoeff.kab_;
//    delete [] m_optCoeff.kw_;
//    delete [] m_optCoeff.ks_;
//    delete [] m_optCoeff.phiI_;
//    delete [] m_optCoeff.phiII_;
//    delete [] m_soilRefl_;
}


// void Defined::defineCanopy()
// {
//     // 20250107备份
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::water,
//                                             Canopy{-1, 0, 0, 0, 0, 0, 0, 0, 0, 17,0, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::evergreen_needleleaf_forest,
//                                             Canopy{3.0, 1, 15, 5, 0.5, -0.35, -0.15, 0.5, 0.01, 1,3, 0, 0, 5/1.0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::evergreen_broadleaf_forest,
//                                             Canopy{3.0, 1, 15, 5, 0.5, -0.35, -0.15, 0.5, 0.10, 2,3, 0, 0, 3/1.0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::deciduous_needleleaf_forest,
//                                             Canopy{3.0, 1, 15, 5, 0.5, -0.35, -0.15, 0.5, 0.01, 3,3, 0, 0, 5/1.0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::deciduous_broadleaf_forest,
//                                             Canopy{3.0, 1, 15, 5, 0.5, -0.35, -0.15, 0.5, 0.10, 4,3, 0, 0, 3/1.0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::mixed_forest,
//                                             Canopy{3.0, 1, 13, 5, 0.5, -0.35, -0.15, 0.5, 0.05, 5,3, 0, 0, 4/1.0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::closed_shrublands,
//                                             Canopy{3.0, 1, 2.0, 3, 0.5, -0.35, -0.15, 0.5, 0.03, 6,1, 0, 0, 1/1.5}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::open_shrublands,
//                                             Canopy{3.0, 1, 1.0, 3, 0.5, -0.35, -0.15, 0.5, 0.03, 7,1, 0, 0, 1/1.5}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::woody_savannas,
//                                             Canopy{3.0, 1, 2.0, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 8,1, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::savannas,
//                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 9,1, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::grasslands,
//                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 10,1, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::permanent_watlands,
//                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 11,1, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::croplands,
//                                             Canopy{3.0, 1, 1.0, 3, 0.5, -0.35, -0.15, 0.2, 0.1, 12,2, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::urban_and_builtup,
//                                             Canopy{3.0, 1, 1, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 13,4, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::cropland_vegetation_mosaic,
//                                             Canopy{3.0, 1, 1.0, 3, 0.5, -0.35, -0.15, 0.2, 0.1, 14,2, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::snow_and_ice,
//                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 15,0, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::barren_sparsely_vegetated,
//                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 16,1, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::fill_value,
//                                             Canopy{3.0, 1, 3, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 255,0, 0, 0, 0}));
//     m_mCanopy.insert(std::pair<int, Canopy>(IGBP::unclassified,
//                                             Canopy{3.0, 1, 3, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 0,0, 0, 0, 0}));
// }

void Defined::defineCanopy()
{
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::water,
                                            Canopy{-1, 0, 0, 0, 0, 0, 0, 0, 0, 17,0, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::evergreen_needleleaf_forest,
                                            Canopy{3.0, 1, 18, 5, 0.5, -0.35, -0.15, 0.5, 0.01, 1,3, 0, 0, 5/1.0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::evergreen_broadleaf_forest,
                                            Canopy{3.0, 1, 20, 5, 0.5, -0.35, -0.15, 0.5, 0.10, 2,3, 0, 0, 3/1.0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::deciduous_needleleaf_forest,
                                            Canopy{3.0, 1, 16, 5, 0.5, -0.35, -0.15, 0.5, 0.01, 3,3, 0, 0, 5/1.0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::deciduous_broadleaf_forest,
                                            Canopy{3.0, 1, 15, 5, 0.5, -0.35, -0.15, 0.5, 0.10, 4,3, 0, 0, 3/1.0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::mixed_forest,
                                            Canopy{3.0, 1, 17, 5, 0.5, -0.35, -0.15, 0.5, 0.05, 5,3, 0, 0, 4/1.0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::closed_shrublands,
                                            Canopy{3.0, 1, 2.0, 3, 0.5, -0.35, -0.15, 0.5, 0.03, 6,1, 0, 0, 1/1.5}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::open_shrublands,
                                            Canopy{3.0, 1, 1.5, 3, 0.5, -0.35, -0.15, 0.5, 0.03, 7,1, 0, 0, 1/1.5}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::woody_savannas,
                                            Canopy{3.0, 1, 2.0, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 8,1, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::savannas,
                                            Canopy{3.0, 1, 1.5, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 9,1, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::grasslands,
                                            Canopy{3.0, 1, 0.6, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 10,1, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::permanent_watlands,
                                            Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.02, 11,1, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::croplands,
                                            Canopy{3.0, 1, 1.0, 3, 0.5, -0.35, -0.15, 0.2, 0.1, 12,2, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::urban_and_builtup,
                                            Canopy{3.0, 1, 1, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 13,4, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::cropland_vegetation_mosaic,
                                            Canopy{3.0, 1, 3.0, 3, 0.5, -0.35, -0.15, 0.2, 0.1, 14,2, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::snow_and_ice,
                                            Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 15,0, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::barren_sparsely_vegetated,
                                            Canopy{3.0, 1, 0.05, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 16,1, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::fill_value,
                                            Canopy{3.0, 1, 3, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 255,0, 0, 0, 0}));
    m_mCanopy.insert(std::pair<int, Canopy>(IGBP::unclassified,
                                            Canopy{3.0, 1, 3, 3, 0.5, -0.35, -0.15, 0.2, 0.2, 0,0, 0, 0, 0}));
}

void Defined::defineLeafbio() {
    FluspectParam fp{58.0, 0.015, 0.0036,    0.0,  1.86,0.025,0 };
    FluspectParam fp1{45.0, 0.020, 0.05,    0.0,  2.0,0.025,0 };
    FluspectParam fp2{50.0, 0.025, 0.05,    0.0,  2.5,0.025,0 };
    FluspectParam fp3{35.0, 0.015, 0.005,    0.0,  1.8,0.025,0 };
    FluspectParam fp4{50.0, 0.020, 0.005,    0.0,  2.2,0.025,0 };
    FluspectParam fp5{50.0, 0.020, 0.025,    0.0,  2.0,0.025,0 };
    FluspectParam fp6{35.0, 0.018, 0.005,    0.0,  2.0,0.025,0 };
    FluspectParam fp7{35.0, 0.015, 0.005,    0.0,  1.7,0.025,0 };
    FluspectParam fp8{35.0, 0.012, 0.005,    0.0,  1.6,0.025,0 };
    FluspectParam fp9{30.0, 0.010, 0.005,    0.0,  1.5,0.025,0 };
    FluspectParam fp10{20.0, 0.010, 0.005,    0.0,  1.2,0.025,0 };
    FluspectParam fp11{30.0, 0.015, 0.005,    0.0,  1.5,0.025,0 };
    FluspectParam fp12{35.0, 0.015, 0.005,    0.0,  1.5,0.025,0 };
    FluspectParam fp14{40.0, 0.015, 0.015,    0.0,  1.5,0.025,0 };
    FluspectParam fp16{35.0, 0.002, 0.001,    0.0,  0.5,0.025,0 };

    // // 20250107备份
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::water,
    //                                           LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_needleleaf_forest,
    //                                           LeafBio{ 70, 9, 0.01, 3, 0.6, 0.015, 0.2,0.3,278,308,328,15,0.507,0,1,1,0,fp1}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_broadleaf_forest,
    //                                           LeafBio{ 90, 12, 0.01, 3, 0.7, 0.02, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp2}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_needleleaf_forest,
    //                                           LeafBio{ 70, 9, 0.01, 3, 0.6, 0.013, 0.2,0.3,278,303,328,15,0.507,0,1,1,0,fp3}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_broadleaf_forest,
    //                                           LeafBio{ 90, 12, 0.01, 3, 0.7, 0.017, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp4}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::mixed_forest,
    //                                           LeafBio{ 80, 12, 0.01, 3, 0.6, 0.016, 0.2,0.3,281,307,328,15,0.507,0,1,1,0,fp5}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::closed_shrublands,
    //                                           LeafBio{ 45, 9, 0.01, 3, 0.7, 0.012, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp6}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::open_shrublands,
    //                                           LeafBio{ 45, 9, 0.01, 3, 0.7, 0.011, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp7}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::woody_savannas,
    //                                           LeafBio{ 65, 9, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp8}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::savannas,
    //                                           LeafBio{ 55, 9, 0.01, 3, 0.65, 0.013, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp9}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::grasslands,
    //                                           LeafBio{ 20, 8, 0.01, 3, 0.4, 0.012, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp10}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::permanent_watlands,
    //                                           LeafBio{ 30, 8, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp11}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::croplands,
    //                                           LeafBio{ 40, 9, 0.01, 4, 0.6, 0.016, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp12}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::urban_and_builtup,
    //                                           LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::cropland_vegetation_mosaic,
    //                                           LeafBio{ 50, 9, 0.01, 4, 0.5, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp14}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::snow_and_ice,
    //                                           LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::barren_sparsely_vegetated,
    //                                           LeafBio{ 20, 9, 0.01, 0, 0.8, 0.012, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp16}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::unclassified,
    //                                           LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::fill_value,
    //                                           LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));

    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::water,
    //                                           LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_needleleaf_forest,
    //                                           LeafBio{ 70, 9, 0.01, 3, 0.6, 0.015, 0.2,0.3,278,308,328,15,0.507,0,1,1,0,fp1}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_broadleaf_forest,
    //                                           LeafBio{ 90, 12, 0.01, 3, 0.7, 0.02, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp2}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_needleleaf_forest,
    //                                           LeafBio{ 70, 9, 0.01, 3, 0.6, 0.013, 0.2,0.3,278,303,328,15,0.507,0,1,1,0,fp3}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_broadleaf_forest,
    //                                           LeafBio{ 90, 12, 0.01, 3, 0.7, 0.017, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp4}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::mixed_forest,
    //                                           LeafBio{ 80, 12, 0.01, 3, 0.6, 0.016, 0.2,0.3,281,307,328,15,0.507,0,1,1,0,fp5}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::closed_shrublands,
    //                                           LeafBio{ 60, 9, 0.01, 3, 0.7, 0.012, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp6}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::open_shrublands,
    //                                           LeafBio{ 45, 9, 0.01, 3, 0.7, 0.011, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp7}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::woody_savannas,
    //                                           LeafBio{ 65, 9, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp8}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::savannas,
    //                                           LeafBio{ 55, 9, 0.01, 3, 0.65, 0.013, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp9}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::grasslands,
    //                                           LeafBio{ 35, 4, 0.01, 3, 0.4, 0.012, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp10}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::permanent_watlands,
    //                                           LeafBio{ 30, 8, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp11}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::croplands,
    //                                           LeafBio{ 45, 9, 0.01, 4, 0.6, 0.016, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp12}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::urban_and_builtup,
    //                                           LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::cropland_vegetation_mosaic,
    //                                           LeafBio{ 50, 9, 0.01, 4, 0.5, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp14}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::snow_and_ice,
    //                                           LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::barren_sparsely_vegetated,
    //                                           LeafBio{ 20, 9, 0.01, 0, 0.8, 0.012, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp16}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::unclassified,
    //                                           LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    // m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::fill_value,
    //                                           LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));

    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::water,
                                              LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_needleleaf_forest,
                                              LeafBio{ 40, 5, 0.01, 3, 0.6, 0.015, 0.2,0.3,278,308,328,15,0.507,0,1,1,0,fp1}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::evergreen_broadleaf_forest,
                                              LeafBio{ 10, 2, 0.01, 3, 0.7, 0.02, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp2}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_needleleaf_forest,
                                              LeafBio{ 70, 9, 0.01, 3, 0.6, 0.013, 0.2,0.3,278,303,328,15,0.507,0,1,1,0,fp3}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::deciduous_broadleaf_forest,
                                              LeafBio{ 40, 6, 0.01, 3, 0.7, 0.017, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp4}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::mixed_forest,
                                              LeafBio{ 40, 6, 0.01, 3, 0.6, 0.016, 0.2,0.3,281,307,328,15,0.507,0,1,1,0,fp5}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::closed_shrublands,
                                              LeafBio{ 10, 8, 0.01, 3, 0.7, 0.012, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp6}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::open_shrublands,
                                              LeafBio{ 50, 2, 0.01, 3, 0.7, 0.011, 0.2,0.3,278,313,328,15,0.507,0,1,1,0,fp7}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::woody_savannas,
                                              LeafBio{ 50, 6, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp8}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::savannas,
                                              LeafBio{ 50, 6, 0.01, 3, 0.65, 0.013, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp9}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::grasslands,
                                              LeafBio{ 50, 10, 0.01, 3, 0.4, 0.012, 0.2,0.3,288,313,328,15,0.507,0,1,1,0,fp10}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::permanent_watlands,
                                              LeafBio{ 20, 8, 0.01, 3, 0.5, 0.014, 0.2,0.3,288,303,328,15,0.507,0,1,1,0,fp11}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::croplands,
                                              LeafBio{ 50, 6, 0.01, 4, 0.6, 0.016, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp12}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::urban_and_builtup,
                                              LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,283,311,328,15,0.507,0,1,1,0,fp}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::cropland_vegetation_mosaic,
                                              LeafBio{ 50, 6, 0.01, 4, 0.5, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp14}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::snow_and_ice,
                                              LeafBio{ 0, 0, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::barren_sparsely_vegetated,
                                              LeafBio{ 20, 9, 0.01, 0, 0.8, 0.012, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp16}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::unclassified,
                                              LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
    m_mLeafbio.insert(std::pair<int, LeafBio>(IGBP::fill_value,
                                              LeafBio{ 80, 9, 0.01, 0, 0.6396, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp}));
}

void Defined::defineCanopy_plus()
{
    // // 20250107备份
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::non_cropland,
    //                                             Canopy{3.0, 1, 1.0, 5, 0.5, -0.35, -0.15, 0.2, 0.02, 12,3}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::cotton,
    //                                             Canopy{3.0, 1.0, 1.5, 5, 0.5, -0.35, -0.15, 0.2, 0.025, 12,3}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::spring_maize,
    //                                             Canopy{3.0, 1, 1.5, 5, 0.5, -0.35, -0.15, 0.2, 0.025, 12,3}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::winter_wheat_and_summer_maize,
    //                                             Canopy{3.0, 1, 3.0, 5, 0.5, -0.35, -0.15, 0.1, 0.015, 12,3}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::other_one_cropping_systems,
    //                                             Canopy{3.0, 1, 1.5, 5, 0.5, -0.35, -0.15, 0.2, 0.015, 12,3}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::other_double_cropping_systems,
    //                                             Canopy{3.0, 1, 1.2, 3, 0.5, -0.35, -0.15, 0.2, 0.015, 12,1}));
    // m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::winter_wheat_and_rice,
    //                                             Canopy{3.0, 1, 0.5, 3, 0.5, -0.35, -0.15, 0.2, 0.015, 12,1}));

    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::non_cropland,
                                                    Canopy{3.0, 1, 1.0, 5, 0.5, -0.35, -0.15, 0.2, 0.02, 12,3}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::cotton,
                                                Canopy{3.0, 1, 1.5, 5, 0.5, -0.35, -0.15, 0.2, 0.025, 12,3}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::spring_maize,
                                                Canopy{3.0, 1, 2.5, 5, 0.5, -0.35, -0.15, 0.2, 0.025, 12,3}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::winter_wheat_and_summer_maize,
                                                Canopy{3.0, 1, 2.5, 5, 0.5, -0.35, -0.15, 0.1, 0.015, 12,3}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::other_one_cropping_systems,
                                                Canopy{3.0, 1, 2.0, 5, 0.5, -0.35, -0.15, 0.2, 0.015, 12,3}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::other_double_cropping_systems,
                                                Canopy{3.0, 1, 2.0, 3, 0.5, -0.35, -0.15, 0.2, 0.015, 12,1}));
    m_mCanopy_sub.insert(std::pair<int, Canopy>(IGBPsubtype::winter_wheat_and_rice,
                                                Canopy{3.0, 1, 1.0, 3, 0.5, -0.35, -0.15, 0.2, 0.015, 12,1}));

}

void Defined::defineLeafbio_plus() {
    FluspectParam fp{25.0, 0.015, 0.005,    0.0,  1.2,0.025,0 };
    FluspectParam fp1{20.0, 0.010, 0.005,    0.004,  1.8,0.025,0 };
    FluspectParam fp2{10.0, 0.010, 0.005,    0.003,  1.5,0.025,0 };
    FluspectParam fp3{40.0, 0.011, 0.010,    0.003,  1.6,0.025,0 };
    FluspectParam fp4{25.0, 0.012, 0.005,    0.004,  1.7,0.025,0 };
    FluspectParam fp5{35.0, 0.012, 0.005,    0.003,  1.6,0.025,0 };
    FluspectParam fp6{40.0, 0.011, 0.012,    0.003,  1.8,0.025,0 };

    // // 20250107备份
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::non_cropland,
    //                                               LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::cotton,
    //                                               LeafBio{ 35, 9, 0.01, 3, 0.65, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp1}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::spring_maize,
    //                                               LeafBio{ 10, 9, 0.01, 4, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp2}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_summer_maize,
    //                                               LeafBio{ 30, 9, 0.01, 4, 0.50, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp3}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_one_cropping_systems,
    //                                               LeafBio{ 30, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp4}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_double_cropping_systems,
    //                                               LeafBio{ 35, 9, 0.01, 3, 0.68, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp5}));
    // m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_rice,
    //                                               LeafBio{ 40, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp6}));

    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::non_cropland,
                                                  LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::cotton,
                                                  LeafBio{ 60, 9, 0.01, 3, 0.65, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp1}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::spring_maize,
                                                  LeafBio{ 55, 9, 0.01, 4, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp2}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_summer_maize,
                                                  LeafBio{ 58, 9, 0.01, 4, 0.50, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp3}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_one_cropping_systems,
                                                  LeafBio{ 59, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp4}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_double_cropping_systems,
                                                  LeafBio{ 57, 9, 0.01, 3, 0.68, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp5}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_rice,
                                                  LeafBio{ 59, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp6}));

    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::non_cropland,
                                                  LeafBio{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,fp}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::cotton,
                                                  LeafBio{ 60, 9, 0.01, 3, 0.65, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp1}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::spring_maize,
                                                  LeafBio{ 55, 9, 0.01, 4, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp2}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_summer_maize,
                                                  LeafBio{ 58, 9, 0.01, 4, 0.50, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp3}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_one_cropping_systems,
                                                  LeafBio{ 59, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp4}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::other_double_cropping_systems,
                                                  LeafBio{ 57, 9, 0.01, 3, 0.68, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp5}));
    m_mLeafbio_sub.insert(std::pair<int, LeafBio>(IGBPsubtype::winter_wheat_and_rice,
                                                  LeafBio{ 59, 9, 0.01, 3, 0.40, 0.015, 0.2,0.3,281,308,328,15,0.507,0,1,1,0,fp6}));

}

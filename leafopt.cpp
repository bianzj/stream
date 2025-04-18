/*
in this case, the leaf reflectance and transpiration will be modeled;

*/


#include "defined.h"
#include "structs.h"
#include <math.h>
#include "leafopt.h"







void LeafOpt::fluspect(OptCoeff fluspectCoeff,FluspectParam fluspectParam,Spectral& spectral)
{
    // 提取叶片生物物理参数
    float Cab = fluspectParam.Cab;  // 叶绿素含量
    float Cw = fluspectParam.Cw;    // 叶片含水量
    float Cdm = fluspectParam.Cdm;  // 干物质含量
    float Cs = fluspectParam.Cs;    // 其他含量（例如叶片中的其他化合物）
    float N = fluspectParam.N;      // 层数或密度参数

    // 引用光谱系数的各个向量
    std::vector<float> &nr_         = fluspectCoeff.nr_;
    std::vector<float> &kdm_        = fluspectCoeff.kdm_;
    std::vector<float> &kab_         = fluspectCoeff.kab_;
    std::vector<float> &kw_         = fluspectCoeff.kw_;
    std::vector<float> &ks_         = fluspectCoeff.ks_;
    std::vector<float> &phiI_       = fluspectCoeff.phiI_;
    std::vector<float> &phiII_      = fluspectCoeff.phiII_;

    float nr, Kdm, Kab, Kw, Ks, phiII, phiI, Kall, t1, t2, tau, kChlrel,
        t12, r12, t21, r21, denom, Ra, r, D, rq, tq, a, b, bNm1, bN2_g, a2, Rsub, Tsub,
        s, j, talf, ralf, tt, Ta;


    for(int i=0; i<N1; i++)
    {
        nr          = nr_[i];
        //
        Kdm         = kdm_[i];
        Kab         = kab_[i];
        Kw          = kw_[i];
        Ks          = ks_[i];
        phiI        = phiI_[i];
        phiII       = phiII_[i];

        // PROSPECT模型的计算
        Kall        = (Cab*Kab + Cdm*Kdm + Cw*Kw  + Cs*Ks)/N;

        // Non-conservative scattering (normal case)
        t1          = (1-Kall)*exp(-Kall);
        t2          = Kall*Kall*Utils::expint(Kall);
        tau         = 1;
        if(Kall > 0) tau      = t1+t2;
        kChlrel     = 0;
        if(Kall > 0) kChlrel  = Cab*Kab/(Kall*N);

        // 计算顶层的透过率和反射率
        talf        = SCI::calctav(59,nr);
        ralf        = 1-talf;
        t12         = SCI::calctav(90,nr);
        r12         = 1-t12;
        t21         = t12/(nr*nr);
        r21         = 1-t21;

        // 计算顶层的反射率和透过率
        denom       = 1-r21*r21*tau*tau;
        Ta          = talf*tau*t21/denom;
        Ra          = ralf+r21*tau*Ta;

        // 计算更深层的反射率和透过率
        tt           = t12*tau*t21/denom;
        r           = r12+r21*tau*tt;

        // 通过Stokes方程计算接下来N-1层的特性（实际层数）
        D           = sqrt((1+r+tt)*(1+r-tt)*(1-r+tt)*(1-r-tt));
        rq          = r*r;
        tq          = tt*tt;
        a           = (1+rq-tq+D)/(2*r);
        b           = (1-rq+tq+D)/(2*tt);

        bNm1        = pow(b,(N-1));
        bN2_g         = bNm1*bNm1;
        a2          = a*a;
        denom       = a2*bN2_g-1;
        Rsub        = a*(bN2_g-1)/denom;
        Tsub        = bNm1*(a2-1)/denom;
        s           = r/tt;                             // Conservative scattering (CS)
        if(Kall>0) s        = 2*a/(a*a-1)*log(b);   // Normal case overwrites CS case

        // 零吸收情况下的处理
        if(r+tt>=1)
        {
            Tsub     = tt/(tt+(1-tt)*(N-1));
            Rsub	    = 1-Tsub;
        }

        /// 结合顶层和N-1层计算叶片的反射率和透射率
        denom       = 1-Rsub*r;
        spectral.leafTran_[i]        = Ta*Tsub/denom;
        spectral.leafRefl_[i]       = Ra+Ta*Rsub*tt/denom;

//m_leafopt.refl = refl;
//m_leafopt.tran = tran;
//m_leafopt.kChlrel = kChlrel;

//t        =   tran;
//r        =   refl;
//
//I_rt     =   (r+t)<1;
//D(I_rt)  =   sqrt((1 + r(I_rt) + t(I_rt)) * ...
//                  (1 + r(I_rt) - t(I_rt)) * ...
//                  (1 - r(I_rt) + t(I_rt)) * ...
//                  (1 - r(I_rt) - t(I_rt)));
//a(I_rt)  =   (1 + r(I_rt).^2 - t(I_rt).^2 + D(I_rt)) / (2*r(I_rt));
//b(I_rt)  =   (1 - r(I_rt).^2 + t(I_rt).^2 + D(I_rt)) / (2*t(I_rt));
//a(~I_rt) =   1;
//b(~I_rt) =   1;
//
//I_a      =   a>1;
//s(I_a)   =   2*a(I_a) / (a(I_a).^2 - 1) * log(b(I_a));
//s(~I_a)  =   r(~I_a) / t(~I_a);
//
//k        =   (a-1) / (a+1) * log(b);
//kChl     =   kChlrel * k;
//k(j)        = 0;
//kChl(j)     = 0;
//
//if fqe > 0
//
//    %% Fluorescence
//    wle         = m_spectral.wlE';    % excitation wavelengths, transpose to column
//    wlf         = m_spectral.wlF';    % fluorescence wavelengths, transpose to column
//    wlp         = m_spectral.wlP;     % PROSPECT wavelengths, also a row vector
//
//    minwle      = min(wle);
//    maxwle      = max(wle);
//    minwlf      = min(wlf);
//    maxwlf      = max(wlf);
//
//    % indices of wle and wlf within wlp
//
//    Iwle        = find(wlp>=minwle & wlp<=maxwle);
//    Iwlf        = find(wlp>=minwlf & wlp<=maxwlf);
//
//    eps         = 2^(-ndub);
//
//    % initialisations
//    te          = 1-(k(Iwle)+s(Iwle)) * eps;
//    tf          = 1-(k(Iwlf)+s(Iwlf)) * eps;
//    re          = s(Iwle) * eps;
//    rf          = s(Iwlf) * eps;
//
//    sigmoid     = 1/(1+exp(-wlf/10)*exp(wle'/10));  % matrix computed as an outproduct
//
//    % Other factor .5 deleted, since these are the complete efficiencies
//    % for either PSI or PSII. not a linear combination
//    [MfI,  MbI]  = deal(fqe(1) * ((.5*phiI( Iwlf))*eps) * kChl(Iwle)'*sigmoid);
//    [MfII, MbII] = deal(fqe(2) * ((.5*phiII(Iwlf))*eps) * kChl(Iwle)'*sigmoid);
//
//    Ih          = ones(1,length(te));     % row of ones
//    Iv          = ones(length(tf),1);     % column of ones
//
//    % Doubling routine
//
//    for i = 1:ndub
//
//        xe = te/(1-re*re);  ten = te*xe;  ren = re*(1+ten);
//        xf = tf/(1-rf*rf);  tfn = tf*xf;  rfn = rf*(1+tfn);
//
//        A11  = xf*Ih + Iv*xe';           A12 = (xf*xe')*(rf*Ih + Iv*re');
//        A21  = 1+(xf*xe')*(1+rf*re');   A22 = (xf*rf)*Ih+Iv*(xe*re)';
//
//        MfnI   = MfI  * A11 + MbI  * A12;
//        MbnI   = MbI  * A21 + MfI  * A22;
//        MfnII  = MfII * A11 + MbII * A12;
//        MbnII  = MbII * A21 + MfII * A22;
//
//        te   = ten;  re  = ren;   tf   = tfn;   rf   = rfn;
//        MfI  = MfnI; MbI = MbnI;  MfII = MfnII; MbII = MbnII;
//
//    end
//}
//    m_leafopt.MbI  = MbI;
//    m_leafopt.MbII = MbII;
//    m_leafopt.MfI  = MfI;
    }
}



#include "rt.h"
#include <numeric> // 添加这个头文件以使用 std::accumulate




float Jfunc1(float k, float l, float t)
{
    float del = (k - l) * t;  // 计算k和l的差值与时间t的乘积，del代表了这个差值的影响
    float Jout = del;         // 初始化Jout为del

    // 如果del大于1e-3，使用更精确的指数衰减公式计算Jout
    if (del > 1e-3)
    {
        // 计算k和l在时间t内的指数衰减差值的平均，返回该值
        Jout = ((exp(-l * t)) - exp(-k * t)) / (k - l);
    }

    // 如果del小于等于1e-3，使用近似的泰勒级数展开来计算Jout
    if (del <= 1e-3)
    {
        // 使用泰勒展开来近似计算，减小误差
        Jout = 0.5 * t * (exp(-k * t) + exp(-l * t)) * (1 - del * del / 12.0);
    }
    return Jout;  // 返回计算结果
}

float Jfunc2(float k, float l, float t)
{
    // 计算k和l之和与时间t的乘积的指数衰减，返回该值
    return (1.0 - exp(-(k + l) * t)) / (k + l);
}

float Jfunc3(float k,float l,float t)
{
    return (1.0-exp(-(k+l)*t))/(k+l);
}


//void volscatt(float sza, float vza,float raa, float ttl, float *chi_s,float *chi_o,float *frho,float *ftau)
//{
//
//    float rd = 3.1415926/180.0;
//    float costs = cos(rd * sza);
//    float costo = cos(rd * vza);
//    float sints = sin(rd * sza);
//    float sinto = sin(rd * vza);
//    float cosraa = cos(rd * raa);
//    float raar = rd * raa;
//    float costl = cos(rd * ttl);
//    float sintl = sin(rd * ttl);
//    float cs = costl * costs;
//    float co = costl * costo;
//    float ss = sintl * sints;
//    float so = sintl * sinto;
//
//    float cosbts = 5.0;
//    float cosbto = 5.0;
//    float bts =0,ds = 0, bto,doo;
//
//    if (abs(ss) > 1e-6)
//        cosbts = -cs /ss;
//    if (abs(so) > 1e-6)
//        cosbto = -co /so;
//    if (abs(cosbts) < 1.0)
//    {
//        bts = acos(cosbts);
//        ds = ss;
//    }
//    else
//    {
//        bts =3.1415926;
//        ds = cs;
//        (*chi_s) = 2.0 /PI * ((bts -PI * 0.5) * cs + sin(bts) * ss);
//    }
//
//    if (abs(cosbto) < 1.0)
//    {
//        bto = acos(cosbto);
//        doo = so;
//    }
//    else
//    {
//        if (vza < 90.0)
//        {
//            bto =PI;
//            doo = co;
//        }
//        else
//        {
//            bto = 0.0;
//            doo = -co;
//        }
//    }
//    (*chi_o) = 2.0 /PI * ((bto -PI * 0.5) * co + sin(bto) * so);
//    float btran1 = abs(bts - bto);
//    float btran2 = PI - abs(bts + bto - PI);
//
//    float bt1,bt2,bt3;
//    if (raar <= btran1)
//    {
//        bt1 = raar;
//        bt2 = btran1;
//        bt3 = btran2;
//    }
//    else
//    {
//        bt1 = btran1;
//        if (raar <= btran2)
//        {
//            bt2 = raar;
//            bt3 = btran2;
//        }
//        else
//        {
//            bt2 = btran2;
//            bt3 = raar;
//        }
//    }
//    float t1,t2;
//    t1 = 2.0 * cs * co + ss * so * cosraa;
//    t2 = 0.0;
//    if (bt2 > 0.0)
//        t2= sin(bt2) * (2.0 * ds * doo+ss * so * cos(bt1) * cos(bt3));
//
//    float denom = 2.0 *PI *PI;
//    *frho = ((PI-bt2) * t1 + t2) / denom;
//    *ftau = (-bt2 * t1 + t2) / denom;
//
//    if (*frho < 0.0) *frho=0.0;
//    if (*ftau < 0.0) *ftau=0.0;
//}

void volscatt(float sza, float vza, float raa, float ttl, float *chi_s, float *chi_o, float *frho, float *ftau)
{
    // rd: 角度与弧度的转换常量，3.1415926/180.0 表示 π/180，用于将角度转换为弧度
    float rd = 3.1415926 / 180.0;

    // 计算太阳天顶角（SZA）和观测天顶角（VZA）的余弦和正弦值
    float costs = cos(rd * sza);  // 太阳天顶角的余弦值
    float costo = cos(rd * vza);  // 观测天顶角的余弦值
    float sints = sin(rd * sza);  // 太阳天顶角的正弦值
    float sinto = sin(rd * vza);  // 观测天顶角的正弦值

    // 计算太阳方位角（RAA）的余弦值
    float cosraa = cos(rd * raa);  // 太阳方位角的余弦值
    float raar = rd * raa;  // 将太阳方位角转换为弧度

    // 计算散射角度ttl（通常是叶片或土壤的散射角度）的余弦和正弦值
    float costl = cos(rd * ttl);  // 散射角的余弦值
    float sintl = sin(rd * ttl);  // 散射角的正弦值

    // 计算散射的几何因子
    float cs = costl * costs;  // 太阳天顶角与散射角度的几何因子（与散射方向相关）
    float co = costl * costo;  // 观测天顶角与散射角度的几何因子（与观测方向相关）

    // 计算散射角度的垂直分量
    float ss = sintl * sints;  // 太阳方向上的垂直分量
    float so = sintl * sinto;  // 观测方向上的垂直分量

    // 初始化一些常数和临时变量
    float cosbts = 5.0;  // 用于计算散射的常数，表示散射角度的余弦
    float cosbto = 5.0;  // 用于计算透过的常数，表示透过角度的余弦
    float bts = 0.0, ds = 0.0, bto, doo;

    // 判断太阳和观测方向的散射角度是否有效（防止除零错误）
    if (abs(ss) > 1e-6)  // 如果太阳方向上的垂直分量大于一个小值
        cosbts = -cs / ss;  // 计算太阳散射角度的余弦值

    if (abs(so) > 1e-6)  // 如果观测方向上的垂直分量大于一个小值
        cosbto = -co / so;  // 计算观测透过角度的余弦值

    // 如果cosbts（太阳散射角度的余弦值）小于1.0，表示散射角有效
    if (abs(cosbts) < 1.0)
    {
        bts = acos(cosbts);  // 计算散射角度（以弧度表示）
        ds = ss;  // 设置太阳方向的垂直分量
        (*chi_s) = 2.0 / PI * ((bts - PI * 0.5) * cs + sin(bts) * ss);  // 计算散射系数chi_s
    }
    else  // 如果cosbts大于等于1.0，表示散射角度过大，进行特殊处理
    {
        bts = 3.1415926;  // 设定为π，表示极限散射角度
        ds = cs;  // 设置散射方向的垂直分量
        (*chi_s) = 2.0 / PI * ((bts - PI * 0.5) * cs + sin(bts) * ss);  // 计算散射系数chi_s
    }

    // 处理观测方向的透过角度
    if (abs(cosbto) < 1.0)  // 如果透过角度的余弦值有效
    {
        bto = acos(cosbto);  // 计算透过角度（以弧度表示）
        doo = so;  // 设置观测方向的垂直分量
    }
    else  // 如果cosbto无效，表示透过角度超过范围
    {
        if (vza < 90.0)  // 如果观测天顶角小于90°
        {
            bto = PI;  // 设定透过角度为π，表示极限透过角度
            doo = co;  // 设置透过方向的垂直分量
        }
        else  // 如果观测天顶角大于等于90°
        {
            bto = 0.0;  // 设定透过角度为0，表示极限透过角度
            doo = -co;  // 设置透过方向的垂直分量
        }
    }

    (*chi_o) = 2.0 / PI * ((bto - PI * 0.5) * co + sin(bto) * so);  // 计算透过系数chi_o

    // 计算散射和透过角度之间的差值
    float btran1 = abs(bts - bto);  // 计算散射角度和透过角度之间的差值
    float btran2 = PI - abs(bts + bto - PI);  // 计算散射角度和透过角度之间的另一种差值

    // 根据方位角（RAA）选择适当的角度
    float bt1, bt2, bt3;
    if (raar <= btran1)  // 如果太阳方位角小于等于btran1
    {
        bt1 = raar;  // 设置bt1为太阳方位角
        bt2 = btran1;  // 设置bt2为散射角度和透过角度之间的差值
        bt3 = btran2;  // 设置bt3为散射角度和透过角度之间的另一种差值
    }
    else  // 如果太阳方位角大于btran1
    {
        bt1 = btran1;  // 设置bt1为散射角度和透过角度之间的差值
        if (raar <= btran2)  // 如果太阳方位角小于等于btran2
        {
            bt2 = raar;  // 设置bt2为太阳方位角
            bt3 = btran2;  // 设置bt3为散射角度和透过角度之间的另一种差值
        }
        else  // 如果太阳方位角大于btran2
        {
            bt2 = btran2;  // 设置bt2为散射角度和透过角度之间的另一种差值
            bt3 = raar;  // 设置bt3为太阳方位角
        }
    }

    // 计算散射和透过的贡献因子
    float t1, t2;
    t1 = 2.0 * cs * co + ss * so * cosraa;  // 计算散射辐射的贡献因子
    t2 = 0.0;
    if (bt2 > 0.0)
        t2 = sin(bt2) * (2.0 * ds * doo + ss * so * cos(bt1) * cos(bt3));  // 计算透过辐射的贡献因子

    // 计算最终的反射率和透过率
    float denom = 2.0 * PI * PI;  // 归一化因子
    *frho = ((PI - bt2) * t1 + t2) / denom;  // 计算反射率
    *ftau = (-bt2 * t1 + t2) / denom;  // 计算透过率

    // 确保反射率和透过率不小于零
    if (*frho < 0.0) *frho = 0.0;  // 如果反射率小于零，设为零
    if (*ftau < 0.0) *ftau = 0.0;  // 如果透过率小于零，设为零
}

void cal_LIDF(float ala, float *freq)
{
    // 定义两个数组tx1和tx2，这些值表示叶片倾斜角度的范围：每个角度区间的起始和结束值。
    float tx2[18] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85};
    float tx1[18] = {5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90};

    // n表示倾斜角度区间的数量，这里有18个区间
    int n = 18.0;

    // x数组存储每个区间的中值，tl1和tl2分别存储每个区间的起始和结束倾斜角度（单位转为弧度）
    float x[18], tl1[18], tl2[18];

    // 计算每个区间的中间值和将角度转换为弧度
    for (int k = 0; k < n; k++)
    {
        // 计算每个角度区间的中间值 x[k]（例如：0-5度之间的中值是2.5度）
        x[k] = (tx2[k] + tx1[k]) / 2.0;

        // 将每个倾斜角度（tx1和tx2）从度转换为弧度，角度转弧度的公式是 angle * (π/180)
        tl1[k] = tx1[k] * (PI / 180.0);  // 起始角度（弧度）
        tl2[k] = tx2[k] * (PI / 180.0);  // 结束角度（弧度）
    }

    // 计算离心率（excent），用于影响LIDF的计算
    float excent = exp(-1.6184e-5 * ala * ala * ala + 2.1145e-3 * ala * ala - 1.2390e-1 * ala + 3.2491);
    // 初始化一个累加变量，用于计算LIDF的归一化因子
    float sum0 = 0;

    // 遍历每个角度区间，计算每个区间的LIDF频率
    for (int i = 0; i < n - 1; i++)
    {
        // 计算区间的两端角度的投影（x1和x2），excent影响计算的精度
        float x1 = excent / (sqrt(1.0 + excent * excent * tan(tl1[i]) * tan(tl1[i])));
        float x2 = excent / (sqrt(1.0 + excent * excent * tan(tl2[i]) * tan(tl2[i])));

        // 如果excent等于1，说明叶片处于特定的极限状态，此时频率计算较为简单
        if (excent == 1)
        {
            // 计算频率，使用余弦值的差表示叶片在此角度范围内的分布
            freq[i] = abs(cos(tl1[i]) - cos(tl2[i]));
        }
        else
        {
            // 否则，继续进行更为复杂的计算，使用不同的公式计算频率
            float alpha = excent / sqrt(abs(1.0 - excent * excent));  // 计算α参数，用于调整LIDF
            float alpha2 = alpha * alpha;  // 计算α的平方

            // 计算x1和x2的平方值，用于后续的计算
            float x12 = x1 * x1;
            float x22 = x2 * x2;

            // 如果excent大于1，使用对数计算LIDF频率
            if (excent > 1)
            {
                float alpx1 = sqrt(alpha2 + x12);  // 计算α和x1的组合值
                float alpx2 = sqrt(alpha2 + x22);  // 计算α和x2的组合值

                // 计算LIDF频率（这里使用了对数和平方根等函数来进行复杂的调整）
                float dum = x1 * alpx1 + alpha2 * log(x1 + alpx1);
                freq[i] = abs(dum - (x2 * alpx2 + alpha2 * log(x2 + alpx2)));
            }
            else  // 如果excent小于等于1，使用反正弦函数进行计算
            {
                float almx1 = sqrt(alpha2 - x12);  // 计算α和x1的组合值
                float almx2 = sqrt(alpha2 - x22);  // 计算α和x2的组合值

                // 计算LIDF频率（这里使用了反正弦函数来进行复杂的调整）
                float dum = x1 * almx1 + alpha2 * asin(x1 / alpha);
                freq[i] = abs(dum - (x2 * almx2 + alpha2 * asin(x2 / alpha)));
            }
        }

        // 将当前计算的频率值累加到总和
        sum0 = sum0 + freq[i];
    }

    // 对频率数组进行归一化处理，将每个频率值除以总和，保证频率总和为1
    for (int i = 0; i < n; i++)
        freq[i] = freq[i] / sum0;
}

//void diffuseScatter_VNIR(float lai, float rho,float tau,float rs,float sza,float Esun,float Esky,float *diffuserad_leaf,float *diffuserad_soil, float vza =0,float raa = 0,float thm=53)
//{
//
//    float rd = PI/180.0;
//    float epsc = 1-rho-tau;
//    float epss = 1-rs;
//    float cts = cos(sza*rd);
//    float cto = cos(vza*rd);
//    float ctscto = cts*cto;
//    float tan_vza = tan(vza*rd);
//    float sin_sza = cos(sza*rd);
//    float tan_sza = tan(sza*rd);
//
//    float    ks = 0.0;
//    float    ko = 0.0;
//    float    bf = 0.0;
//    float    sob = 0.0;
//    float    sof = 0.0;
//    float freq[18];
//    cal_LIDF(thm,freq);
//    float na = 18;
//    float dlza = 5.0;
//
//    for(int k =0;k<na;k++)
//    {
//        float ttl = 2.5+5.0*k;
//        float ctl = cos(ttl * rd);
//        float chi_s,chi_o,frho,ftau;
//
//        volscatt(sza, vza, raa, ttl, &chi_s, &chi_o, &frho, &ftau);
//
//        float ksli = chi_s / cts;
//        float koli = chi_o / cto;
//
//
//        float sobli = frho * PI / ctscto;
//        float sofli = ftau * PI / ctscto;
//        float bfli = ctl*ctl;
//
//        ks = ks + ksli*freq[k];
//        ko = ko + koli*freq[k];
//
//        bfli = ctl * ctl;
//        bf = bf + bfli*freq[k];
//        sob = sob + sobli * freq[k];
//        sof = sof + sofli * freq[k];
//    }
//
//    float sdb = 0.5 * (ks + bf);
//    float sdf = 0.5 * (ks - bf);
//    float dob = 0.5 * (ko + bf);
//    float dof = 0.5 * (ko - bf);
//    float ddb = 0.5 * (1.0 + bf);
//    float ddf = 0.5 * (1.0 - bf);
//
//    float sigb = ddb * rho + ddf * tau;
//    float sigf = ddf * rho + ddb * tau;
//    float att = 1.0 - sigf;
//    float m2 = (att + sigb) * (att - sigb);
//
//    // ind = (m2 > 0)
//    // m2 = m2 * ind
//    float m = sqrt(m2);
//    float sb = sdb * rho + sdf * tau;
//    float sf = sdf * rho + sdb * tau;
//    float vb = dob * rho + dof * tau;
//    float vf = dof * rho + dob * tau;
//    float w = sob * rho + sof * tau;
//    float e1 = exp(-m * lai);
//    float e2 = e1 * e1;
//    float rinf = (att - m) / sigb;
//    float rinf2 = rinf * rinf;
//    float re = rinf * e1;
//    float denom = 1.0 - rinf2 * e2;
//    float J1ks = Jfunc1(ks, m, lai);
//    float J2ks = Jfunc2(ks, m, lai);
//    float J1ko = Jfunc1(ko, m, lai);
//    float J2ko = Jfunc2(ko, m, lai);
//    float Ps = (sf + sb * rinf) * J1ks;
//    float Qs = (sf * rinf + sb) * J2ks;
//    float Pv = (vf + vb * rinf) * J1ko;
//    float Qv = (vf * rinf + vb) * J2ko;
//
//    float rho_dd = rinf * (1.0 - e2) / denom;
//    float tau_dd = (1.0 - rinf2) * e1 / denom;
//    float tau_sd = (Ps - re * Qs) / denom;
//    float rho_sd = (Qs - re * Ps) / denom;
//    float tau_do = (Pv - re * Qv) / denom;
//    float rho_do = (Qv - re * Pv) / denom;
//
//    float tau_ss = exp(-ks*lai);
//    float tau_oo = exp(-ko*lai);
//
//    float rsd     = rho_sd + (tau_ss + tau_sd)*rs*tau_dd/denom;
//    float rdd     = rho_dd + tau_dd*rs*tau_dd/denom;
//
//    // soil layer upper
//    float Eplu_1 = rs*((tau_ss+tau_sd)*Esun+tau_dd*Esky)/denom;
//    float E0 = rho_sd *Esun + rho_dd*Esky + tau_dd*Eplu_1;
//    float Emin_1 = tau_sd *Esun + tau_dd*Esky + rho_dd*Eplu_1;
//
//
//    *diffuserad_soil = Emin_1 * epss;
//    *diffuserad_leaf = E0 * epsc;
//}

void diffuseScatter_VNIR(float lai, float rho, float tau, float rs, float sza, float Esun, float Esky, float *diffuserad_leaf, float *diffuserad_soil, float vza = 0, float raa = 0, float thm = 53)
{
    // rd: 弧度转换常量，PI/180.0 用于角度转换为弧度
    float rd = PI / 180.0;

    // 计算叶片吸收率 epsc，epsc = 1 - rho - tau
    // 其中，rho 是反射率，tau 是透过率，1 - rho - tau 是剩余的吸收率
    float epsc = 1 - rho - tau;

    // 计算土壤吸收率 epss，epss = 1 - rs
    // rs 是土壤的反射率，1 - rs 为土壤的吸收率
    float epss = 1 - rs;

    // 计算太阳天顶角的余弦值 cos(sza)，用于辐射的衰减计算
    float cts = cos(sza * rd);

    // 计算观察天顶角的余弦值 cos(vza)，用于描述观测角度的影响
    float cto = cos(vza * rd);

    // 计算太阳天顶角和观察天顶角的余弦值的乘积
    float ctscto = cts * cto;

    // 计算观察天顶角的正切值 tan(vza)，用于描述视角对散射的影响
    float tan_vza = tan(vza * rd);

    // 计算太阳天顶角的正切值 tan(sza)，用于描述太阳高度角对辐射的影响
    float sin_sza = cos(sza * rd);
    float tan_sza = tan(sza * rd);

    // 初始化计算结果的变量
    float ks = 0.0;  // 叶片的散射系数
    float ko = 0.0;  // 土壤的散射系数
    float bf = 0.0;  // 与散射相关的因子
    float sob = 0.0; // 叶片的散射辐射
    float sof = 0.0; // 土壤的散射辐射

    // 定义一个频率数组，存储光照角度下的分布频率
    float freq[18];
    cal_LIDF(thm, freq);  // 调用 LIDF 函数计算光照分布频率
    float na = 18;  // 假设有18个不同的光照角度，基于LIDF的假设
    float dlza = 5.0;  // 每个光照角度之间的间隔为5度，表示LIDF中每个角度区间的宽度

    // 遍历光照角度，计算每个角度的辐射贡献
    for (int k = 0; k < na; k++)
    {
        // ttl 是当前光照角度，5度递增
        float ttl = 2.5 + 5.0 * k;
        float ctl = cos(ttl * rd);  // 计算当前角度的余弦值

        float chi_s, chi_o, frho, ftau;
        // 计算散射和透过率的中间结果，chi_s表示散射，chi_o表示透过，frho和ftau分别是反射和透过的因子
        volscatt(sza, vza, raa, ttl, &chi_s, &chi_o, &frho, &ftau);

        // 计算叶片和土壤的散射系数
        float ksli = chi_s / cts;  // 叶片的散射系数，按太阳天顶角进行归一化
        float koli = chi_o / cto;  // 土壤的散射系数，按视角天顶角进行归一化

        // 计算叶片和土壤的散射辐射 sobli 和 sofli
        float sobli = frho * PI / ctscto;  // 叶片的散射辐射，按太阳和观察天顶角进行修正
        float sofli = ftau * PI / ctscto;  // 土壤的散射辐射，按太阳和观察天顶角进行修正

        // 计算散射辐射的权重因子 bfli
        float bfli = ctl * ctl;  // 该因子与当前角度的余弦平方成正比

        // 累加所有光照角度下的贡献
        ks = ks + ksli * freq[k];
        ko = ko + koli * freq[k];
        bf = bf + bfli * freq[k];
        sob = sob + sobli * freq[k];
        sof = sof + sofli * freq[k];
    }

    // 计算不同方向的辐射强度
    float sdb = 0.5 * (ks + bf);  // 叶片和土壤的散射辐射强度（增强散射）
    float sdf = 0.5 * (ks - bf);  // 叶片和土壤的透过辐射强度（减弱透过）
    float dob = 0.5 * (ko + bf);  // 叶片和土壤的反射辐射强度（增强反射）
    float dof = 0.5 * (ko - bf);  // 叶片和土壤的透过反射强度（减弱透过反射）
    float ddb = 0.5 * (1.0 + bf);  // 叶片和土壤的整体辐射因子（增强辐射）
    float ddf = 0.5 * (1.0 - bf);  // 叶片和土壤的整体辐射因子（减弱辐射）

    // 根据计算结果，得到叶片和土壤的吸收辐射强度
    float sigb = ddb * rho + ddf * tau;  // 叶片和土壤的吸收辐射因子（增强辐射的影响）
    float sigf = ddf * rho + ddb * tau;  // 叶片和土壤的吸收辐射因子（减弱辐射的影响）
    float att = 1.0 - sigf;  // 计算辐射的衰减因子
    float m2 = (att + sigb) * (att - sigb);  // 计算辐射的平方因子，考虑吸收和透过的影响

    // m2的值必须大于0，确保物理意义的合理性
    float m = sqrt(m2);  // 计算衰减因子的平方根，得到最终的辐射衰减系数

    // 根据衰减因子计算辐射强度
    float sb = sdb * rho + sdf * tau;  // 计算叶片的反射辐射强度
    float sf = sdf * rho + sdb * tau;  // 计算叶片的透过辐射强度
    float vb = dob * rho + dof * tau;  // 计算土壤的反射辐射强度
    float vf = dof * rho + dob * tau;  // 计算土壤的透过辐射强度
    float w = sob * rho + sof * tau;  // 计算总辐射强度，综合叶片和土壤的散射效应

    // 计算光在叶片中的衰减因子 e1 和 e2，分别表示光在叶片中的一次和二次衰减
    float e1 = exp(-m * lai);  // 用指数衰减模型计算光在叶片中的衰减
    float e2 = e1 * e1;  // 二次衰减

    // 计算反射率因子 rinf 和 rinf2，用于修正透过率和反射率
    float rinf = (att - m) / sigb;
    float rinf2 = rinf * rinf;

    // 计算反射率和透过率修正因子
    float re = rinf * e1;
    float denom = 1.0 - rinf2 * e2;

    // 调用 Jfunc1 和 Jfunc2 函数，计算散射系数与辐射强度的关系
    float J1ks = Jfunc1(ks, m, lai);
    float J2ks = Jfunc2(ks, m, lai);
    float J1ko = Jfunc1(ko, m, lai);
    float J2ko = Jfunc2(ko, m, lai);

    // 计算叶片和土壤的散射辐射 Ps, Qs, Pv 和 Qv
    float Ps = (sf + sb * rinf) * J1ks;
    float Qs = (sf * rinf + sb) * J2ks;
    float Pv = (vf + vb * rinf) * J1ko;
    float Qv = (vf * rinf + vb) * J2ko;

    // 计算叶片和土壤的反射率和透过率修正因子
    float rho_dd = rinf * (1.0 - e2) / denom;
    float tau_dd = (1.0 - rinf2) * e1 / denom;

    // 计算叶片和土壤之间的透过率（tau_sd）
    // 其中：
    // Ps 为叶片的散射辐射强度
    // Qs 为与散射相关的辐射强度函数
    // re 为反射率修正因子，考虑了透过率与散射辐射的相互影响
    // denom 为归一化因子，确保辐射传输的能量守恒
    float tau_sd = (Ps - re * Qs) / denom;

    // 计算叶片和土壤之间的反射率（rho_sd）
    // 其中：
    // Qs 为与散射相关的辐射强度函数
    // Ps 为叶片的散射辐射强度
    // re 为反射率修正因子
    // denom 为归一化因子，用于将不同辐射强度归一化处理
    float rho_sd = (Qs - re * Ps) / denom;

    // 计算叶片和土壤的透过率 tau_ss 和 tau_oo
    float tau_ss = exp(-ks * lai);
    float tau_oo = exp(-ko * lai);

    // 计算土壤的反射率和透过率，进一步修正辐射传输
    float rsd = rho_sd + (tau_ss + tau_sd) * rs * tau_dd / denom;
    float rdd = rho_dd + tau_dd * rs * tau_dd / denom;

    // 计算土壤上层的辐射 Eplu_1，考虑了多次反射和透过的影响
    float Eplu_1 = rs * ((tau_ss + tau_sd) * Esun + tau_dd * Esky) / denom;

    // 计算土壤的总辐射 E0
    float E0 = rho_sd * Esun + rho_dd * Esky + tau_dd * Eplu_1;

    // 计算土壤的最小辐射 Emin_1，考虑了散射和透过效应
    float Emin_1 = tau_sd * Esun + tau_dd * Esky + rho_dd * Eplu_1;

    // 将叶片和土壤的散射辐射结果存储到输出变量中
    *diffuserad_soil = Emin_1 * epss;
    *diffuserad_leaf = E0 * epsc;
}


void diffuseScatter_VNIR_Urban(float ref_roof, float ref_wall, float ref_street, float svf_s, float svf_w, float frs, float frh, float fws, float fwh, float fss, float fsh, float BAI, float sza,float Esun,float Esky,float *diffuserad_roof,float *diffuserad_wall, float *diffuserad_street, float vza =0,float raa = 0,float thm=53)
{
    // 计算墙壁吸收的散射能量
    float p= 0.88 * (1 - exp(-0.7 * pow(BAI, 0.75))); //需要计算，先写定值
    float ud = (1 - p) / 2.0; //不准确但是这里简单认为向上向下概率相同
    float rho_wall = ((1 - svf_w) * ref_wall * ud);

    *diffuserad_roof = 0;

    float Lwall1 = (Esun * ref_wall * p * (1 - ref_wall)) / (1 - ref_wall * p);
    float Lwall21 = (Esun * (fwh + fws) * ref_wall * ud) / (1 - ref_wall * p);
    float Lwall2 = ((Lwall21 + Esun) * ref_street * (1 - rho_wall)) / (1 - ref_street * rho_wall);
    *diffuserad_wall = Lwall1 + Lwall2;

    float Lstreet11 = (Esun * (fwh + fws) * ref_wall * ud) / (1 - ref_wall * p);
    float Lstreet1 = (Lstreet11 * (1 - ref_street)) / (1 - ref_street * rho_wall);
    float Lstreet2 = (Esun * (fss + fsh) *  (1 - ref_street)) / (1 - ref_street * rho_wall);
    *diffuserad_street = Lstreet1 + Lstreet2;

}

void diffuseScatter_TIR(float lai, float rho,float tau,float rs,float sza,float Esun,float Esky,float Tss, float Tsh, float Tvs,float Tvh, float *diffuserad_leaf,float *diffuserad_soil, float vza =0,float raa = 0,float thm=53)
{

    float rd = 3.1415926/180.0;
    float epsc = 1-rho-tau;
    float epss = 1-rs;
    float cts = cos(sza*rd);
    float cto = cos(vza*rd);
    float ctscto = cts*cto;
    float tan_vza = tan(vza*rd);
    float sin_sza = cos(sza*rd);
    float tan_sza = tan(sza*rd);

    float    ks = 0.0;
    float    ko = 0.0;
    float    bf = 0.0;
    float    sob = 0.0;
    float    sof = 0.0;
    float freq[18];
    cal_LIDF(thm,freq);
    float na = 18;
    float dlza = 5.0;

    for(int k =0;k<na;k++)
    {
        float ttl = 2.5+5.0*k;
        float ctl = cos(ttl * rd);
        float chi_s,chi_o,frho,ftau;

        volscatt(sza, vza, raa, ttl, &chi_s, &chi_o, &frho, &ftau);

        float ksli = chi_s / cts;
        float koli = chi_o / cto;


        float sobli = frho * PI / ctscto;
        float sofli = ftau * PI / ctscto;
        float bfli = ctl*ctl;

        ks = ks + ksli*freq[k];
        ko = ko + koli*freq[k];

        bfli = ctl * ctl;
        bf = bf + bfli*freq[k];
        sob = sob + sobli * freq[k];
        sof = sof + sofli * freq[k];
    }

    float sdb = 0.5 * (ks + bf);
    float sdf = 0.5 * (ks - bf);
    float dob = 0.5 * (ko + bf);
    float dof = 0.5 * (ko - bf);
    float ddb = 0.5 * (1.0 + bf);
    float ddf = 0.5 * (1.0 - bf);

    float sigb = ddb * rho + ddf * tau;
    float sigf = ddf * rho + ddb * tau;
    float att = 1.0 - sigf;
    float m2 = (att + sigb) * (att - sigb);

    // ind = (m2 > 0)
    // m2 = m2 * ind
    float m = sqrt(m2);
    float sb = sdb * rho + sdf * tau;
    float sf = sdf * rho + sdb * tau;
    float vb = dob * rho + dof * tau;
    float vf = dof * rho + dob * tau;
    float w = sob * rho + sof * tau;
    float e1 = exp(-m * lai);
    float e2 = e1 * e1;
    float rinf = (att - m) / sigb;
    float rinf2 = rinf * rinf;
    float re = rinf * e1;
    float denom = 1.0 - rinf2 * e2;
    float J1ks = Jfunc1(ks, m, lai);
    float J2ks = Jfunc2(ks, m, lai);
    float J1ko = Jfunc1(ko, m, lai);
    float J2ko = Jfunc2(ko, m, lai);
    float Ps = (sf + sb * rinf) * J1ks;
    float Qs = (sf * rinf + sb) * J2ks;
    float Pv = (vf + vb * rinf) * J1ko;
    float Qv = (vf * rinf + vb) * J2ko;

    float rho_dd = rinf * (1.0 - e2) / denom;
    float tau_dd = (1.0 - rinf2) * e1 / denom;
    float tau_sd = (Ps - re * Qs) / denom;
    float rho_sd = (Qs - re * Ps) / denom;
    float tau_do = (Pv - re * Qv) / denom;
    float rho_do = (Qv - re * Pv) / denom;

    float tau_ss = exp(-ks*lai);
    float tau_oo = exp(-ko*lai);

    float rsd     = rho_sd + (tau_ss + tau_sd)*rs*tau_dd/denom;
    float rdd     = rho_dd + tau_dd*rs*tau_dd/denom;

    float fHs = (1-rinf2)*(1-rs)/(1-rinf*rs);
    float fHc =  m *(1-rinf);
    float fbottom = (rs - rinf)/(1-rinf*rs);


    float Ess = SCI::StefanBoltzmann(Tss);
    float Esh = SCI::StefanBoltzmann(Tsh);
    float Evs = SCI::StefanBoltzmann(Tvs);
    float Evh = SCI::StefanBoltzmann(Tvh);

    float pss = 0.5,pvs = 0.5;
    float Es = Ess * pss + Esh * (1-pss);
    float Ev = Evs * pvs + Evh * (1-pvs);


    int con = 1;
    // three layer, atmosphere 0, m_canopy 1, soil 2;
    // f1 from upper to bottom;
    // f2 from bottom to upper;
    float f10,f11,f12,f20,f21,f22;
    float f1top = Esky;

    f10 = f1top;
    f11 = f10*(1-fHc) + Ev * fHc;
    f21 = fbottom * f11 + Es * fHs;

    float Emin = (f11 + rinf*f21)/(1-rinf*rinf);
    float Eplu = (f21 + rinf*f11)/(1-rinf*rinf);


    // float Eplu_1 = rs*((tau_ss+tau_sd)*Esun+tau_dd*Esky)/denom;
    // float E0 = rho_sd *Esun + rho_dd*Esky + tau_dd*Eplu_1;
    // float Emin_1 = tau_sd *Esun + tau_dd*Esky + rho_dd*Eplu_1;

    *diffuserad_soil = Emin * epss;
    *diffuserad_leaf = Eplu * epsc+f1top*epsc;
}

// void diffuseScatter_TIR_urban(float bai, float svf_w, float svf_s, float rrho,float wrho,float srho,float sza,float Esun,float Esky,float Trs, float Trh, float Tws,float Twh, float Tss, float Tsh, float *diffuserad_roof, float *diffuserad_wall,  float *diffuserad_street, float vza =0,float raa = 0,float thm=53)
// {
//
//     float rd = 3.1415926/180.0;
//     float epsr = 1-rrho;
//     float epsw = 1-wrho;
//     float epss = 1-srho;
//
//     float cts = cos(sza*rd);
//     float cto = cos(vza*rd);
//     float ctscto = cts*cto;
//     float tan_vza = tan(vza*rd);
//     float sin_sza = cos(sza*rd);
//     float tan_sza = tan(sza*rd);
//
//     float Ers = SCI::StefanBoltzmann(Trs);
//     float Erh = SCI::StefanBoltzmann(Trh);
//     float Ews = SCI::StefanBoltzmann(Tws);
//     float Ewh = SCI::StefanBoltzmann(Twh);
//     float Ess = SCI::StefanBoltzmann(Tss);
//     float Esh = SCI::StefanBoltzmann(Tsh);
//
//     // 混合辐射源的权重（假设50%均匀分布）
//     float prs = 0.5, pws = 0.5, pss = 0.5;
//     float Er = Ers * prs + Esh * (1 - prs);  // 混合太阳辐射
//     float Ew = Ews * pws + Ewh * (1 - pws);  // 混合太阳辐射
//     float Es = Ess * pss + Esh * (1 - pss);  // 混合太阳辐射
//
//     float term1 = epss * svf_s * Esky;
//     float term3 = epss * epsw * (1 - svf_s) * Ew;
//     float term4 = epss * (1 - epsw) * (1 - svf_s) * svf_w * Esky;
//     float term5 = epss * epsw * (1 - epsw) * (1 - svf_s) * (1 - 2 * svf_w) * Ew;
//     float term6 = epss * (1 - epsw) * (1 - svf_s) * svf_w * epss * Es;
//     // 总的 L_r^*
//     float diffuse_street = term1 + term3 + term4 + term5 + term6;
//
//     float temp1 = epsw * svf_w * Esky;
//     float temp3 = epsw * svf_w * epss * Es;
//     float temp4 = epsw * epsw * (1 - 2 * svf_w) * Esky;
//     float temp5 = epsw * (1 - epss) * svf_w * svf_s * Esky;
//     float temp6 = epsw * (1 - epsw) * svf_w * (1 - 2 * svf_w) * Esky;
//     float temp7 = epsw * epsw * (1 - epsw) * (1 - 2 * svf_w) * (1 - 2 * svf_w) * Esky;
//     float temp8 = epsw * epsw * (1 - epss) * svf_w * (1 - svf_s) * Esky;
//     float temp9 = epsw * (1 - epsw) * svf_w * (1 - 2 * svf_w) * epss * Es;
//     // 总的 L_w^*
//     float diffuse_wall = temp1 + temp3 + temp4 + temp5 + temp6 + temp7 + temp8 + temp9;
//
//     float diffuse_roof = Esky * epsr;
//
//     *diffuserad_street = diffuse_street;
//     *diffuserad_wall = diffuse_wall;
//     *diffuserad_roof = diffuse_roof;
// }

void diffuseScatter_TIR_urban(float bai, float svf_w, float svf_s, float frs, float frh, float fws, float fwh, float fss, float fsh, float rrho,float wrho,float srho,float sza,float Esun,float Esky,float Trs, float Trh, float Tws,float Twh, float Tss, float Tsh, float *diffuserad_roof, float *diffuserad_wall,  float *diffuserad_street, float vza =0,float raa = 0,float thm=53)
{

    float rd = 3.1415926/180.0;
    float epsr = 1-rrho;
    float epsw = 1-wrho;
    float epss = 1-srho;

    float cts = cos(sza*rd);
    float cto = cos(vza*rd);
    float ctscto = cts*cto;
    float tan_vza = tan(vza*rd);
    float sin_sza = cos(sza*rd);
    float tan_sza = tan(sza*rd);

    float Ers = SCI::StefanBoltzmann(Trs);
    float Erh = SCI::StefanBoltzmann(Trh);
    float Ews = SCI::StefanBoltzmann(Tws);
    float Ewh = SCI::StefanBoltzmann(Twh);
    float Ess = SCI::StefanBoltzmann(Tss);
    float Esh = SCI::StefanBoltzmann(Tsh);

    // 混合辐射源的权重（假设50%均匀分布）
    float prs = 0.5, pws = 0.5, pss = 0.5;
    float Er = Ers * prs + Erh * (1 - prs);  // 混合太阳辐射
    float Ew = Ews * pws + Ewh * (1 - pws);  // 混合太阳辐射
    float Es = Ess * pss + Esh * (1 - pss);  // 混合太阳辐射

    // 计算墙壁吸收的散射能量
    float p= 0.88 * (1 - exp(-0.7 * pow(bai, 0.75))); //需要计算，先写定值
    float ud = (1 - p) / 2.0; //不准确但是这里简单认为向上向下概率相同
    float rho_wall = ((1 - svf_w) * rrho * ud);

    *diffuserad_roof = Esky * epsr + epsr * Er;

    float Lwall1 = (Ew * epsw * p * (1 - wrho)) / (1 - wrho * p);
    float Lwall21 = (Ew * epsw * (fwh + fws) * ud) / (1 - wrho * p);
    float Lwall2 = ((Lwall21 + Ew * epsw) * wrho * (1 - rho_wall)) / (1 - wrho * rho_wall);
    *diffuserad_wall = Lwall1 + Lwall2;

    float Lstreet11 = (Es * epss * (fwh + fws) * ud) / (1 - wrho * p);
    float Lstreet1 = (Lstreet11 * (1 - srho)) / (1 - srho * rho_wall);
    float Lstreet2 = ((Es * epss / (1 - wrho)) * (fss + fsh) *  (1 - srho)) / (1 - srho * rho_wall);
    *diffuserad_street = Lstreet1 + Lstreet2;
}


void RT::optical(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio) {


    auto & fluspectCoeff = m_pDefined->m_optCoeff;
    auto & fluspectParam = m_pPixelio->m_pInputset->leafbio.fp;
    auto & spectral = m_pPixelio->m_pStaticVariable->spectal;
    auto & canopy = m_pPixelio->m_pInputset->canopy;

    if(canopy.lai > 0) {
        m_leafopt.fluspect(fluspectCoeff, fluspectParam, spectral);
        spectral.leafRefl_ir = m_pDefined->m_spectral.leafRefl_ir;
        spectral.leafTran_ir = m_pDefined->m_spectral.leafTran_ir;
    }

    for(int i=0;i<N1;i++)
    {
        spectral.wl_[i] = m_pDefined->m_atomcond.wl[i];
        spectral.soilRefl_[i] = m_pDefined->m_spectral.soilRefl_[i];
    }
    spectral.soilRefl_ir = m_pDefined->m_spectral.soilRefl_ir;


}


float calculate_effective_lai_crown(Canopy &canopy, double sza)
{
    float lai_temp = canopy.lai;
    double xza_temp = sza;
    double G = canopy.Gleaf;
    double std_temp = canopy.treeStand;
    float canopyheight_temp = canopy.canopyHeight;
    float b_temp = canopy.b;
    float radi_vertical_temp = canopyheight_temp / 2.0;
    float radi_horizontal_temp = canopyheight_temp / (2.0 * b_temp);

    double tgthx = std::tan(xza_temp * PI / 180.0);
    double cthetx = std::cos(xza_temp * PI / 180.0);
    double areav = std::sqrt(radi_vertical_temp * radi_vertical_temp + std::pow(radi_horizontal_temp * tgthx, 2)) * PI * radi_vertical_temp;
    double bv_in = std::exp(-lai_temp / (areav * std_temp) * G / cthetx);
    double bv = std::exp(-areav * (1 - bv_in) * std_temp);
    double uv = std::cos(xza_temp * PI / 180.0);
    double clumping_index = -std::log(bv) * uv / (lai_temp * G);
    float effective_lai = lai_temp * clumping_index;

    return effective_lai;
}

void RT::netrad_shortwave(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio) {
    // 获取动态变量和静态变量
    NetRad &netrad = m_pPixelio->m_pDynamicVariable->netrad;  // 短波净辐射
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;  // 光谱信息
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;  // 树冠信息
    float sza = m_pPixelio->m_angle.sza;  // 太阳高度角
    int k_node = m_pPixelio->k_node;  // 节点索引
    Meteo &meteo = m_pPixelio->m_pInputset->vMeteo[k_node];  // 气象数据
    float *direct_ = m_pDefined->m_atomcond.fesun;  // 直接辐射
    float *diffuse_ = m_pDefined->m_atomcond.fesky;  // 散射辐射
    float *wl_ = m_pDefined->m_atomcond.wl;  // 波长

    // 初始化净辐射变量
    netrad.diffuseVrad_leaf = 0;  // 叶片散射辐射
    netrad.directVrad_leaf = 0;  // 叶片直接辐射
    netrad.diffuseVrad_soil = 0;  // 土壤散射辐射
    netrad.directVrad_soil = 0;  // 土壤直接辐射
    netrad.directPrad_leaf = 0;  // 叶片直接辐射强度
    netrad.diffusePrad_leaf = 0.0;  // 叶片散射辐射强度

    // 常数定义
    float A = 6.02214E23;  // 阿伏伽德罗常数
    float H = 6.6262E-34;  // 普朗克常数
    float C = 299792458.0;  // 光速

    // 条件判断，若入射辐射小于20或太阳高度角大于75，直接返回
    if (meteo.rin < 20 || sza > 75)
        return;

    float lai = canopy.lai;  // 获取叶面积指数

    // 遍历所有光谱波段
    for (int kband = 0; kband < N1; kband++) {
        // 计算直接辐射和散射辐射
        float Esun = meteo.rin * direct_[kband] * 0.001;  // 直接辐射强度
        float Esky = meteo.rli * diffuse_[kband] * 0.001;  // 散射辐射强度
        float diffuse_soil = 0.0;  // 初始化土壤散射辐射
        float diffuse_leaf = 0.0;  // 初始化叶片散射辐射
        float direct_soil = 0.0;  // 初始化土壤直接辐射
        float direct_leaf = 0.0;  // 初始化叶片直接辐射

        // 获取当前波段的光谱特性
        float lrho = spectral.leafRefl_[kband];  // 叶片反射率
        float ltau = spectral.leafTran_[kband];  // 叶片透射率
        float rs = spectral.soilRefl_[kband];  // 土壤反射率
        float wl = wl_[kband];  // 当前波段的波长

        // 如果LAI大于0，计算叶片和土壤的辐射
        if (lai > 0) {

            // // 将LAI转化为有效LAI计算净辐射，但是在方向亮温计算是否有影响存疑
            // int type_temp = canopy.type;
            // // 定义要检查的类型集合
            // std::vector<int> canopy_types = {1, 2, 3, 4, 5, 6, 7};
            // // 判断条件
            // bool ind_crown = std::find(canopy_types.begin(), canopy_types.end(), type_temp) != canopy_types.end();
            // if (ind_crown) {
            //     // lai = calculate_effective_lai_crown(canopy, sza);
            //     lai = calculate_effective_lai_crown(canopy, sza);
            // }

            // 计算叶片和土壤的散射辐射
            diffuseScatter_VNIR(lai, lrho, ltau, rs, sza, Esun, Esky, &diffuse_leaf, &diffuse_soil);

            // 更新净辐射值
            netrad.diffuseVrad_leaf += diffuse_leaf;  // 更新叶片散射辐射
            netrad.diffuseVrad_soil += diffuse_soil;  // 更新土壤散射辐射
            netrad.directVrad_leaf += (Esun / cos(sza * PI / 180.0)) * (1 - lrho - ltau);  // 更新叶片直接辐射
            netrad.directVrad_soil += (Esun / cos(sza * PI / 180.0)) * (1 - rs);  // 更新土壤直接辐射

            // 如果波长在可见光范围内（400-700nm），则计算辐射强度
            if (wl >= 400 && wl <= 700) {
                netrad.directPrad_leaf += (Esun / cos(sza * PI / 180.0)) * (1 - lrho - ltau) * wl * (1e-3) / (A * H * C);  // 叶片直接辐射强度
                netrad.diffusePrad_leaf += diffuse_leaf * wl * (1e-3) / (A * H * C);  // 叶片散射辐射强度
            }
        } else {
            // 如果LAI为0，只有土壤辐射
            netrad.directVrad_soil += Esun * (1 - rs);  // 更新土壤直接辐射
            netrad.diffuseVrad_soil += Esky * (1 - rs);  // 更新土壤散射辐射
        }
    }
}


void RT::netrad_longwave(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO>  m_pPixelio)
{

    // 获取需要操作的变量引用
    NetRad &netrad = m_pPixelio->m_pDynamicVariable->netrad; // 存储辐射变量的引用
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal; // 光谱信息的引用
    Canopy &canopy = m_pPixelio->m_pInputset->canopy; // 冠层参数的引用
    int k_node = m_pPixelio->k_node; // 节点索引
    Meteo &meteo = m_pPixelio->m_pInputset->vMeteo[k_node]; // 气象数据的引用
    float *direct_ = m_pDefined->m_atomcond.fesun; // 太阳直射辐射
    float *diffuse_ = m_pDefined->m_atomcond.fesky; // 天空漫射辐射
    float *wl_ = m_pDefined->m_atomcond.wl; // 光谱波段的波长
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal; // 热力学变量的引用
    //NetRad &netrad = m_pPixelio->m_pDynamicVariable->netrad;

    netrad.diffuseTrad_leaf = 0;
    netrad.directTrad_leaf = 0;
    netrad.diffuseTrad_soil = 0;
    netrad.directTrad_soil = 0;

    // rad in and rad out
    float Esun = 0; // 太阳直射辐射，这里初始化为0，因为长波辐射主要考虑天空漫射
    float Esky = meteo.rli; // 天空漫射辐射
    float diffuse_soil = 0.0; // 土壤散射辐射初始化
    float diffuse_leaf = 0.0; // 叶片散射辐射初始化
    float direct_soil = 0.0; // 土壤直射辐射初始化
    float direct_leaf = 0.0; // 叶片直射辐射初始化

    // 获取热力学变量中的温度信息
    float Tss = thermal.Tsoilsunlit; // 光照土壤温度
    float Tsh = thermal.Tsoilshaded; // 阴影土壤温度
    float Tvs = thermal.Tleafsunlit; // 光照叶片温度
    float Tvh = thermal.Tleafshaded; // 阴影叶片温度

    float lai = canopy.lai;
    // compo info
    float lrho = spectral.leafRefl_ir;
    float ltau = 0.0;
    float sza = m_pPixelio->m_angle.sza;
    float rs = spectral.soilRefl_ir;
    float wl = -1;

    if(lai >0) {
        // 将LAI转化为有效LAI计算净辐射，但是在方向亮温计算是否有影响存疑
        int type_temp = canopy.type;
        // 定义要检查的类型集合
        // std::vector<int> canopy_types = {1, 2, 3, 4, 5, 6, 7};
        std::vector<int> canopy_types = {1, 2, 3, 4, 5};
        // 判断条件
        bool ind_crown = std::find(canopy_types.begin(), canopy_types.end(), type_temp) != canopy_types.end();
        if (ind_crown) {
            lai = calculate_effective_lai_crown(canopy, sza);
        }

        //如果叶面积指数大于0,调用 diffuseScatter_TIR 函数计算叶片和土壤的散射辐射,累加叶片和土壤的散射辐射到净辐射变量中。
        diffuseScatter_TIR(lai, lrho, ltau, rs, sza, Esun, Esky, Tss, Tsh, Tvs, Tvh, &diffuse_leaf, &diffuse_soil);
        netrad.diffuseTrad_leaf += diffuse_leaf;
        netrad.diffuseTrad_soil += diffuse_soil;
    }else
    {
        //如果LAI小于或等于0，则仅计算土壤的散射辐射。
        netrad.diffuseTrad_soil += Esky *(1-rs);
    }

}


// void RT::nadirTir(std::shared_ptr<PixelIO> &m_pPixelio) {
//
//     float result = 0;
//     // 获取 PixelIO 中的各种数据
//     Canopy &canopy = m_pPixelio->m_pInputset->canopy;
//     Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
//     Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
//     Angle &angle = m_pPixelio->m_angle;
//     int knode = m_pPixelio->k_node;
//     if(canopy.lai<=0)     // 如果叶面积指数（LAI）小于等于0，表示没有植被
//     {
//         float Tss = thermal.Tsoilsunlit;        // 获取阳光照射土壤的温度
//         float rad = SCI::Planck(10.5,Tss);        // 通过 Planck 函数计算辐射值（波长 10.5 微米）
// //        m_pPixelio->m_vSkt[knode]  = SCI::invPlanck(10.5,Radss*(1-m_spectral.soilRefl_ir));
//         rad = rad *(1-spectral.soilRefl_ir);        // 考虑土壤的反射率，调整辐射值
//         //   m_pPixelio->skt =  SCI::invPlanck(10.5,Radss*(1-m_spectral.soilRefl_ir));
//         if (isnan(rad)) {
//             m_pPixelio->m_vSkt[knode] = 0;
//             return;
//         }
//         if(rad > 20 || rad < 3) {m_pPixelio->m_vSkt[knode] = 273.17; } // 如果辐射值不在合理范围内（3 到 20），将结果设为 273.17 K
//             // m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5,rad);
//         else{
//             m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5,rad);  // 否则，通过反向 Planck 函数计算温度并设置结果
// //            m_pPixelio->m_vSkt[knode] = 273.17;
//         }
//     }else
//     {
//         // 如果存在植被，获取各种反射率和温度值
//         float refl_soil = spectral.soilRefl_ir;
//         float refl_leaf =  spectral.leafRefl_ir;
// //        float Tss = thermal.Tsoilsunlit;
// //        float Tsh = thermal.Tsoilsunlit;
// //        float Tcs = thermal.Tsoilsunlit;
// //        float Tch = thermal.Tsoilsunlit;
//         float Tss = thermal.Tsoilsunlit;
//         float Tsh = thermal.Tsoilshaded;
//         float Tcs = thermal.Tleafsunlit;
//         float Tch = thermal.Tleafshaded;
//
//         // 计算土壤和叶片的发射率
// //        float emis_soil = 0.955;
// //        float emis_leaf = 0.985;
//         float emis_soil = 1-refl_soil;
//         float emis_leaf = 1-refl_leaf;
//
//         float fss,fsh,fcs,fch,mss,msh,mcs,mch;
//         tirt_direct(canopy,angle,fss,fsh,fcs,fch);        // 计算直接辐射传输因子
//         tirt_scatter(canopy,angle,spectral,mss,msh,mcs,mch);        // 计算散射辐射传输因子
//
//         // 通过 Planck 函数计算各种温度对应的辐射值
//         float Radss = SCI::Planck(10.5,Tss);
//         float Radsh = SCI::Planck(10.5,Tsh);
//         float Radcs = SCI::Planck(10.5,Tcs);
//         float Radch = SCI::Planck(10.5,Tch);
//         // 计算综合辐射值
//         float rad = (fss*emis_soil+mss)*Radss + (fsh*emis_soil+msh)*Radsh +(fcs*emis_leaf+mcs)*Radcs +(fch*emis_leaf+mch)*Radch;
// //      test  float rad = emis_soil*Radss ;
//         if (isnan(rad)) {
//             m_pPixelio->m_vSkt[knode] = 0;
//             return;
//         }
//         if(rad > 20 || rad < 3) {
//             m_pPixelio->m_vSkt[knode] = 273.17;
// //           m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5,rad);
//        }
//        else{
//             m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5,rad);
// //            m_pPixelio->m_vSkt[knode] = 273.17;
//        }
//     }
//
// }

void RT::nadirTir(std::shared_ptr<PixelIO> &m_pPixelio) {
    // 这个代码采用的是森林冠层模型进行天顶方向亮温解算
    // 获取 PixelIO 中的各种数据
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
    Angle &angle = m_pPixelio->m_angle;

    // 筛选均质和冠层像元
    int type_temp = canopy.type;
    // 定义要检查的类型集合
    std::vector<int> hom_types = {6, 7, 8, 9, 10, 12, 14, 15, 16};
    std::vector<int> canopy_types = {1, 2, 3, 4, 5};
    // std::vector<int> canopy_types = {1, 2, 3, 4, 5, 6, 7};
    // 判断条件
    bool ind_hom = std::find(hom_types.begin(), hom_types.end(), type_temp) != hom_types.end();
    bool ind_crown = std::find(canopy_types.begin(), canopy_types.end(), type_temp) != canopy_types.end();

    if (ind_hom) {
        int knode = m_pPixelio->k_node;
        float wavelength = 10.5f;
        float min_rad = 3.0f;
        float max_rad = 20.0f;
        float default_temp = 273.17f;

        if (canopy.lai <= 0) {
            // Bare soil case
            float Tss = thermal.Tsoilsunlit;
            float rad = SCI::Planck(wavelength, Tss) * (1 - spectral.soilRefl_ir);

            if (isnan(rad)) {
                m_pPixelio->m_vSkt[knode] = 0;
                return;
            }
            m_pPixelio->m_vSkt[knode] = (rad > max_rad || rad < min_rad)
                ? default_temp
                : SCI::invPlanck(wavelength, rad);
        } else {
            // Vegetation case
            float refl_soil = spectral.soilRefl_ir;
            float refl_leaf = spectral.leafRefl_ir;
            float emis_soil = 1 - refl_soil;
            float emis_leaf = 1 - refl_leaf;

            float fss, fsh, fcs, fch, mss, msh, mcs, mch;
            tirt_direct(canopy, angle, fss, fsh, fcs, fch);
            tirt_scatter(canopy, angle, spectral, mss, msh, mcs, mch);

            // Precompute common terms
            float fs_emis = fss * emis_soil + fsh * emis_soil;
            float ms = mss + msh;
            float fc_emis = fcs * emis_leaf + fch * emis_leaf;
            float mc = mcs + mch;

            // Compute weighted radiation
            float rad = (fss * emis_soil + mss) * SCI::Planck(wavelength, thermal.Tsoilsunlit)
                      + (fsh * emis_soil + msh) * SCI::Planck(wavelength, thermal.Tsoilshaded)
                      + (fcs * emis_leaf + mcs) * SCI::Planck(wavelength, thermal.Tleafsunlit)
                      + (fch * emis_leaf + mch) * SCI::Planck(wavelength, thermal.Tleafshaded);

            rad /= (fs_emis + ms + fc_emis + mc);

            if (isnan(rad)) {
                m_pPixelio->m_vSkt[knode] = 0;
                return;
            }
            m_pPixelio->m_vSkt[knode] = (rad > max_rad || rad < min_rad)
                ? default_temp
                : SCI::invPlanck(wavelength, rad);
        }
    }
    else if (ind_crown) {
        int knode = m_pPixelio->k_node;
        float tch_temp = thermal.Tleafsunlit;
        float tss_temp = thermal.Tsoilsunlit;
        float tsh_temp = thermal.Tsoilshaded;
        float tcs_temp = thermal.Tleafshaded;

        float lai_temp = canopy.lai;

        float vza_temp = angle.vza;
        float sza_temp = angle.sza;
        float vaa_temp = angle.vaa;
        float saa_temp = angle.saa;

        angle.vza = vza_temp;
        angle.sza = sza_temp;
        angle.vaa = vaa_temp;
        angle.saa = saa_temp;

        float refl_soil = spectral.soilRefl_ir;
        float refl_leaf = spectral.leafRefl_ir;
        float emis_s_temp = 1 - refl_soil;
        float emis_v_temp = 1 - refl_leaf;

        if (lai_temp <= 0) // 如果叶面积指数（LAI）小于等于0，表示没有植被
        {
            float rad = SCI::Planck(10.5, tss_temp); // 通过 Planck 函数计算辐射值（波长 10.5 微米）
            rad = rad * (1 - emis_s_temp); // 考虑土壤的反射率，调整辐射值
            if (isnan(rad)) {
                m_pPixelio->m_vSkt[knode] = 0;
                return;
            }
            if (rad > 20 || rad < 3) {
                m_pPixelio->m_DBT = 273.17;
            } // 如果辐射值不在合理范围内（3 到 20），将结果设为 273.17 K
            else {
                m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5, rad); // 否则，通过反向 Planck 函数计算温度并设置结果
            }
        } else {
            std::array<float, 4> Ecom = {emis_s_temp, emis_s_temp, emis_v_temp, emis_v_temp};
            std::array<float, 4> Tcom = {tss_temp, tsh_temp, tcs_temp, tch_temp};
            std::array<float, 4> Rcom;
            for (int i = 0; i < 4; ++i) {
                Rcom[i] = SCI::Planck(10.5, Tcom[i]);
            }

            // 计算各方向的比例因子
            float fss, fsh, fcs, fch;
            tirt_direct_canopy(canopy, angle, fss, fsh, fcs, fch); // 计算直接辐射传输因子
            std::array<float, 4> Pcom = {fss, fsh, fcs, fch};
            const size_t number_component = Ecom.size();
            std::array<float, 4> Ecom_direct = {};
            std::array<float, 4> Rcom_direct = {};
            float Ecom_direct_sum = 0;
            float Rcom_direct_sum = 0;
            for (size_t k = 0; k < number_component; ++k) {
                Ecom_direct[k] = Pcom[k] * Ecom[k];
                Rcom_direct[k] = Rcom[k] * Ecom_direct[k];
                Ecom_direct_sum += Ecom_direct[k];
                Rcom_direct_sum += Rcom_direct[k];
            }

            float mss, msh, mcs, mch;
            tirt_scatter(canopy, angle, spectral, mss, msh, mcs, mch); // 计算散射辐射传输因子
            std::array<float, 4> Ecom_scatter = {mss, msh, mcs, mch};
            std::array<float, 4> Rcom_scatter = {};
            float Ecom_scatter_sum = mss + msh + mcs + mch;
            float Rcom_scatter_sum = 0;
            for (size_t k = 0; k < number_component; ++k) {
                Rcom_scatter[k] = Rcom[k] * Ecom_scatter[k];
                Rcom_scatter_sum += Rcom_scatter[k];
            }
            float rad = (Rcom_direct_sum + Rcom_scatter_sum) / (Ecom_direct_sum + Ecom_scatter_sum);
            if (isnan(rad)) {
                m_pPixelio->m_vSkt[knode] = 0;
                return;
            }
            if (rad > 20 || rad < 3) {
                m_pPixelio->m_vSkt[knode] = 273.17;
            } else {
                m_pPixelio->m_vSkt[knode] = SCI::invPlanck(10.5, rad);
            }
        }
    }

}

float hotspot_analytical(Canopy canopy, Angle angle)
{
    float CIv = 1.0;
    float CIs = 1.0;
    float Gv = 0.5;
    float Gs = 0.5;

    float lai = canopy.lai;
    float hspot = canopy.hspot;

    float vza = angle.vza;
    float vaa = angle.vaa;
    float sza = angle.sza;
    float saa = angle.saa;
    float vsa = vaa - saa;

    if(sza > 75){
        return 0;
    }

    float cthetv = cos(vza*RD);
    float cthets = cos((sza*RD));
    float sthets = sin((sza*RD));
    float sthetv = sin((vza*RD));
    float cphivs = cos((vsa*RD));
    float bv = exp(-lai*CIv/cthetv);
    float bs = exp(-lai*CIv/cthets);
    float fc = 1-bv;
    float fs = 1-fc;

//    float index = 0.83 - 0.04* floor(lai/0.5);
    float index = 0.65;
    float kv = Gv/cthetv;
    float ks = Gs/cthets;
    //float fss,fsh,fcs,fch;
    float correction = 2.0/(kv+ks);
    float H = 1.0;
    float d = H*hspot/correction;
    float delta = 1.0 * sqrt(pow(cthets, -2) + pow(cthetv, -2) -
                             2 * (cthets * cthetv + sthets * sthetv * cphivs) / (cthets * cthetv));
    float bv_upper = 1-(1-bv)*index;
    float bs_upper = 1-(1-bs)*index;
    float hv_upper = -log(bv_upper) * cthetv / GG / lai;
    float hs_upper = -log(bs_upper) * cthets / GG / lai;
    float h_upper = sqrt(hv_upper*hs_upper);
    float h_bottom = 1-h_upper;
    float H_upper = H*h_upper;
    float H_bottom = H*h_bottom;
    float lai_upper = lai*h_upper;
    float lai_bottom = lai*h_bottom;
    float w_upper = delta *1.0;
    if(delta < 0.0001){
        w_upper = 1.0;
    }else{
        w_upper = (1.0-exp(-H*delta/d))*d/(H*delta);
    }

    float overlapping = GG * sqrt(1.0 * CIv * CIs / (cthets * cthetv)) * w_upper;
    float sunlit_fraction_background_upper = exp(-(CIv * GG / cthetv + CIs * GG / cthets - overlapping) * lai_upper);

    float w_bottom = delta*1.0;
    if(delta < 0.0001){
        w_bottom = 1.0;
    }else{
        w_bottom = (1.0-exp(-H*delta/d))*d/(H*delta);
    }
    float sunlit_fraction_vegetation_bottom = 1.0 - exp(-Gv*sqrt(1.0*CIv*CIs/(cthetv*cthets))*w_bottom*lai_bottom);
    float bvs_upper = exp(-Gv * lai_upper * CIv / cthetv);

    float gapvs = (1 - bvs_upper + sunlit_fraction_vegetation_bottom * sunlit_fraction_background_upper);



    return gapvs;
}

float hotspot_analytical_canopy(float lai, float hspot, float vza, float sza, float raa, float CIs, float CIv)
{
    float Gv = 0.5;
    float Gs = 0.5;

    if(sza > 75){
        return 0;
    }

    float cthetv = cos(vza*RD);
    float cthets = cos((sza*RD));
    float sthets = sin((sza*RD));
    float sthetv = sin((vza*RD));
    float cphivs = cos((raa*RD));
    float bv = exp(-lai*CIv/cthetv);
    float bs = exp(-lai*CIv/cthets);
    float fc = 1-bv;
    float fs = 1-fc;

//    float index = 0.83 - 0.04* floor(lai/0.5);
    float index = 0.65;
    float kv = Gv/cthetv;
    float ks = Gs/cthets;
    //float fss,fsh,fcs,fch;
    float correction = 2.0/(kv+ks);
    float H = 1.0;
    float d = H*hspot/correction;
    float delta = 1.0 * sqrt(pow(cthets, -2) + pow(cthetv, -2) -
                             2 * (cthets * cthetv + sthets * sthetv * cphivs) / (cthets * cthetv));
    float bv_upper = 1-(1-bv)*index;
    float bs_upper = 1-(1-bs)*index;
    float hv_upper = -log(bv_upper) * cthetv / GG / lai;
    float hs_upper = -log(bs_upper) * cthets / GG / lai;
    float h_upper = sqrt(hv_upper*hs_upper);
    float h_bottom = 1-h_upper;
    float H_upper = H*h_upper;
    float H_bottom = H*h_bottom;
    float lai_upper = lai*h_upper;
    float lai_bottom = lai*h_bottom;
    float w_upper = delta *1.0;
    if(delta < 0.0001){
        w_upper = 1.0;
    }else{
        w_upper = (1.0-exp(-H*delta/d))*d/(H*delta);
    }

    float overlapping = GG * sqrt(1.0 * CIv * CIs / (cthets * cthetv)) * w_upper;
    float sunlit_fraction_background_upper = exp(-(CIv * GG / cthetv + CIs * GG / cthets - overlapping) * lai_upper);

    float w_bottom = delta*1.0;
    if(delta < 0.0001){
        w_bottom = 1.0;
    }else{
        w_bottom = (1.0-exp(-H*delta/d))*d/(H*delta);
    }
    float sunlit_fraction_vegetation_bottom = 1.0 - exp(-Gv*sqrt(1.0*CIv*CIs/(cthetv*cthets))*w_bottom*lai_bottom);
    float bvs_upper = exp(-Gv * lai_upper * CIv / cthetv);

    float gapvs = (1 - bvs_upper + sunlit_fraction_vegetation_bottom * sunlit_fraction_background_upper);

    return gapvs;
}

float hotspot_volume(float lai,float sza)
{
    float sthets = cos(sza *RD);
    if(sza > 75)
    {
        return 0;
    }

    float gap_probability_illuminate = exp(-GG * lai * CI / sthets);
    float sunlit_fraction_volume = (1-gap_probability_illuminate) / (GG * lai * CI) * sthets;
    return sunlit_fraction_volume;
}

float hotspot_layer(Canopy canopy,Angle angle)
{
    float CIv = 1.0;
    float CIs = 1.0;
    float Gv = 0.5;
    float Gs = 0.5;

    float lai = canopy.lai;
    float hspot = canopy.hspot;

    float vza = angle.vza;
    float vaa = angle.vaa;
    float sza = angle.sza;
    float saa = angle.saa;
    float vsa = vaa - saa;
    if(sza > 75) return 0;


    float cthetv = cos(vza*RD);
    float cthets = cos((sza*RD));
    float sthets = sin((sza*RD));
    float sthetv = sin((vza*RD));
    float cphivs = cos((vsa*RD));
    float bv = exp(-lai*CIv/cthetv);
    float bs = exp(-lai*CIv/cthets);
    float fc = 1-bv;
    float fs = 1-fc;

    float index = 0.83 - 0.04* floor(lai/0.5);
    float kv = Gv/cthetv;
    float ks = Gs/cthets;
    //float fss,fsh,fcs,fch;
    float correction = 2.0/(kv+ks);
    float H = 1.0;
    float d = H*hspot/correction;
    float delta = 1.0 * sqrt(pow(cthets, -2) + pow(cthetv, -2) -
                             2 * (cthets * cthetv + sthets * sthetv * cphivs) / (cthets * cthetv));
    float w=1.0;
    if(delta > 0) {
        float alpha = H * sqrt(delta) / d;
        float w = (1 - exp(-alpha)) / alpha;
    }
    float pls = CIs*Gs/cthets;
    float plv = CIv*Gv/cthetv;
    float overlapping = sqrt(plv*pls)*w;
    float gapvs = exp(-(plv+pls-overlapping)*lai);
    return gapvs;

}

float hotspot_layer_canopy(float lai, float hspot, float vza, float sza, float raa, float CIs, float CIv)
{
    float Gv = 0.5;
    float Gs = 0.5;

    if(sza > 75) return 0;

    float cthetv = cos(vza*RD);
    float cthets = cos((sza*RD));
    float sthets = sin((sza*RD));
    float sthetv = sin((vza*RD));
    float cphivs = cos((raa*RD));
    float bv = exp(-lai*CIv/cthetv);
    float bs = exp(-lai*CIv/cthets);
    float fc = 1-bv;
    float fs = 1-fc;

    float index = 0.83 - 0.04* floor(lai/0.5);
    float kv = Gv/cthetv;
    float ks = Gs/cthets;
    //float fss,fsh,fcs,fch;
    float correction = 2.0/(kv+ks);
    float H = 1.0;
    float d = H*hspot/correction;
    float delta = 1.0 * sqrt(pow(cthets, -2) + pow(cthetv, -2) -
                             2 * (cthets * cthetv + sthets * sthetv * cphivs) / (cthets * cthetv));
    float w=1.0;
    if(delta > 0) {
        float alpha = H * sqrt(delta) / d;
        float w = (1 - exp(-alpha)) / alpha;
    }
    float pls = CIs*Gs/cthets;
    float plv = CIv*Gv/cthetv;
    float overlapping = sqrt(plv*pls)*w;
    float gapvs = exp(-(plv+pls-overlapping)*lai);
    return gapvs;

}

float gap_probability_hom_analytical(float lai, float vza)
{

    float cthetx = cos(vza*RD);
    return exp(-lai * GG * CI / cthetx);
}

float gap_probability_hom_hemisphere_analytical(float lai)
{
    float coeff = 0.825;
    return exp(-coeff * lai * GG);
}

float gap_probability_crown_analytical(float lai,  float std, float radi_horizontal, float radi_vertical,  float xza)
{
    // 计算tg(thx)和cos(thex)
    float tgthx = std::tan(xza*RD);
    float cthetx = std::cos(xza*RD);

    float areav = std::sqrt(radi_vertical * radi_vertical + std::pow(radi_horizontal * tgthx, 2)) * M_PI * radi_vertical;
    float bv_in = std::exp(-lai / (areav * std) * GG / cthetx);
    float bv = std::exp(-areav * (1.0 - bv_in) * std);
    return bv;
}

void RT::tirt_direct(Canopy canopy, Angle angle, float &fss,float &fsh,float &fcs,float &fch)
{
    float lai = canopy.lai;
    float vza = angle.vza;
    float Psoil = gap_probability_hom_analytical(lai,vza);  // 土壤间隙率
    float Pleaf = 1-Psoil;  // 叶片概率
    float Psoil_sunlit = hotspot_layer(canopy,angle);  // 光照土壤概率
    float Pleaf_sunlit = hotspot_analytical(canopy,angle); // 光照射叶片概率
     fcs = Pleaf_sunlit;
     fss = Psoil_sunlit;
    float fc = Pleaf;
    float fs = Psoil;
    fch = fc-fcs;
    fsh = fs-fss;

}

void RT::tirt_scatter(Canopy canopy, Angle angle,Spectral spectral, float &mss, float &msh, float &mcs, float &mch) {

    float lai = canopy.lai;
    float vza = angle.vza;
    float sza = angle.sza;
    float refl_soil = spectral.soilRefl_ir;
    float refl_leaf = spectral.leafRefl_ir;

    float bv = gap_probability_hom_analytical(lai, vza);
    float M = gap_probability_hom_hemisphere_analytical(lai);
    float alpha[10] = {0.2855375, 0.2885375, 0.2964427, 0.3003953, 0.3083004, 0.3201581, 0.3399209, 0.3715415,
                       0.4189723, 1};
    int vza_index_bottom = floor(vza / 10);
    int vza_index_top = vza_index_bottom + 1;
    if (vza_index_top > 9) vza_index_top = 9;

    float ratio = (vza - 10 * vza_index_bottom) / 10.0;
    float alphanew = alpha[vza_index_bottom] * (1 - ratio) + alpha[vza_index_top] * ratio;
    float mc = (1 - refl_leaf) * (bv * (1 - M) * refl_soil + (1 - alphanew) * (1 - bv * M) * (1 - bv) * refl_leaf);
    float ms = 0;
    mss = 0;
    msh = 0;
    float Vsunlit = hotspot_volume(lai, sza);
    float Vshaded = 1 - Vsunlit;
    mcs = bv * (1 - M) * refl_soil * Vsunlit +
          (1 - alpha[vza_index_bottom]) * (1 - bv * M) * (1 - bv) * refl_leaf * Vsunlit;
    mcs = mcs * (1 - refl_leaf);
    mch = bv * (1 - M) * refl_soil * Vshaded +
          (1 - alpha[vza_index_bottom]) * (1 - bv * M) * (1 - bv) * refl_leaf * Vshaded;
    mch = mch * (1 - refl_leaf);

}

double calculate_projection(const std::vector<double>& angles, const std::vector<double>& shape, double alpha, double height_diff, double rd) {
    double length = shape[0];
    double width = shape[1];
    double vza = angles[0];
    double vaa = angles[1];
    double laa = angles[2];
    double waa = angles[3];
    double tan_vza = std::tan(vza * rd);
    return alpha * (height_diff * length * tan_vza * std::abs(std::cos((vaa - laa) * rd)) +
                    height_diff * width * tan_vza * std::abs(std::cos((vaa - waa) * rd)));
}

double calculate_overlap(double tantv, double tants, double up) {
    return std::sqrt(tantv * tantv + tants * tants - 2 * tantv * tants * up) / (tantv + tants);
}

void RT::calculate_direct_emissivity(Canopy canopy, Angle angle, Building building, float &fss, float &fsh, float &frs, float &frh, float &fws, float &fwh, int ifP) {
    double vza = angle.vza;
    double sza = angle.sza;
    double vaa = angle.vaa;
    double saa = angle.saa;
    double raa = vaa - saa;

    const double rd = M_PI / 180.0;
    double tantv = std::tan(vza * rd);
    double tants = std::tan(sza * rd);
    double up = std::cos(raa * rd);

    double laa = 0;
    double waa = laa + 90;
    double proof = 0;
    double projv = 0, projs = 0;

    // 计算街道的视场比例
    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        double height_diff = height;

        // 屋顶投影
        double proj_roof = alpha * length * width;
        proof += proj_roof;

        // 墙壁投影
        std::vector<double> angles = {vza, vaa, laa, waa};
        projv += calculate_projection(angles, {length, width}, alpha, height_diff, rd);

        angles[0] = sza;
        angles[1] = saa;
        projs += calculate_projection(angles, {length, width}, alpha, height_diff, rd);
    }

    double Overlapping = calculate_overlap(tantv, tants, up);
    double projvs = projv + projs * Overlapping;

    // 计算街道的可视比例
    fss = std::exp(-projv) * (1 - proof); // 街道阳光照射部分
    double fsh_temp = std::exp(-projvs) * (1 - proof); // 街道阴影部分
    fsh = fss - fsh_temp;

    // 计算屋顶的可视比例
    double pRoofV = 0, pRoofS = 0, pRoof_sunlit = 0;
    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        projv = 0;
        projs = 0;

        for (const auto& other_shape : building.shapes) {
            double other_length = other_shape[0];
            double other_width = other_shape[1];
            double other_height = other_shape[2];
            double other_alpha = other_shape[3];
            double height2m1 = other_height - height;
            if (height2m1 <= 0) continue;

            projv += calculate_projection({vza, vaa, laa, waa}, {other_length, other_width}, other_alpha, height2m1, rd);
            projs += calculate_projection({sza, saa, laa, waa}, {other_length, other_width}, other_alpha, height2m1, rd);
        }

        projvs = projv + projs * Overlapping;
        double gapv_roof = std::exp(-projv);
        double gaps_roof = std::exp(-projs);
        double gapvs_roof = std::exp(-projvs);
        pRoofV += alpha * gapv_roof * length * width;
        pRoofS += alpha * gaps_roof * length * width;
        pRoof_sunlit += alpha * gapvs_roof * length * width;
    }

    frs = pRoof_sunlit;
    frh = pRoofV - pRoof_sunlit;

    // 计算墙壁的可视比例
    double pWall_sunlit_fraction = 0;
    double pWall_sunlit_fraction_Weight = 0;
    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        double dheight = height / building.n_part;

        double projv = 0, projs = 0;

        for (int kh = 0; kh < building.n_part; ++kh) {
            double height_temp = dheight * (kh + 0.5);

            // 观测方向投影
            for (const auto& shape_proj : building.shapes) {
                double proj_height = shape_proj[2] - height_temp;
                if (proj_height < 0) continue;
                double proj = proj_height * shape_proj[3] * tantv;
                projv += proj * shape_proj[0] * std::abs(std::cos((vaa - laa) * rd)) +
                         proj * shape_proj[1] * std::abs(std::cos((vaa - waa) * rd));

                // 太阳方向投影
                proj = proj_height * shape_proj[3] * tants;
                projs += proj * shape_proj[0] * std::abs(std::cos((saa - laa) * rd)) +
                         proj * shape_proj[1] * std::abs(std::cos((saa - waa) * rd));
            }
        }
        projv /= building.n_part;
        projs /= building.n_part;
        projvs = projv + projs * Overlapping;
        double gapv_wall = std::exp(-projv);
        double gapvs_wall = std::exp(-projvs);

        bool lsunlit = (std::abs(laa - saa) < 90 || std::abs(laa - saa) > 270) &&
                       (std::abs(laa - vaa) < 90 || std::abs(laa - vaa) > 270);
        bool wsunlit = (std::abs(waa - saa) < 90 || std::abs(waa - saa) > 270) &&
                       (std::abs(waa - vaa) < 90 || std::abs(waa - vaa) > 270);

        pWall_sunlit_fraction += alpha * gapvs_wall * (
                (height * length * tantv * std::abs(std::cos((vaa - laa) * rd))) * lsunlit +
                (height * width * tantv * std::abs(std::cos((vaa - waa) * rd)) * wsunlit)
        );

        pWall_sunlit_fraction_Weight += alpha * gapv_wall * (
                (height * length * tantv * std::abs(std::cos((vaa - laa) * rd))) +
                (height * width * tantv * std::abs(std::cos((vaa - waa) * rd)))
        );
    }

    if (pWall_sunlit_fraction_Weight != 0) {
        pWall_sunlit_fraction /= pWall_sunlit_fraction_Weight;
    } else {
        pWall_sunlit_fraction = 0;
    }

    double pWallV = 1 - fss - pRoofV;
    fws = pWallV * pWall_sunlit_fraction;
    fwh = pWallV - fws;

//    // 输出结果
//    if (ifP == 1) {
//        std::cout << "Wall: " << fws * building.Ewall << ", Street: " << fss * building.Estreat << ", Roof: " << frs * building.Eroof << std::endl;
//    } else if (ifP == 2) {
//        std::cout << "Wall Sunlit: " << fws * building.Ewall << ", Wall Shaded: " << fwh * building.Ewall << std::endl;
//        std::cout << "Street Sunlit: " << fss * building.Estreat << ", Street Shaded: " << fsh * building.Estreat << std::endl;
//        std::cout << "Roof Sunlit: " << frs * building.Eroof << ", Roof Shaded: " << frh * building.Eroof << std::endl;
//    } else if (ifP == 3) {
//        std::cout << fws << ", " << fwh << ", " << fss << ", " << fsh << ", " << frs << ", " << frh << std::endl;
//    } else {
//        std::cout << fws * building.Ewall + fss * building.Estreat + frs * building.Eroof << std::endl;
//    }

}


void RT::calculate_scattering_emissivity(Canopy canopy, Angle angle, Building building, float &mss, float &msh, float &mrs, float &mrh, float &mws, float &mwh, int ifP) {
    double vza = angle.vza;
    double sza = angle.sza;
    double vaa = angle.vaa;
    double saa = angle.saa;
    double raa = vaa - saa;

    if (raa > 180) {
        raa = 360 - raa;
    }

    const double rd = M_PI / 180.0;
    double ui = std::cos(sza * rd);
    double uv = std::cos(vza * rd);
    double si = std::sin(sza * rd);
    double sv = std::sin(vza * rd);
    double up = std::cos(raa * rd);
    double tantv = std::tan(vza * rd);
    double tants = std::tan(sza * rd);
    double laa = 0;   // the relative azimuth angle of length
    double waa = laa + 90;  // the relative azimuth angle of width

    // Street calculation
    double height1r = 0;
    double projv = 0;
    double projs = 0;
    double projvs = 0;

    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        double height2m1 = height - height1r;
        if (height2m1 <= 0) continue;

        double proj_roof = alpha * length * width;
        double projv_wall = alpha * (height2m1 * length * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                                     height2m1 * width * tantv * std::abs(std::cos((vaa - waa) * rd)));
        projv += projv_wall + proj_roof;

        double projs_wall = alpha * (height2m1 * length * tants * std::abs(std::cos((saa - laa) * rd)) +
                                     height2m1 * width * tants * std::abs(std::cos((saa - waa) * rd)));
        projs += projs_wall + proj_roof;
    }

    double Overlapping = std::sqrt(tantv * tantv + tants * tants - 2 * tantv * tants * up) / (tantv + tants);
    projvs = projv + projs * Overlapping;

    double pStreatV = std::exp(-projv);
    double pStreatS = std::exp(-projs);
    double pStreatV_sunlit = std::exp(-projvs);
    double pStreatV_shaded = pStreatV - pStreatV_sunlit;

    // Wall calculation
    double if2 = 1.0;
    int n_hza0 = 90;
    int n_haa0 = 60;
    std::vector<double> hza0(n_hza0), haa0(n_haa0);
    for (int i = 0; i < n_hza0; ++i) hza0[i] = i;
    for (int i = 0; i < n_haa0; ++i) haa0[i] = i * 360.0 / n_haa0;

    std::vector<double> hza, haa;
    for (double h : hza0) {
        for (double a : haa0) {
            hza.push_back(h);
            haa.push_back(a);
        }
    }

    double dangle = M_PI / 2.0 / n_hza0;
    std::vector<double> tantemp(hza.size()), fweiplus(hza.size());
    for (size_t i = 0; i < hza.size(); ++i) {
        tantemp[i] = std::tan(hza[i] * rd);
        fweiplus[i] = std::sin(hza[i] * rd) * dangle * std::cos(hza[i] * rd);
    }

    double fweiplusSum = if2 * std::accumulate(fweiplus.begin(), fweiplus.end(), 0.0);

    double tempSv = 0.0;
    for (const auto& shape : building.shapes) {
        tempSv += shape[3] * shape[2] * shape[0] * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                  shape[3] * shape[2] * shape[1] * tantv * std::abs(std::cos((vaa - waa) * rd));
    }

    double proj_rooff = 0.0;
    for (const auto& shape : building.shapes) {
        proj_rooff += shape[1] * shape[0] * shape[3];
    }

    double f = std::exp(-(tempSv + proj_rooff));
    double ff = std::exp(-proj_rooff);
    double f0 = std::exp(-tempSv);
    double fw = ff - f;
    double i0v = ff - f;
    double fs = f;
    double fr = 1 - ff;

    double eu = 0.0;
    double ed = 0.0;

    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        double dheight = height / building.n_part;

        for (int kh = 0; kh < building.n_part; ++kh) {
            double heighttemp = dheight * (kh + 0.5);
            double tempdS = (length * dheight * alpha) * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                            (width * dheight * alpha) * tantv * std::abs(std::cos((vaa - waa) * rd));
            double ftempv = 1.0;

            std::vector<double> projStemp(hza.size(), 0.0);
            double proj_roof = 0.0;
            for (size_t i = 0; i < building.shapes.size(); ++i) {
                if (building.shapes[i][2] - heighttemp > 0) {
                    proj_roof += building.shapes[i][1] * building.shapes[i][0] * building.shapes[i][3];
                    for (size_t j = 0; j < hza.size(); ++j) {
                        projStemp[j] += building.shapes[i][3] * (building.shapes[i][2] - heighttemp) * building.shapes[i][0] * tantemp[j] * std::abs(std::cos((haa[j] - laa) * rd)) +
                                        building.shapes[i][3] * (building.shapes[i][2] - heighttemp) * building.shapes[i][1] * tantemp[j] * std::abs(std::cos((haa[j] - waa) * rd));
                    }
                }
            }

            double ftemp = 0.0;
            for (size_t j = 0; j < hza.size(); ++j) {
                ftemp += std::exp(-projStemp[j]);
            }
            ftemp /= hza.size();

            tempSv = 0.0;
            for (const auto& shape : building.shapes) {
                if (shape[2] - heighttemp > 0) {
                    tempSv += shape[3] * (shape[2] - heighttemp) * shape[0] * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                              shape[3] * (shape[2] - heighttemp) * shape[1] * tantv * std::abs(std::cos((vaa - waa) * rd));
                }
            }
            ftempv = std::exp(-tempSv);
            eu += ftemp * tempdS * ftempv * if2 / fweiplusSum;
        }
    }

    eu = eu / i0v * 0.5;
    ed = ed / i0v * 0.5;
    double p = 1 - eu - ed;
    if (i0v == 0) {
        p = 0;
        eu = 0;
        ed = 0;
    }

    double heighttemp = 0;
    double ws = 0;
    double C = 1.05;
    for (const auto& shape : building.shapes) {
        if (shape[2] - heighttemp > 0) {
            double proj_roof = 0.0;
            double projStemp = 0.0;
            for (size_t i = 0; i < building.shapes.size(); ++i) {
                if (building.shapes[i][2] - heighttemp > 0) {
                    proj_roof += building.shapes[i][1] * building.shapes[i][0] * building.shapes[i][3];
                    projStemp += building.shapes[i][3] * (building.shapes[i][2] - heighttemp) * building.shapes[i][0] * tantemp[i] * std::abs(std::cos((haa[i] - laa) * rd)) +
                                 building.shapes[i][3] * (building.shapes[i][2] - heighttemp) * building.shapes[i][1] * tantemp[i] * std::abs(std::cos((haa[i] - waa) * rd));
                }
            }
            double ftemp = std::exp(-projStemp);
            tempSv = 0.0;
            for (const auto& shape : building.shapes) {
                if (shape[2] - heighttemp > 0) {
                    tempSv += shape[3] * (shape[2] - heighttemp) * shape[0] * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                              shape[3] * (shape[2] - heighttemp) * shape[1] * tantv * std::abs(std::cos((vaa - waa) * rd));
                }
            }
            double ftempv = std::exp(-tempSv);
            double tempds = 1 - proj_roof;
            ws = (1 - ftemp) * ftempv * tempds;
        }
    }

    double wr = 0.0;
    for (const auto& shape : building.shapes) {
        double length = shape[0];
        double width = shape[1];
        double height = shape[2];
        double alpha = shape[3];
        double heighttemp = height;
        for (const auto& shape_proj : building.shapes) {
            if (shape_proj[2] - heighttemp > 0) {
                double proj_roof = 0.0;
                double projStemp = 0.0;
                proj_roof += shape_proj[1] * shape_proj[0] * shape_proj[3];
                projStemp += shape_proj[3] * (shape_proj[2] - heighttemp) * shape_proj[0] * tantemp[0] * std::abs(std::cos((haa[0] - laa) * rd)) +
                             shape_proj[3] * (shape_proj[2] - heighttemp) * shape_proj[1] * tantemp[0] * std::abs(std::cos((haa[0] - waa) * rd));
                double ftemp = std::exp(-projStemp);
                double tempdS = length * width * alpha;
                tempSv = 0.0;
                for (const auto& shape_proj : building.shapes) {
                    if (shape_proj[2] - heighttemp > 0) {
                        tempSv += shape_proj[3] * (shape_proj[2] - heighttemp) * shape_proj[0] * tantv * std::abs(std::cos((vaa - laa) * rd)) +
                                  shape_proj[3] * (shape_proj[2] - heighttemp) * shape_proj[1] * tantv * std::abs(std::cos((vaa - waa) * rd));
                    }
                }
                double ftempv = std::exp(-tempSv);
                wr += (1 - ftemp) * tempdS * ftempv;
            }
        }
    }

    double eww = building.Ewall * (1 - building.Ewall) * p * i0v;
    double ews = building.Ewall * (1 - building.Estreat) * ws;
    double esw = building.Estreat * (1 - building.Ewall) * ed * i0v;
    double ewr = building.Ewall * (1 - building.Eroof) * wr;
    double emw = eww + ewr + ews;
    double ems = esw;

    double ew = fw * building.Ewall;
    double es = f * building.Estreat;
    double er = (1 - ff) * building.Eroof;

    double vsraa = (180.0 - std::abs(raa)) / 180.0;
    double vhraa = 1 - vsraa;

    double fts = 0;
    if (sza != 0) {
        double area = 0.0;
        for (const auto& shape : building.shapes) {
            area += (shape[3] * shape[0] * shape[2] * tants * std::abs(std::cos((saa - laa) * rd))) +
                    (shape[3] * shape[1] * shape[2] * tants * std::abs(std::cos((saa - waa) * rd)));
        }
        if (area > 0) {
            fts = (1 - std::exp(-area)) / area;
        }
    }

    emw = ews + eww;
    double emws = emw * fts * vhraa;
    double emwh = emw - emws;
    ems = esw;
    double emss = ems * pStreatS;
    double emsh = ems - emss;

}


void RT::tirt_direct_canopy(Canopy canopy, Angle angle, float &fss,float &fsh,float &fcs,float &fch)
{
    float lai_temp = canopy.lai;
    float std_temp = canopy.treeStand;
    float hspot_temp = canopy.hspot;
    float canopyheight_temp = canopy.canopyHeight;
    float b_temp = canopy.b;
    float radi_vertical_temp = canopyheight_temp / 2.0;
    float radi_horizontal_temp = canopyheight_temp / (2.0 * b_temp);

    float vza_temp = angle.vza;
    float sza_temp = angle.sza;
    float vaa_temp = angle.vaa;
    float saa_temp = angle.saa;
    float raa_temp = vaa_temp - saa_temp;

    float bv = gap_probability_crown_analytical(lai_temp, std_temp, radi_horizontal_temp, radi_vertical_temp, vza_temp);
    float bi = gap_probability_crown_analytical(lai_temp, std_temp, radi_horizontal_temp, radi_vertical_temp, sza_temp);

    float Psoil = bv;
    float Pleaf = 1.0 - Psoil;

    // 计算视角和太阳角的余弦值
    float uv = std::cos(vza_temp * RD);
    float ui = std::cos(sza_temp * RD);

    // 计算CIv和CIi
    float CIv = -std::log(bv) * uv / (lai_temp * GG);
    float CIs = -std::log(bi) * ui / (lai_temp * GG);

    // 计算土壤和叶片的可视比例
    float Psoil_sunlit = hotspot_layer_canopy(lai_temp, hspot_temp, vza_temp, sza_temp, raa_temp, CIs, CIv);
    float Pleaf_sunlit = hotspot_analytical_canopy(lai_temp, hspot_temp, vza_temp, sza_temp, raa_temp, CIs, CIv);
    fcs = Pleaf_sunlit;
    fss = Psoil_sunlit;
    float fc = Pleaf;
    float fs = Psoil;
    fch = fc-fcs;
    fsh = fs-fss;
}

void RT::sample_hom(std::shared_ptr<PixelIO> &m_pPixelio)
{
    int ifradiance = 0;
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
    Angle &angle = m_pPixelio->m_angle;
    Satellite &satellite = m_pPixelio->m_pDynamicVariable->satellite;

    int hh = std::stoi(satellite.time.substr(0, 2)); // 提取前两位作为小时
    int MM = std::stoi(satellite.time.substr(2, 2)); // 提取后两位作为分钟
    float ratio = MM / 60.0f; // 计算插值比例
    float tch_temp = (1 - ratio) * m_pPixelio->m_vTch[hh] + ratio * m_pPixelio->m_vTch[hh + 1];
    float tss_temp = (1 - ratio) * m_pPixelio->m_vTss[hh] + ratio * m_pPixelio->m_vTss[hh + 1];
    float tsh_temp = (1 - ratio) * m_pPixelio->m_vTsh[hh] + ratio * m_pPixelio->m_vTsh[hh + 1];
    float tcs_temp = (1 - ratio) * m_pPixelio->m_vTcs[hh] + ratio * m_pPixelio->m_vTcs[hh + 1];

    float lai_temp = canopy.lai;

    float vza_temp = satellite.vza;
    float sza_temp = satellite.sza;
    float vaa_temp = satellite.vaa;
    float saa_temp = satellite.saa;

    angle.vza = vza_temp;
    angle.sza = sza_temp;
    angle.vaa = vaa_temp;
    angle.saa = saa_temp;

    float emis_s_temp = m_pPixelio->emis_s;
    float emis_v_temp = m_pPixelio->emis_v;
    spectral.soilRefl_ir = 1 - emis_s_temp;
    spectral.leafRefl_ir = 1 - emis_v_temp;

    if(lai_temp<=0)     // 如果叶面积指数（LAI）小于等于0，表示没有植被
    {
        float rad = SCI::Planck(10.5,tss_temp);        // 通过 Planck 函数计算辐射值（波长 10.5 微米）
        rad = rad *(1-emis_s_temp);        // 考虑土壤的反射率，调整辐射值
        if (isnan(rad)) {
            m_pPixelio->m_DBT = 0;
            return;
        }
        if(rad > 20 || rad < 3) {m_pPixelio->m_DBT = 273.17; } // 如果辐射值不在合理范围内（3 到 20），将结果设为 273.17 K
        else{
            m_pPixelio->m_DBT = SCI::invPlanck(10.5,rad);  // 否则，通过反向 Planck 函数计算温度并设置结果
        }
    }else
    {
        std::array<float, 4> Ecom = {emis_s_temp, emis_s_temp, emis_v_temp, emis_v_temp};
        std::array<float, 4> Tcom = {tss_temp, tsh_temp, tcs_temp, tch_temp};
        std::array<float, 4> Rcom;
        for (int i = 0; i < 4; ++i) {
            Rcom[i] = SCI::Planck(10.5,Tcom[i]);
        }

        // 计算各方向的比例因子
        float fss,fsh,fcs,fch;
        tirt_direct(canopy,angle,fss,fsh,fcs,fch);        // 计算直接辐射传输因子
        std::array<float, 4> Pcom = {fss, fsh, fcs, fch};
        const size_t number_component = Ecom.size();
        std::array<float, 4> Ecom_direct = {};
        std::array<float, 4> Rcom_direct = {};
        float Ecom_direct_sum = 0;
        float Rcom_direct_sum = 0;
        for (size_t k = 0; k < number_component; ++k) {
            Ecom_direct[k] = Pcom[k] * Ecom[k];
            Rcom_direct[k] = Rcom[k] * Ecom_direct[k];
            Ecom_direct_sum += Ecom_direct[k];
            Rcom_direct_sum += Rcom_direct[k];
        }

        float mss,msh,mcs,mch;
        tirt_scatter(canopy,angle,spectral,mss,msh,mcs,mch);        // 计算散射辐射传输因子
        std::array<float, 4> Ecom_scatter = {mss,msh,mcs,mch};
        std::array<float, 4> Rcom_scatter = {};
        float Ecom_scatter_sum = mss + msh + mcs + mch;
        float Rcom_scatter_sum = 0;
        for (size_t k = 0; k < number_component; ++k) {
            Rcom_scatter[k] = Rcom[k] * Ecom_scatter[k];
            Rcom_scatter_sum += Rcom_scatter[k];
        }

        // // nadirTirt中计算辐射的公式
        // float Radss = SCI::Planck(10.5,tss_temp);
        // float Radsh = SCI::Planck(10.5,tsh_temp);
        // float Radcs = SCI::Planck(10.5,tcs_temp);
        // float Radch = SCI::Planck(10.5,tch_temp);
        // float rad = (fss * emis_s_temp + mss) * Radss + (fsh * emis_s_temp + msh) * Radsh +(fcs * emis_v_temp + mcs) * Radcs + (fch * emis_v_temp + mch) * Radch;

        float rad = (Rcom_direct_sum + Rcom_scatter_sum) / (Ecom_direct_sum + Ecom_scatter_sum);
        if (isnan(rad)) {
            m_pPixelio->m_DBT = 0;
            return;
        }
        if(rad > 20 || rad < 3) {
            m_pPixelio->m_DBT = 273.17;
        }
        else{
            m_pPixelio->m_DBT = SCI::invPlanck(10.5,rad);
        }
    }
}

void RT::sample_canopy(std::shared_ptr<PixelIO> &m_pPixelio)
{
    int ifradiance = 0;
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
    Angle &angle = m_pPixelio->m_angle;
    Satellite &satellite = m_pPixelio->m_pDynamicVariable->satellite;

    int hh = std::stoi(satellite.time.substr(0, 2)); // 提取前两位作为小时
    int MM = std::stoi(satellite.time.substr(2, 2)); // 提取后两位作为分钟
    float ratio = MM / 60.0f; // 计算插值比例
    float tch_temp = (1 - ratio) * m_pPixelio->m_vTch[hh] + ratio * m_pPixelio->m_vTch[hh + 1];
    float tss_temp = (1 - ratio) * m_pPixelio->m_vTss[hh] + ratio * m_pPixelio->m_vTss[hh + 1];
    float tsh_temp = (1 - ratio) * m_pPixelio->m_vTsh[hh] + ratio * m_pPixelio->m_vTsh[hh + 1];
    float tcs_temp = (1 - ratio) * m_pPixelio->m_vTcs[hh] + ratio * m_pPixelio->m_vTcs[hh + 1];

    float treestand_temp = canopy.treeStand;
    float canopyheight_temp = canopy.canopyHeight;
    float lai_temp = canopy.lai;
    float b_temp = canopy.b;
    float radi_vertical_temp = canopyheight_temp / 2.0;
    float radi_horizontal_temp = canopyheight_temp / (2.0 * b_temp);

    float vza_temp = satellite.vza;
    float sza_temp = satellite.sza;
    float vaa_temp = satellite.vaa;
    float saa_temp = satellite.saa;
    float raa_temp = std::abs(vaa_temp - saa_temp);

    angle.vza = vza_temp;
    angle.sza = sza_temp;
    angle.vaa = vaa_temp;
    angle.saa = saa_temp;

    float emis_s_temp = m_pPixelio->emis_s;
    float emis_v_temp = m_pPixelio->emis_v;
    spectral.soilRefl_ir = 1 - emis_s_temp;
    spectral.leafRefl_ir = 1 - emis_v_temp;

    if(lai_temp<=0)     // 如果叶面积指数（LAI）小于等于0，表示没有植被
    {
        float rad = SCI::Planck(10.5,tss_temp);        // 通过 Planck 函数计算辐射值（波长 10.5 微米）
        rad = rad *(1-emis_s_temp);        // 考虑土壤的反射率，调整辐射值
        if (isnan(rad)) {
            m_pPixelio->m_DBT = 0;
            return;
        }
        if(rad > 20 || rad < 3) {m_pPixelio->m_DBT = 273.17; } // 如果辐射值不在合理范围内（3 到 20），将结果设为 273.17 K
        else{
            m_pPixelio->m_DBT = SCI::invPlanck(10.5,rad);  // 否则，通过反向 Planck 函数计算温度并设置结果
        }
    }else
    {
        std::array<float, 4> Ecom = {emis_s_temp, emis_s_temp, emis_v_temp, emis_v_temp};
        std::array<float, 4> Tcom = {tss_temp, tsh_temp, tcs_temp, tch_temp};
        std::array<float, 4> Rcom;
        for (int i = 0; i < 4; ++i) {
            Rcom[i] = SCI::Planck(10.5,Tcom[i]);
        }

        // 计算各方向的比例因子
        float fss,fsh,fcs,fch;
        tirt_direct_canopy(canopy,angle,fss,fsh,fcs,fch);        // 计算直接辐射传输因子
        std::array<float, 4> Pcom = {fss, fsh, fcs, fch};
        const size_t number_component = Ecom.size();
        std::array<float, 4> Ecom_direct = {};
        std::array<float, 4> Rcom_direct = {};
        float Ecom_direct_sum = 0;
        float Rcom_direct_sum = 0;
        for (size_t k = 0; k < number_component; ++k) {
            Ecom_direct[k] = Pcom[k] * Ecom[k];
            Rcom_direct[k] = Rcom[k] * Ecom_direct[k];
            Ecom_direct_sum += Ecom_direct[k];
            Rcom_direct_sum += Rcom_direct[k];
        }

        float mss,msh,mcs,mch;
        tirt_scatter(canopy,angle,spectral,mss,msh,mcs,mch);        // 计算散射辐射传输因子
        std::array<float, 4> Ecom_scatter = {mss,msh,mcs,mch};
        std::array<float, 4> Rcom_scatter = {};
        float Ecom_scatter_sum = mss + msh + mcs + mch;
        float Rcom_scatter_sum = 0;
        for (size_t k = 0; k < number_component; ++k) {
            Rcom_scatter[k] = Rcom[k] * Ecom_scatter[k];
            Rcom_scatter_sum += Rcom_scatter[k];
        }

        // // nadirTirt中计算辐射的公式
        // float Radss = SCI::Planck(10.5,tss_temp);
        // float Radsh = SCI::Planck(10.5,tsh_temp);
        // float Radcs = SCI::Planck(10.5,tcs_temp);
        // float Radch = SCI::Planck(10.5,tch_temp);
        // float rad = (fss * emis_s_temp + mss) * Radss + (fsh * emis_s_temp + msh) * Radsh +(fcs * emis_v_temp + mcs) * Radcs + (fch * emis_v_temp + mch) * Radch;

        float rad = (Rcom_direct_sum + Rcom_scatter_sum) / (Ecom_direct_sum + Ecom_scatter_sum);
        if (isnan(rad)) {
            m_pPixelio->m_DBT = 0;
            return;
        }
        if(rad > 20 || rad < 3) {
            m_pPixelio->m_DBT = 273.17;
        }
        else{
            m_pPixelio->m_DBT = SCI::invPlanck(10.5,rad);
        }
    }
}

void RT::sample_urban(std::shared_ptr<PixelIO> &m_pPixelio)
{
    int ifradiance = 0;
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
    Angle &angle = m_pPixelio->m_angle;
    Satellite &satellite = m_pPixelio->m_pDynamicVariable->satellite;
    Building building;

    building.Eroof = 0.950;
    building.Estreat = 0.955;
    building.Ewall = 0.920;
    building.shapes =  {{20, 10, 10, 0.003 * 0.5},
                        {20, 10, 50, 0.003 * 0.5}};
    building.n_part = 20;

    int hh = std::stoi(satellite.time.substr(0, 2)); // 提取前两位作为小时
    int MM = std::stoi(satellite.time.substr(2, 2)); // 提取后两位作为分钟
    float ratio = MM / 60.0f; // 计算插值比例
    float trh_temp = (1 - ratio) * m_pPixelio->m_vTrh[hh] + ratio * m_pPixelio->m_vTrh[hh + 1];
    float trs_temp = (1 - ratio) * m_pPixelio->m_vTrs[hh] + ratio * m_pPixelio->m_vTrs[hh + 1];
    float twh_temp = (1 - ratio) * m_pPixelio->m_vTwh[hh] + ratio * m_pPixelio->m_vTwh[hh + 1];
    float tws_temp = (1 - ratio) * m_pPixelio->m_vTws[hh] + ratio * m_pPixelio->m_vTws[hh + 1];
    float tth_temp = (1 - ratio) * m_pPixelio->m_vTsh[hh] + ratio * m_pPixelio->m_vTth[hh + 1];
    float tts_temp = (1 - ratio) * m_pPixelio->m_vTts[hh] + ratio * m_pPixelio->m_vTts[hh + 1];
//    float trs_temp = (1 - ratio) * m_pPixelio->m_vTch[hh] + ratio * m_pPixelio->m_vTch[hh + 1];

    float vza_temp = satellite.vza;
    float sza_temp = satellite.sza;
    float vaa_temp = satellite.vaa;
    float saa_temp = satellite.saa;
    float raa_temp = std::abs(vaa_temp - saa_temp);

    angle.vza = vza_temp;
    angle.sza = sza_temp;
    angle.vaa = vaa_temp;
    angle.saa = saa_temp;

    float fss, fsh, frs, frh, fws, fwh;
    int ifP = 0;
    calculate_direct_emissivity(canopy, angle, building, fss, fsh, frs, frh, fws, fwh, ifP);

    std::array<float, 6> Ecom = {building.Estreat, building.Estreat, building.Eroof, building.Eroof, building.Ewall, building.Ewall};
    std::array<float, 6> Tcom = {tts_temp, tth_temp, trs_temp, trh_temp, tws_temp, twh_temp};
    std::array<float, 6> Rcom;
    for (int i = 0; i < Tcom.size(); ++i) {
        Rcom[i] = SCI::Planck(10.5,Tcom[i]);
    }
    std::array<float, 6> Pcom = {fss, fsh, frs, frh, fws, fwh};
    const size_t number_component = Ecom.size();
    std::array<float, 6> Ecom_direct = {};
    std::array<float, 6> Rcom_direct = {};
    float Ecom_direct_sum = 0;
    float Rcom_direct_sum = 0;
    for (size_t k = 0; k < number_component; ++k) {
        Ecom_direct[k] = Pcom[k] * Ecom[k];
        Rcom_direct[k] = Rcom[k] * Ecom_direct[k];
        Ecom_direct_sum += Ecom_direct[k];
        Rcom_direct_sum += Rcom_direct[k];
    }

    float mss, msh, mrs, mrh, mws, mwh;
    calculate_scattering_emissivity(canopy, angle, building, mss, msh, mrs, mrh, mws, mwh, ifP);        // 计算散射辐射传输因子
    std::array<float, 6> Ecom_scatter = {mss, msh, mrs, mrh, mws, mwh};
    std::array<float, 6> Rcom_scatter = {};
    float Ecom_scatter_sum = mss + msh + mrs + mrh+ mws + mwh;
    float Rcom_scatter_sum = 0;
    for (size_t k = 0; k < number_component; ++k) {
        Rcom_scatter[k] = Rcom[k] * Ecom_scatter[k];
        Rcom_scatter_sum += Rcom_scatter[k];
    }

    float rad = (Rcom_direct_sum + Rcom_scatter_sum) / (Ecom_direct_sum + Ecom_scatter_sum);
    if (isnan(rad)) {
        m_pPixelio->m_DBT = 0;
        return;
    }
    if(rad > 20 || rad < 3) {
        m_pPixelio->m_DBT = 273.17;
    }
    else{
        m_pPixelio->m_DBT = SCI::invPlanck(10.5,rad);
    }
}


void RT::satTirt(std::shared_ptr<PixelIO> &m_pPixelio)
{
    // 获取 PixelIO 中的各种数据
    Canopy &canopy = m_pPixelio->m_pInputset->canopy;
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal;
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;
    Satellite &satellite = m_pPixelio->m_pDynamicVariable->satellite;

    int type_temp = canopy.type;

    // 定义要检查的类型集合
    std::vector<int> hom_types = {8, 9, 10, 12, 14, 15, 16};
    std::vector<int> canopy_types = {1, 2, 3, 4, 5, 6, 7};
    std::vector<int> urban_types = {13};

    // 判断条件
    bool ind_hom = std::find(hom_types.begin(), hom_types.end(), type_temp) != hom_types.end();
    bool ind_crown = std::find(canopy_types.begin(), canopy_types.end(), type_temp) != canopy_types.end();
    bool ind_urban = std::find(urban_types.begin(), urban_types.end(), type_temp) != urban_types.end();

    if (ind_hom) {
        sample_hom(m_pPixelio);
    }
    else if (ind_crown) {
        sample_canopy(m_pPixelio);
    }
    else if (ind_urban) {
        sample_urban(m_pPixelio);
    }
    else    {
        m_pPixelio->m_DBT = 0;
    }

}


typedef struct {
    float length;
    float width;
    float height;
    float alpha;
} Shape;

void calculate_component_fraction(float length, float width, float height, float density, float sza, float saa,
                                  float* frs, float* frh, float* fws, float* fwh, float* fss, float* fsh)
{
    Shape shapes[1] = {
        {length, width, height, density}
    };
    int n_shape = sizeof(shapes) / sizeof(shapes[0]);
    int n_part = 100;

    // Constants
    double rd = M_PI / 180.0;
    double total_pRoof_sunlit = 0, total_pRoof_shaded = 0;
    double total_pWallV_sunlit = 0, total_pWallV_shaded = 0;
    double total_pStreetV_sunlit = 0, total_pStreetV_shaded = 0;
    int n_vza = 0;

    for (double vza = 0; vza <= 1; vza += 5)
    {
        double abs_vza = fabs(vza);
        double vaa = (n_vza > 11) ? 0 : 180;
        double raa = vaa - saa;

        // Calculate angles in radians
        double ui = cos(sza * rd);
        double uv = cos(abs_vza * rd);
        double si = sin(sza * rd);
        double sv = sin(abs_vza * rd);
        double up = cos(raa * rd);
        double tantv = tan(abs_vza * rd);
        double tants = tan(sza * rd);

        double projv = 0, projs = 0, projvs = 0;
        double laa = 0, waa = laa + 90;

        for (int kshape2 = 0; kshape2 < n_shape; kshape2++)
        {
            Shape shape2 = shapes[kshape2];
            double length2 = shape2.length;
            double width2 = shape2.width;
            double height2 = shape2.height;
            double alpha2 = shape2.alpha;

            double height2m1 = height2 - 0;
            if (height2m1 <= 0) continue;

            // Calculate projections
            double proj_roof = alpha2 * length2 * width2;
            double projv_wall = alpha2 * (height2m1 * length2 * tantv * fabs(cos((vaa - laa) * rd)) +
                                          height2m1 * width2 * tantv * fabs(cos((vaa - waa) * rd)));
            projv += projv_wall + proj_roof;

            double projs_wall = alpha2 * (height2m1 * length2 * tants * fabs(cos((saa - laa) * rd)) +
                                          height2m1 * width2 * tants * fabs(cos((saa - waa) * rd)));
            projs += projs_wall + proj_roof;
        }

        // Calculate overlapping factor
        double Overlapping = sqrt(tantv * tantv + tants * tants - 2 * tantv * tants * up) / (tantv + tants);
        projvs = projv + projs * Overlapping;

        // Calculate street visibility (StreetV)
        double pStreetV = exp(-projv);
        double pStreetS = exp(-projs);
        double pStreetV_sunlit = exp(-projvs);
        double pStreetV_shaded = pStreetV - pStreetV_sunlit;
        double pStreetV_sunlit_fraction = pStreetV_sunlit / pStreetV;

        // Calculate roof visibility
        double pRoofV = 0, pRoofS = 0, pRoof_sunlit = 0;
        for (int kshape1 = 0; kshape1 < n_shape; kshape1++)
        {
            Shape shape1 = shapes[kshape1];
            double length1 = shape1.length;
            double width1 = shape1.width;
            double height1 = shape1.height;
            double alpha1 = shape1.alpha;

            projv = 0;
            projs = 0;

            for (int kshape2 = 0; kshape2 < n_shape; kshape2++)
            {
                Shape shape2 = shapes[kshape2];
                double length2 = shape2.length;
                double width2 = shape2.width;
                double height2 = shape2.height;
                double alpha2 = shape2.alpha;

                double height2m1 = height2 - height1;
                if (height2m1 <= 0) continue;

                double proj_roof = alpha2 * length2 * width2;
                double projv_wall = alpha2 * (height2m1 * length2 * tantv * fabs(cos((vaa - laa) * rd)) +
                                              height2m1 * width2 * tantv * fabs(cos((vaa - waa) * rd)));
                projv += projv_wall + proj_roof;

                double projs_wall = alpha2 * (height2m1 * length2 * tants * fabs(cos((saa - laa) * rd)) +
                                              height2m1 * width2 * tants * fabs(cos((saa - waa) * rd)));
                projs += projs_wall + proj_roof;
            }

            Overlapping = sqrt(tantv * tantv + tants * tants - 2 * tantv * tants * up) / (tantv + tants);
            projvs = projv + projs * Overlapping;

            double gapv_roof = exp(-projv);
            double gaps_roof = exp(-projs);
            double gapvs_roof = exp(-projvs);

            double pRoofV_temp = alpha1 * gapv_roof * length1 * width1;
            pRoofV += pRoofV_temp;

            double pRoofS_temp = alpha1 * gaps_roof * length1 * width1;
            pRoofS += pRoofS_temp;

            double pRoof_sunlit_temp = alpha1 * gapvs_roof * length1 * width1;
            pRoof_sunlit += pRoof_sunlit_temp;
        }

        double pRoof_shaded = pRoofV - pRoof_sunlit;

        // Calculate wall visibility and sunlit fractions
        double pWall_sunlit_fraction = 0, pWall_sunlit_fraction_Weight = 0;
        for (int kshape1 = 0; kshape1 < n_shape; kshape1++)
        {
            Shape shape1 = shapes[kshape1];
            double length1 = shape1.length;
            double width1 = shape1.width;
            double height1 = shape1.height;
            double alpha1 = shape1.alpha;

            double dheight = height1 / n_part;

            projv = 0;
            projs = 0;

            for (int kh = 0; kh < n_part; kh++)
            {
                double height_temp = dheight * (kh + 0.5);
                double proj = (shapes[kshape1].height - height_temp) * shapes[kshape1].alpha * tantv;
                proj = fmax(proj, 0);

                double area_roof = shapes[kshape1].alpha * shapes[kshape1].width * shapes[kshape1].length;
                area_roof = fmax(area_roof, 0);
                double proj_roof = area_roof;

                double projv_wall = (proj * shapes[kshape1].length * fabs(cos((vaa - laa) * rd)) +
                                     proj * shapes[kshape1].width * fabs(cos((vaa - waa) * rd)));

                double projs_wall = (proj * shapes[kshape1].length * fabs(cos((saa - laa) * rd)) +
                                     proj * shapes[kshape1].width * fabs(cos((saa - waa) * rd)));

                projv += projv_wall + proj_roof;
                projs += projs_wall + proj_roof;
            }

            projv /= n_part;
            projs /= n_part;

            Overlapping = sqrt(tantv * tantv + tants * tants - 2 * tantv * tants * up) / (tantv + tants);
            projvs = projv + projs * Overlapping;

            double gapv_wall = exp(-projv);
            double gapvs_wall = exp(-projvs);

            // Calculate sunlit fraction
            double lsunlit = 0;
            double part1 = fabs(laa - saa);
            if (part1 > 180) part1 = 360 - part1;
            double part2 = fabs(laa - vaa);
            if (part2 > 180) part2 = 360 - part2;
            if (part1 < 90 && part2 < 90) lsunlit = 1;

            double wsunlit = 0;
            part1 = fabs(waa - saa);
            if (part1 > 180) part1 = 360 - part1;
            part2 = fabs(waa - vaa);
            if (part2 > 180) part2 = 360 - part2;
            if (part1 < 90 && part2 < 90) wsunlit = 1;

            pWall_sunlit_fraction += alpha1 * gapvs_wall * ((height1 * length1 * tantv * fabs(cos((vaa - laa) * rd))) *
                                                            lsunlit +
                                                            (height1 * width1 * tantv * fabs(cos((vaa - waa) * rd)) * wsunlit));
            pWall_sunlit_fraction_Weight += alpha1 * gapv_wall * ((height1 * length1 * tantv * fabs(
                    cos((vaa - laa) * rd))) +
                                                                  (height1 * width1 * tantv * fabs(cos((vaa - waa) * rd))));
        }

        if (pWall_sunlit_fraction_Weight != 0)
            pWall_sunlit_fraction /= pWall_sunlit_fraction_Weight;
        else
            pWall_sunlit_fraction = 0;

        double pWallV = pWall_sunlit_fraction_Weight;
        double pWallV_sunlit = pWallV * pWall_sunlit_fraction;
        double pWallV_shaded = pWallV - pWallV_sunlit;

        pStreetV = 1 - pWallV - pRoofV;
        pStreetV_sunlit = pStreetV * pStreetV_sunlit_fraction;
        pStreetV_shaded = pStreetV - pStreetV_sunlit;

        // Accumulate each proportion value
        if (pRoof_sunlit < 0 || pRoof_sunlit > 1 ||
            pRoof_shaded < 0 || pRoof_shaded > 1 ||
            pWallV_sunlit < 0 || pWallV_sunlit > 1 ||
            pWallV_shaded < 0 || pWallV_shaded > 1 ||
            pStreetV_sunlit < 0 || pStreetV_sunlit > 1 ||
            pStreetV_shaded < 0 || pStreetV_shaded > 1)
        {
            continue;
        }
        n_vza++;
        total_pRoof_sunlit += pRoof_sunlit;
        total_pRoof_shaded += pRoof_shaded;
        total_pWallV_sunlit += pWallV_sunlit;
        total_pWallV_shaded += pWallV_shaded;
        total_pStreetV_sunlit += pStreetV_sunlit;
        total_pStreetV_shaded += pStreetV_shaded;
    }

    *frs = (float)(total_pRoof_sunlit / n_vza);
    *frh = (float)(total_pRoof_shaded / n_vza);
    *fws = (float)(total_pWallV_sunlit / n_vza);
    *fwh = (float)(total_pWallV_shaded / n_vza);
    *fss = (float)(total_pStreetV_sunlit / n_vza);
    *fsh = (float)(total_pStreetV_shaded / n_vza);
}

void RT::netrad_shortwave_urban(std::shared_ptr<Defined> m_pDefined, std::shared_ptr<PixelIO> m_pPixelio) {
    // 获取动态变量和静态变量
    NetRad &netrad = m_pPixelio->m_pDynamicVariable->netrad;  // 短波净辐射
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal;  // 光谱信息
    Urban &urban = m_pPixelio->m_pInputset->urban;  // 城市结构信息
    float sza = m_pPixelio->m_angle.sza;  // 太阳高度角
    float saa = m_pPixelio->m_angle.saa; // 太阳方位角
    int k_node = m_pPixelio->k_node;  // 节点索引
    Meteo &meteo = m_pPixelio->m_pInputset->vMeteo[k_node];  // 气象数据
    float *direct_ = m_pDefined->m_atomcond.fesun;  // 直接辐射
    float *diffuse_ = m_pDefined->m_atomcond.fesky;  // 散射辐射
    float *wl_ = m_pDefined->m_atomcond.wl;  // 波长

    // 初始化净辐射变量
    netrad.diffuseVrad_roof = 0;  // 屋顶散射辐射
    netrad.directVrad_roof = 0;  // 屋顶直接辐射
    netrad.diffuseVrad_wall = 0;  // 墙壁散射辐射
    netrad.directVrad_wall = 0;  // 墙壁直接辐射
    netrad.diffuseVrad_street = 0;  // 街道散射辐射
    netrad.directVrad_street = 0;  // 街道直接辐射

    netrad.directPrad_roof = 0;  // 屋顶直接辐射强度
    netrad.diffusePrad_roof = 0.0;  // 屋顶散射辐射强度
    netrad.directPrad_wall = 0;  // 墙壁直接辐射强度
    netrad.diffusePrad_wall = 0.0;  // 墙壁散射辐射强度
    netrad.directPrad_street = 0;  // 街道直接辐射强度
    netrad.diffusePrad_street = 0.0;  // 街道散射辐射强度

    //读取城市结构参数，测试时采用默认值
//    float height = urban.height;
//    float length = urban.length;
//    float width = urban.width;
//    float density = urban.density;
    float height = 20;
    float length = 10;
    float width = 10;
    float density = 0.0015;

    float hw_ratio = height / width; // h/w 的值
    float sqrt_term = std::sqrt(hw_ratio * hw_ratio + 1); // [(h/w)^2 + 1]^(1/2)
    float svf_s = sqrt_term - hw_ratio;
    float svf_w = 0.5 * (hw_ratio + 1 - sqrt_term) / hw_ratio;

    //初始化组分比例
    float frs = 0;
    float frh = 0;
    float fws = 0;
    float fwh = 0;
    float fss = 0;
    float fsh = 0;
    calculate_component_fraction(length, width, height, density, sza, saa, &frs, &frh, &fws, &fwh, &fss, &fsh);

    // 常数定义
    float A = 6.02214E23;  // 阿伏伽德罗常数
    float H = 6.6262E-34;  // 普朗克常数
    float C = 299792458.0;  // 光速

    // 条件判断，若入射辐射小于20或太阳高度角大于75，直接返回
    if (meteo.rin < 20 || sza > 75)
        return;

    // 遍历所有光谱波段
    for (int kband = 0; kband < N1; kband++) {
        // 计算直接辐射和散射辐射
        float Esun = meteo.rin * direct_[kband] * 0.001;
        float Esky = meteo.rli * diffuse_[kband] * 0.001;
        float diffuse_roof = 0.0;
        float diffuse_wall = 0.0;
        float diffuse_street = 0.0;
        float direct_roof = 0.0;
        float direct_wall = 0.0;
        float direct_street = 0.0;

        float wl = wl_[kband];  // 当前波段的波长
        float ref_roof = 0.20;
        float ref_wall = 0.13;
        float ref_street = 0.18;
        float BAI = (density * 1000 * 1000 * (length * width + 2 * height * length + 2 * height * width)) / (1000 * 1000);
        m_pPixelio->m_pInputset->urban.bai = BAI;

        // 计算城市各组分的散射辐射
        diffuseScatter_VNIR_Urban(ref_roof, ref_wall, ref_street, svf_s, svf_w, frs, frh, fws, fwh, fss, fsh, BAI, sza, Esun, Esky, &diffuse_roof, &diffuse_wall, &diffuse_street);

        // 更新净辐射值
        netrad.diffuseVrad_roof += diffuse_roof;
        netrad.diffuseVrad_wall += diffuse_wall;
        netrad.diffuseVrad_street += diffuse_street;
        netrad.directVrad_roof += (Esun / cos(sza * PI / 180.0))  * (1 - ref_roof) + Esky * (1 - ref_roof);
        netrad.directVrad_wall += (Esun / sin(sza * PI / 180.0)) * (1 - ref_wall) + Esky * (1 - ref_wall) * svf_w;
        netrad.directVrad_street += (Esun / cos(sza * PI / 180.0)) * (1 - ref_street) + Esky * (1 - ref_street) * svf_s;

    }
}


void RT::netrad_longwave_urban(std::shared_ptr<Defined> m_pDefined,std::shared_ptr<PixelIO>  m_pPixelio)
{

    // 获取需要操作的变量引用
    NetRad &netrad = m_pPixelio->m_pDynamicVariable->netrad; // 存储辐射变量的引用
    Spectral &spectral = m_pPixelio->m_pStaticVariable->spectal; // 光谱信息的引用
    Canopy &canopy = m_pPixelio->m_pInputset->canopy; // 冠层参数的引用
    int k_node = m_pPixelio->k_node; // 节点索引
    Meteo &meteo = m_pPixelio->m_pInputset->vMeteo[k_node]; // 气象数据的引用
    Thermal &thermal = m_pPixelio->m_pDynamicVariable->thermal; // 热力学变量的引用
    Urban &urban = m_pPixelio->m_pInputset->urban;

    netrad.diffuseTrad_roof = 0;
    netrad.directTrad_roof = 0;
    netrad.diffuseTrad_wall = 0;
    netrad.directTrad_wall = 0;
    netrad.diffuseTrad_street = 0;
    netrad.directTrad_street = 0;

    // rad in and rad out
    float Esun = 0; // 太阳直射辐射，这里初始化为0，因为长波辐射主要考虑天空漫射
    float Esky = meteo.rli; // 天空漫射辐射
    float diffuse_roof = 0.0;
    float diffuse_wall = 0.0;
    float diffuse_street = 0.0;
    float direct_roof = 0.0;
    float direct_wall = 0.0;
    float direct_street = 0.0;

    netrad.directTrad_roof = 0;
    netrad.directTrad_wall = 0;
    netrad.directTrad_street = 0;
    netrad.diffuseTrad_roof = 0;
    netrad.diffuseTrad_wall = 0;
    netrad.diffuseTrad_street = 0;

    // 获取热力学变量中的温度信息
    float Trs = thermal.Troofsunlit;
    float Trh = thermal.Troofshaded;
    float Tws = thermal.Twallsunlit;
    float Twh = thermal.Twallshaded;
    float Tss = thermal.Tstreetsunlit;
    float Tsh = thermal.Tstreetshaded;

    float bai = urban.bai;
    // compo info
    float emis_roof = 0.950;
    float emis_wall = 0.920;
    float emis_street = 0.955;
    float rrho = 1 - emis_roof;
    float wrho = 1 - emis_wall;
    float srho = 1 - emis_street;
    float ltau = 0.0;

    float Ers = SCI::StefanBoltzmann(Trs);
    float Erh = SCI::StefanBoltzmann(Trh);
    float Ews = SCI::StefanBoltzmann(Tws);
    float Ewh = SCI::StefanBoltzmann(Twh);
    float Ess = SCI::StefanBoltzmann(Tss);
    float Esh = SCI::StefanBoltzmann(Tsh);
    float prs = 0.5, pws = 0.5, pss = 0.5;
    float Er = Ers * prs + Erh * (1 - prs);  // 混合太阳辐射
    float Ew = Ews * pws + Ewh * (1 - pws);  // 混合太阳辐射
    float Es = Ess * pss + Esh * (1 - pss);  // 混合太阳辐射
    float p= 0.88 * (1 - exp(-0.7 * pow(bai, 0.75))); //需要计算，先写定值
    float up = (1 - p) / 2.0; //不准确但是这里简单认为向上向下概率相同
    netrad.directTrad_roof = 0;
    netrad.directTrad_wall = 0;
    netrad.directTrad_street = 0;

    float sza = m_pPixelio->m_angle.sza;
    float saa = m_pPixelio->m_angle.saa;
    float rs = spectral.soilRefl_ir;
    float wl = -1;

    float height = 20;
    float length = 10;
    float width = 10;
    float density = 0.0015;
    float hw_ratio = height / width; // h/w 的值
    float sqrt_term = std::sqrt(hw_ratio * hw_ratio + 1); // [(h/w)^2 + 1]^(1/2)
    float svf_s = sqrt_term - hw_ratio;
    float svf_w = 0.5 * (hw_ratio + 1 - sqrt_term) / hw_ratio;

    //初始化组分比例
    float frs = 0;
    float frh = 0;
    float fws = 0;
    float fwh = 0;
    float fss = 0;
    float fsh = 0;
    calculate_component_fraction(length, width, height, density, sza, saa, &frs, &frh, &fws, &fwh, &fss, &fsh);
    //如果叶面积指数大于0,调用 diffuseScatter_TIR 函数计算叶片和土壤的散射辐射,累加叶片和土壤的散射辐射到净辐射变量中。
    diffuseScatter_TIR_urban(bai, svf_w, svf_s, frs, frh, fws, fwh, fss, fsh, rrho, wrho, srho, sza, Esun, Esky, Trs, Trh, Tws, Twh, Tss, Tsh,&diffuse_roof, &diffuse_wall, &diffuse_street);
    netrad.diffuseTrad_roof += diffuse_roof;
    netrad.diffuseTrad_wall += diffuse_wall;
    netrad.diffuseTrad_street += diffuse_street;
}

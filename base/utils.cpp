#include "utils.h"

#include <algorithm>


/* Calculate the Sun Latitude according the Day Number calculated
   by function DayNumber                                          */

float SunLatitude(int nDayNumber)
{
	float fSunAngle;
	float fSunLatitude;

	fSunAngle=2.*PI*(nDayNumber-1.)/365.0;

	fSunLatitude=0.006894 - 0.399512*cos(fSunAngle) + 0.072075*sin(fSunAngle)
		- 0.006799*cos(2.*fSunAngle) + 0.000896*sin(2.*fSunAngle)
		- 0.002689*cos(3.*fSunAngle) + 0.001516*sin(3.*fSunAngle);

	fSunLatitude = fSunLatitude*180./PI;
	return fSunLatitude;
}


/* Caculate the TimeDifference from Day Number, the unit is rad.,
   fTimeDifference*229.183 is to convert rad. to minute           */

float TimeDifference( int nDayNumber)
{
	float fSunAngle;
	float fTimeDifference;

	fSunAngle=2.*PI*(nDayNumber-1.)/365.0;

	fTimeDifference = 0.000043 + 0.002061*cos(fSunAngle) - 0.032040*sin(fSunAngle)
		  - 0.014974*cos(2.*fSunAngle) - 0.040685*sin(2.*fSunAngle);
	fTimeDifference  = fTimeDifference * 229.183;
	return fTimeDifference;
}

float TimeNumber(int hour, int minute, int second)
{
   float fTimeNumber;
   fTimeNumber = hour + minute/60.0 + second/3600.0;
   return fTimeNumber;
}

float SunHeightAngle(float fLatitude, float fSunLatitude, float fTimeNumber)
{
   float fTimeAngle;
   float fSunHeightAngle;
   float fSinSunHeightAngle;

   if (fTimeNumber <= 12)
      fTimeAngle = (12.0 - fTimeNumber) * 15.0;
   else
      fTimeAngle = (fTimeNumber - 12.0) * 15.0;

   fTimeAngle = fTimeAngle * PI/180;

   fSinSunHeightAngle = sin(PI*fLatitude/180)*sin(PI*fSunLatitude/180) +
       cos(PI*fLatitude/180)*cos(PI*fSunLatitude/180)*cos(fTimeAngle);
   fSunHeightAngle = asin(fSinSunHeightAngle)*180.0/PI;

   return fSunHeightAngle;
}

float SunPosition(float fLatitude, float fSunHeightAngle, float fSunLatitude)
{
   float fSunPosition;
   float fCosSunPosition;

   fCosSunPosition = tan(PI*fSunHeightAngle/180.0)*tan(PI*fLatitude/180.0) -
       sin(PI*fSunLatitude/180.0)/((cos(PI*fSunHeightAngle/180))*(cos(PI*fLatitude/180.0)));
   fSunPosition = acos(fCosSunPosition);
   fSunPosition = fSunPosition * 180/PI;
   return fSunPosition;
}

 void Utils::sunAngle(float t,float fLongtitude,float fLatitude,float *zenith, float* azimuth)
{
	int nDayNumber;
	float fTimeNumber,fSunLatitude1,fSunLatitude2,fSunLatitude,fTimeDifference,fTimeNumber1,fSunHeightAngle,fSunPosition;
	float sunangle[2];
	nDayNumber = int(t);
	fTimeNumber = 24*(t-int(t));


	fSunLatitude1 = SunLatitude(nDayNumber);
	fSunLatitude2 = SunLatitude(nDayNumber + 1);
	fSunLatitude = (fSunLatitude2 + fSunLatitude1)/2.0;

	fTimeDifference = TimeDifference(nDayNumber);
	fTimeNumber1 = fTimeNumber + fTimeDifference/60 + (fLongtitude - 120)*4/60;
	fSunHeightAngle = SunHeightAngle(fLatitude, fSunLatitude, fTimeNumber1);
	fSunPosition = SunPosition( fLatitude, fSunHeightAngle, fSunLatitude);

	if(fTimeNumber1>12)
		fSunPosition = 180+fSunPosition;
	else
		fSunPosition = 180-fSunPosition;

	fSunHeightAngle = 90-fSunHeightAngle;
	*zenith = fSunHeightAngle;
	*azimuth = fSunPosition;

}

std::vector<std::string> Utils::splitt(std::string& s, std::string& deli)
{
	std::vector<std::string> ret;
	if (deli.empty()) {
		if (!s.empty()) ret.push_back(s);
		return ret;
	}

	std::string::size_type last = 0;
	while (last < s.size()) {
		const auto index = s.find_first_of(deli, last);
		const auto end = index == std::string::npos ? s.size() : index;
		if (end > last) ret.push_back(s.substr(last, end - last));
		if (index == std::string::npos) break;
		last = index + 1;
	}
	return ret;
}

float Utils::expint(float x)
{
	int i1 = 1000;
	int i2 = 100000;
	double sum = 0.0, ii;
	for (int i = i1; i < i2; i++)
	{
		ii = i / 1000.0;
		sum = sum + exp(-x * ii) / ii * 0.001;
	}
	return sum;

}

int Utils::getMapIndex(std::map<std::string, int> maps, std::string name)
{
	int loc = 0;
	std::map<std::string, int>::iterator it;
	it = maps.find(name);
	if (it != maps.end())
		loc = it->second;
	return loc;
	return 0;
}

bool isnum(char n)
{
	return (n >= '0' && n <= '9');
}
std::vector <int> Utils::findnum(std::string& ch)
{
	std::vector <int> numVec;
	int k = ch.size();
	int* num = new int[k];
	int result;
	int n = 0;
	int i = 0;
	while (n < k) {
		result = 0;
		if (isnum(ch[n]))
		{
			result = ch[n] - '0';
			while (n < k && isnum(ch[++n]))
				result = (ch[n] - '0') + 10 * result;
			num[i++] = result;
		}
		++n;
	}
	for (int j = 0; j < i; ++j)
	{
		numVec.push_back(num[j]);
	}
	return numVec;
}


float Utils::max(float a, float b) { if (a > b) return a; else return b; }
float Utils::min(float a, float b) { if (a < b) return a; else return b; }


float* Utils::readascfile(std::string infileName, int skip, int col, int &num)
{
	std::string line;
	std::vector <std::string> fields;
	std::string deli(" ");

	std::ifstream infile(infileName.c_str());
	if (infile.is_open())
	{
		num = 0;
		if (skip != 0)
		{
			for (int i = 0; i < skip; i++) std::getline(infile, line);
		}
		while (std::getline(infile, line))
		{
			fields = Utils::splitt(line, deli);
			if (int(fields.size()) >= 1)
			{
				num++;
			}
		}
	}
	else std::cout << "Unable to open the file: " << infileName << std::endl;
	infile.close();

	float* mydata = new float[num];
	if (num >= 1)
	{

		int jj = 0;
		std::ifstream infilee(infileName.c_str());
		mydata = new float[num];
		//getline(infilee,line);

		if (skip != 0)
		{
			for (int i = 0; i < skip; i++) std::getline(infilee, line);
		}
		while (std::getline(infilee, line))
		{
			fields = Utils::splitt(line, deli);
			if (int(fields.size()) > col)
			{
				mydata[jj] = atof(fields[col].c_str());
				jj = jj + 1;
			}
		}
		infilee.close();
	}

	return mydata;
}



int Utils::readascfileinout(std::string infileName, int skip, int col, std::vector<float> &data, int &num)
{
    std::string line;
    std::vector <std::string> fields;
    std::string deli(" ");

    std::ifstream infile(infileName.c_str());
    if (infile.is_open())
    {
        num = 0;
        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infile, line);
        }
        while (std::getline(infile, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) >= 1)
            {
                num++;
            }
        }
    }
    else std::cout << "Unable to open the file: " << infileName << std::endl;
    infile.close();

//    float* mydata = new float[num];

    data.clear();
    if (num >= 1)
    {
        int jj = 0;
        std::ifstream infilee(infileName.c_str());
//        mydata = new float[num];
        //getline(infilee,line);

        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infilee, line);
        }
        while (std::getline(infilee, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) > col)
            {
//                mydata[jj] = atof(fields[col].c_str());
                data.push_back(atof(fields[col].c_str()));
                jj = jj + 1;
            }
        }
        infilee.close();
    }

    return 1;
}

int Utils::readcsvfileinout(std::string infileName, int skip, int col, std::vector<float> &data, int &num)
{
    std::string line;
    std::vector <std::string> fields;
    std::string deli(",");

    std::ifstream infile(infileName.c_str());
    if (infile.is_open())
    {
        num = 0;
        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infile, line);
        }
        while (std::getline(infile, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) >= 1)
            {
                num++;
            }
        }
    }
    else std::cout << "Unable to open the file: " << infileName << std::endl;
    infile.close();

//    float* mydata = new float[num];

    data.clear();
    if (num >= 1)
    {
        int jj = 0;
        std::ifstream infilee(infileName.c_str());
//        mydata = new float[num];
        //getline(infilee,line);

        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infilee, line);
        }
        while (std::getline(infilee, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) > col)
            {
//                mydata[jj] = atof(fields[col].c_str());
                data.push_back(atof(fields[col].c_str()));
                jj = jj + 1;
            }
        }
        infilee.close();
    }

    return 1;
}


std::string Utils::getDirectoryPath(const std::string& fullPath)
{
    size_t lastSlash = fullPath.find_last_of('/');
    if(lastSlash!=std::string::npos)
    {
        return fullPath.substr(0,lastSlash);
    }else{
        return "";
    }
}

std::string Utils::getFileName(const std::string& fullPath)
{
    size_t lastSlash = fullPath.find_last_of('/');
    if(lastSlash!=std::string::npos)
    {
        return fullPath.substr(lastSlash+1);
    }else{
        return fullPath;
    }
}

template<typename T>
int Utils::getNextPow2Number(T number) {
    return std::pow(2, std::ceil(std::log2(number)));
}

std::vector<std::vector<float>> Utils::readImage(std::string infilename) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return std::vector<std::vector<float>>();
    }
    int width = poDataset->GetRasterXSize();          //获取影像信息
    int height = poDataset->GetRasterYSize();
    int nBands = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    std::vector<std::vector<float>> collected(nBands);
    for(int kband=1;kband < nBands;kband++)
    {
//        GDALRasterBand * band = poDataset->GetRasterBand(kband);
//        float *bandData = new float(width*height);
//        band->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
//        collected[kband-1].assign(bandData,bandData+width*height);
//        delete [] bandData;

        GDALRasterBand * poBand = poDataset->GetRasterBand(kband);
        float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
        CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
        collected[kband-1].assign(bandData,bandData+width*height);
        CPLFree(bandData);
    }

    double geoTransform[6];                       //获取坐标信息
    poDataset->GetGeoTransform(geoTransform);
    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return collected;
}


int Utils::saveImage(std::string outfilepath, std::vector<std::vector<float>> &c,
                     int width, int height,int band, std::string proj, double trans[6]) {

    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
    GDALAllRegister();  //注册所有的驱动

//    GDALDataset *poDataset;   //GDAL数据集
//    GDALRasterBand* poBand = poDataset->GetRasterBand(band);
//    GDALDataType type = poBand->GetRasterDataType();

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *ods = driver->Create(outfilepath.c_str(),width,height,band,GDT_Float32,NULL);
    for(int kband = 0;kband<band;kband++) {
        GDALRasterBand *oBand = ods->GetRasterBand(kband+1);
        //float *bandData = c[kband].data();
        CPLErr result = oBand->RasterIO(GF_Write, 0, 0, width, height, c[kband].data(), width, height, GDT_Float32, 0, 0);
    }
    GDALClose(ods);
    return 1;
}

int Utils::saveGeoImage(std::string outfilepath, std::vector<std::vector<float>> &c,
						 int width, int height, int band, std::string proj, double trans[6]) {

	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
	GDALAllRegister();  // 注册所有的驱动

	// 获取驱动
	GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (driver == nullptr || width <= 0 || height <= 0 || band <= 0 ||
		static_cast<int>(c.size()) < band) {
		std::cerr << "Unable to create GeoTIFF: invalid output dimensions or data.\n";
		return 0;
	}
	GDALDataset *ods = driver->Create(outfilepath.c_str(), width, height, band, GDT_Float32, NULL);
	if (ods == nullptr) {
		std::cerr << "Unable to create GeoTIFF: " << outfilepath << std::endl;
		return 0;
	}

	// 设置投影信息
	if (!proj.empty()) {
		ods->SetProjection(proj.c_str()); // 设置投影
	}

	// 设置地理变换信息
	if (trans != nullptr) {
		ods->SetGeoTransform(trans);  // 设置地理变换参数
	}

//    GDALRasterBand *oBand = ods->GetRasterBand(1);  // 直接获取第一个波段
//    oBand->RasterIO(GF_Write, 0, 0, width, height, c[0].data(), width, height, GDT_Float32, 0, 0);

	// 写入数据
	for (int kband = 0; kband < band; kband++) {
		if (static_cast<int>(c[kband].size()) < width * height) {
			GDALClose(ods);
			std::cerr << "Unable to write GeoTIFF: insufficient band data.\n";
			return 0;
		}
		GDALRasterBand *oBand = ods->GetRasterBand(kband + 1);
		CPLErr result = oBand->RasterIO(GF_Write, 0, 0, width, height, c[kband].data(), width, height, GDT_Float32, 0, 0);
	}

	// 关闭文件
	GDALClose(ods);
	return 1;
}

int Utils::saveImage1(std::string outfilepath, std::vector<float> &c,
                     int width, int height,int band, std::string proj, double trans[6]) {

    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
    GDALAllRegister();  //注册所有的驱动

//    GDALDataset *poDataset;   //GDAL数据集
//    GDALRasterBand* poBand = poDataset->GetRasterBand(band);
//    GDALDataType type = poBand->GetRasterDataType();

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *ods = driver->Create(outfilepath.c_str(),width,height,1,GDT_Float32,NULL);
    GDALRasterBand *oBand = ods->GetRasterBand(1);
    float *bandData = c.data();
    CPLErr result =oBand->RasterIO(GF_Write,0, 0,width,height,c.data(),width,height,GDT_Float32,0,0);

    GDALClose(ods);
    return 1;
}


int Utils::readImageinout(std::string infilename,std::vector<std::vector<float>> &collected,
                          int &width, int &height, int &nband) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    //std::vector<std::vector<float>> collected(nband);
    for(int kband=1;kband < nband+1;kband++)
    {
//        GDALRasterBand * band = poDataset->GetRasterBand(kband);
//        float *bandData = new float(width*height);
//        band->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
//        collected[kband-1].assign(bandData,bandData+width*height);
//        delete [] bandData;

        GDALRasterBand * poBand = poDataset->GetRasterBand(kband);
        float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
        CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
        std::vector<float> tempcollected;
        tempcollected.assign(bandData,bandData+width*height);
        collected.push_back(tempcollected);
        CPLFree(bandData);

    }

    double geoTransform[6];                       //获取坐标信息
    poDataset->GetGeoTransform(geoTransform);
    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return 1;
}


int Utils::readImageinout1(std::string infilename,std::vector<float> &collected,
                          int &width, int &height, int &nband) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    //std::vector<std::vector<float>> collected(nband);

    GDALRasterBand * poBand = poDataset->GetRasterBand(1);
    float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
    CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);

    collected.assign(bandData,bandData+width*height);
    CPLFree(bandData);


   // std::cout<<"123"<<std::endl;
//    double geoTransform[6];                       //获取坐标信息
//    poDataset->GetGeoTransform(geoTransform);
//    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return 1;
}

int Utils::readImageinout11(std::string infilename,std::vector<float> &collected,
                           int &width, int &height, int &nband,double *trans, std::string &proj) {

	CPLSetConfigOption("PROJ_LIB", "/home/hero/vcpkg/installed/x64-linux/share/proj/");

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    //std::vector<std::vector<float>> collected(nband);

    GDALRasterBand * poBand = poDataset->GetRasterBand(1);
    float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
    CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);

    collected.assign(bandData,bandData+width*height);
    CPLFree(bandData);


     // std::cout<<"123"<<std::endl;
    // double geoTransform[6];                       //获取坐标信息
	// double trans[6];
    poDataset->GetGeoTransform(trans);
    proj = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return 1;
}


int Utils::readImageInfo(std::string infilename, int &width, int &height, int & nband) {

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;

    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    return 1;
}



bool Utils::readHdf5image(std::string infilename, std::string objname, std::vector<std::vector<float>> &cc, int &width, int &height, int &nband) {


//    std::string outfile = "/home/airt/work/stream/stream_data/example/dem_era5_25_c.h5";
//    std::vector<float> test_;
//    int width,height,band;
//    Utils::readHdf5image1(infile,"test",test_,width,height,band);
//    Utils::writeHdf5image1(outfile, "test",test_,width,height);

    // 打开HDF5文件
    hid_t file_id;
    herr_t status;
    // Input datasets must also work when the source directory is read-only.
    file_id = H5Fopen(infilename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) {
        cc.clear();
        width = height = nband = 0;
        std::cerr << "Unable to open HDF5 file: " << infilename << std::endl;
        return false;
    }

    // 创建数据集中的数据本身
    hid_t dataset_id;    // 数据集本身的id
    // dataset_id = H5Dopen(group位置id,
    //                 const char *name, 数据集名
    //                    数据集访问性质)
    dataset_id = H5Dopen(file_id, objname.c_str(), H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Fclose(file_id);
        cc.clear();
        width = height = nband = 0;
        std::cerr << "Unable to open HDF5 dataset: " << objname << std::endl;
        return false;
    }


    hid_t dspace = H5Dget_space(dataset_id);/** 获取数据集大小信息 */
    if (dspace < 0 || H5Sget_simple_extent_ndims(dspace) != 3) {
        if (dspace >= 0) H5Sclose(dspace);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        cc.clear();
        width = height = nband = 0;
        std::cerr << "HDF5 dataset must have three dimensions: " << objname << std::endl;
        return false;
    }
    hsize_t dims[3];
    H5Sget_simple_extent_dims(dspace, dims, NULL);
    nband = dims[0];
    height= dims[1];
    width = dims[2];
//    std::cout<<width<<height<<std::endl;


    hid_t dtype;
    int len;
    dtype = H5Dget_type(dataset_id);
    len = H5Tget_size(dtype);
   //std::cout<<(dtype);

    // 将数据写入数据集
    // herr_t 读取状态 = H5Dread(写入目标数据集id,
    //                              内存数据类型,
    //                       memory_dataspace_id, 定义内存dataspace和其中的选择
    //                          - H5S_ALL: 文件中dataspace用做内存dataspace，file_dataspace_id中的选择作为内存dataspace的选择
    //                         file_dataspace_id, 定义文件中dataspace的选择
    //                          - H5S_ALL: 文件中datasapce的全部，定义为数据集中dataspace定义的全部维度数据
    //                        本次IO操作的转换性质,
    //                          const void * buf, 内存中接受数据的位置
    float *bandData = (float *)CPLMalloc(sizeof(float)*width*height*nband);
    status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, bandData);
    for(int k = 0;k <nband;k++) {
        int shift = k * width * height;
        std::vector<float> c;
        c.assign(bandData + shift, bandData +shift + width * height);
        cc.push_back(c);
    }
    CPLFree(bandData);




    // 关闭dataset相关对象
    status = H5Dclose(dataset_id);
    status = H5Sclose(dspace);

    // 关闭文件对象
    status = H5Fclose(file_id);


    return true;
}


bool Utils::writeHdf5image(std::string outfilepath, std::string objname, std::vector<std::vector<float>> &cc, int width, int height, int nband) {

    if (width <= 0 || height <= 0 || nband <= 0 ||
        static_cast<int>(cc.size()) < nband) {
        std::cerr << "Unable to write HDF5 image: invalid dimensions or data.\n";
        return false;
    }

    const int data_rank =3;
    hid_t file_id;
    file_id = H5Fcreate(outfilepath.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        std::cerr << "Unable to create HDF5 file: " << outfilepath << std::endl;
        return false;
    }


    hsize_t dims[3];
    dims[0] = nband;
    dims[1] = height;
    dims[2] = width;
//    dims[0] = height;
//    dims[1] = width;
    hid_t dataspace_id, dataset_id;
    dataspace_id = H5Screate_simple(data_rank, dims, NULL);

    //hid_t plist = H5Pcreate(H5P_DATASET_CREATE);
    //H5Pset_chunk(plist,)
    //H5Pset_deflate(plist,6);
    dataset_id = H5Dcreate2(file_id, objname.c_str(), H5T_IEEE_F32BE, dataspace_id,
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Sclose(dataspace_id);
        H5Fclose(file_id);
        std::cerr << "Unable to create HDF5 dataset: " << objname << std::endl;
        return false;
    }


   // hid_t filespace_id = H5Dget_space(dataset_id);

    const int planeSize = width * height;
    const int totalsize = planeSize * nband;
   float *data = new float[totalsize];
   float * walkarr = data;

    for(int i =0;i<nband;i++)
    {
        if (static_cast<int>(cc[i].size()) < width * height) {
            H5Dclose(dataset_id);
            H5Sclose(dataspace_id);
            H5Fclose(file_id);
            delete [] data;
            std::cerr << "Unable to write HDF5 image: insufficient band data.\n";
            return false;
        }
        std::copy_n(cc[i].begin(), planeSize, walkarr);
        walkarr += planeSize;
    }


    herr_t status;
    status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT_g, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, data);

    if (status < 0) {
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        delete [] data;
        std::cerr << "Unable to write HDF5 dataset: " << objname << std::endl;
        return false;
    }

    status = H5Sclose(dataspace_id);
    status = H5Dclose(dataset_id);
    status = H5Fclose(file_id);

    delete [] data;
    return true;

}


bool Utils::readHdf5image1(std::string infilename, std::string objname, std::vector<float> &c, int &width, int &height, int &nband) {


//    std::string outfile = "/home/airt/work/stream/stream_data/example/dem_era5_25_c.h5";
//    std::vector<float> test_;
//    int width,height,band;
//    Utils::readHdf5image1(infile,"test",test_,width,height,band);
//    Utils::writeHdf5image1(outfile, "test",test_,width,height);

    // 打开HDF5文件
    hid_t file_id;
    herr_t status;
    // Input datasets must also work when the source directory is read-only.
    file_id = H5Fopen(infilename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) {
        c.clear();
        width = height = nband = 0;
        std::cerr << "Unable to open HDF5 file: " << infilename << std::endl;
        return false;
    }

    // 创建数据集中的数据本身
    hid_t dataset_id;    // 数据集本身的id
    // dataset_id = H5Dopen(group位置id,
    //                 const char *name, 数据集名
    //                    数据集访问性质)
    dataset_id = H5Dopen(file_id, objname.c_str(), H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Fclose(file_id);
        c.clear();
        width = height = nband = 0;
        std::cerr << "Unable to open HDF5 dataset: " << objname << std::endl;
        return false;
    }


    hid_t dspace = H5Dget_space(dataset_id);/** 获取数据集大小信息 */
    if (dspace < 0 || H5Sget_simple_extent_ndims(dspace) != 2) {
        if (dspace >= 0) H5Sclose(dspace);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        c.clear();
        width = height = nband = 0;
        std::cerr << "HDF5 dataset must have two dimensions: " << objname << std::endl;
        return false;
    }
    hsize_t dims[3];
    H5Sget_simple_extent_dims(dspace, dims, NULL);
    height= dims[0];
    width = dims[1];
//    std::cout<<width<<height<<std::endl;


    hid_t dtype;
    int len;
    dtype = H5Dget_type(dataset_id);
    len = H5Tget_size(dtype);
    //std::cout<<(dtype);

    // 将数据写入数据集
    // herr_t 读取状态 = H5Dread(写入目标数据集id,
    //                              内存数据类型,
    //                       memory_dataspace_id, 定义内存dataspace和其中的选择
    //                          - H5S_ALL: 文件中dataspace用做内存dataspace，file_dataspace_id中的选择作为内存dataspace的选择
    //                         file_dataspace_id, 定义文件中dataspace的选择
    //                          - H5S_ALL: 文件中datasapce的全部，定义为数据集中dataspace定义的全部维度数据
    //                        本次IO操作的转换性质,
    //                          const void * buf, 内存中接受数据的位置
    float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
    status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, bandData);

    c.assign(bandData, bandData + width * height);

    CPLFree(bandData);




    // 关闭dataset相关对象
    status = H5Dclose(dataset_id);
    status = H5Sclose(dspace);

    // 关闭文件对象
    status = H5Fclose(file_id);


    return true;
}


bool Utils::writeHdf5image1(std::string outfilepath, std::string objname, std::vector<float> &c, int width, int height) {

    if (width <= 0 || height <= 0 || static_cast<int>(c.size()) < width * height) {
        std::cerr << "Unable to write HDF5 image: invalid dimensions or data.\n";
        return false;
    }

    const int data_rank =2;
    hid_t file_id;
    file_id = H5Fcreate(outfilepath.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        std::cerr << "Unable to create HDF5 file: " << outfilepath << std::endl;
        return false;
    }

    hsize_t dims[3];
    dims[0] = height;
    dims[1] = width;
    hid_t dataspace_id, dataset_id;
    dataspace_id = H5Screate_simple(data_rank, dims, NULL);
   // H5Tcreate(file_id,)
    dataset_id = H5Dcreate2(file_id, objname.c_str(), H5T_IEEE_F32BE, dataspace_id,
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Sclose(dataspace_id);
        H5Fclose(file_id);
        std::cerr << "Unable to create HDF5 dataset: " << objname << std::endl;
        return false;
    }
//    dataset_id = H5Dcreate1(file_id, objname.c_str(), H5T_IEEE_F32BE, dataspace_id,
//                            H5P_DEFAULT);
    float *bandData = c.data();
    herr_t status;
    status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT_g, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, bandData);

    if (status < 0) {
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        std::cerr << "Unable to write HDF5 dataset: " << objname << std::endl;
        return false;
    }

    status = H5Sclose(dataspace_id);
    status = H5Dclose(dataset_id);
    status = H5Fclose(file_id);


    return true;

}


void Utils::vec2arr(std::vector<std::vector<float>> &vec,float * newarr) {



}

void Utils::calculatePixelRange(double trans1[6], const std::string &proj1, int width1, int height1,
						  double trans2[6], const std::string &proj2, int width2, int height2,
						  int &startWidth, int &startHeight, int &endWidth, int &endHeight) {
	// 左上角的地理坐标
	double x_topleft = trans1[0];
	double y_topleft = trans1[3];

	// 右下角的地理坐标
	double x_bottomright = trans1[0] + width1 * trans1[1];
	double y_bottomright = trans1[3] + height1 * trans1[5];

	// 创建投影转换
	OGRSpatialReference sr1, sr2;
	sr1.importFromWkt(proj1.c_str());
	sr2.importFromWkt(proj2.c_str());
	OGRCoordinateTransformation* transform = nullptr;

	if (!sr1.IsSame(&sr2)) {
		transform = OGRCreateCoordinateTransformation(&sr1, &sr2);
		if (transform == nullptr) {
			std::cerr << "无法创建投影转换！" << std::endl;
			return;
		}

		// 转换左上角和右下角的坐标
		transform->Transform(1, &x_topleft, &y_topleft);
		transform->Transform(1, &x_bottomright, &y_bottomright);
	}

	// 转换为目标区域的行列号
	startWidth = static_cast<int>((x_topleft - trans2[0]) / trans2[1]);
	startHeight = static_cast<int>((y_topleft - trans2[3]) / trans2[5]);
	endWidth = static_cast<int>((x_bottomright - trans2[0]) / trans2[1] + 1e-6); // 加上偏移量
	endHeight = static_cast<int>((y_bottomright - trans2[3]) / trans2[5] + 1e-6) ;

	if (transform != nullptr) {
		OGRCoordinateTransformation::DestroyCT(transform);
	}
}


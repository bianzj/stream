#pragma once
#include <vector>
#include <numbers>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <errno.h>
#include <string>
#include <stdlib.h>   /* atof */
#include "gdal.h"
#include "gdal_priv.h"
#include "structs.h"
#include "netcdf.h"
#include <hdf5.h>
#include <H5Cpp.h>



namespace Utils {

	template<typename T>
	int getNextPow2Number(T number) ;

    std::vector<std::vector<float>> readImage(std::string infilename);

    int readImageinout(std::string infilename,std::vector<std::vector<float>> &c,
                       int &width, int &height, int &nband);

    int readImageinout1(std::string infilename,std::vector<float> &c,
                       int &width, int &height, int &nband);

    int readImageinout11(std::string infilename,std::vector<float> &c,
                        int &width, int &height, int &nband,double *trans, std::string &proj);

    int saveImage(std::string outfilepath, std::vector<std::vector<float>> &c,
                  int width, int height,int band, std::string proj, double trans[6]);

    int saveImage1(std::string outfilepath, std::vector<float> &c,
                   int width, int height,int band, std::string proj, double trans[6]);

	int saveGeoImage(std::string outfilepath, std::vector<std::vector<float>> &c,
				  int width, int height,int band, std::string proj, double trans[6]);

    int readImageInfo(std::string infilename, int& width, int& height, int& nband );

	std::vector<std::string> splitt(std::string& s, std::string& deli);


	float expint(float x);

	float* readascfile(std::string infileName, int skip, int col, int& num);

    int readascfileinout(std::string infileName, int skip, int col,
                         std::vector<float> &data, int& num);

    int readcsvfileinout(std::string infileName, int skip, int col,
                         std::vector<float> &data, int& num);

	void sunAngle(float t, float longtitude, float latitude, float *zenith, float *azimuth);

	int getMapIndex(std::map<std::string, int> maps, std::string name);


	/*void imageToBuffer(VkDevice m_device, int m_queueIndex, const nvvk::Texture& imgIn, nvmath::vec3i size, const vk::Buffer& pixelBufferOut);
	void bufferToBuffer(VkDevice m_device, int m_queueIndex, const nvvk::Buffer& bufferIn, vk::DeviceSize size, const nvvk::Buffer& bufferOut);*/

	template <typename T>
	void saveOut(std::string outFile, T data, int num);

	std::vector <int> findnum(std::string& ch);

	float max(float a, float b);
	float min(float a, float b);

    std::string getDirectoryPath(const std::string& fullPath);

    std::string getFileName(const std::string& fullPath);


    bool readHdf5image1(std::string infilename,std::string objname, std::vector<float> &c,
                       int &width, int &height, int &nband);
    bool writeHdf5image1(std::string outfilepath, std::string objname, std::vector<float> &c,
                        int width, int height);

    bool readHdf5image(std::string infilename,std::string objname, std::vector<std::vector<float>> &c,
                        int &width, int &height, int &nband);
    bool writeHdf5image(std::string outfilepath, std::string objname, std::vector<std::vector<float>> &c,
                         int width, int heigh, int nband);

    void vec2arr(std::vector<std::vector<float>> &vec, float * newarr);

	void calculatePixelRange(double trans1[6], const std::string &proj1, int width1, int height1,
						  double trans2[6], const std::string &proj2, int width2, int height2,
						  int &startWidth, int &startHeight, int &endWidth, int &endHeight);

	bool readNcImage1(std::string infilename, std::string objname, std::vector<float> &c,
						  int &width, int &height, int &nband);
	bool writeNcImage1(std::string outfilepath, std::string objname, std::vector<float> &c,
					   int width, int height);
	bool readNcImage(std::string infilename, std::string objname, std::vector<std::vector<float>> &c,
					 int &width, int &height, int &nband);
	bool writeNcImage(std::string outfilepath, std::string objname, std::vector<std::vector<float>> &c,
					  int width, int height, int nband) ;


}
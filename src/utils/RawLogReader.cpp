/*
 * This file is part of Kintinuous.
 *
 * Copyright (C) 2015 The National University of Ireland Maynooth and 
 * Massachusetts Institute of Technology
 *
 * The use of the code within this file and all code within files that 
 * make up the software that is Kintinuous is permitted for 
 * non-commercial purposes only.  The full terms and conditions that 
 * apply to the code within this file are detailed within the LICENSE.txt 
 * file and at <http://www.cs.nuim.ie/research/vision/data/kintinuous/code.php> 
 * unless explicitly stated.  By downloading this file you agree to 
 * comply with these terms.
 *
 * If you wish to use any of this code for commercial purposes then 
 * please email commercialisation@nuim.ie.
 */

#include "RawLogReader.h"

RawLogReader::RawLogReader()
{

    static ParameterReader pd;

    defaultname = pd.getData("dataset_path");
   // rgb_txt_file = defaultname+"rgb.txt";
   // depth_txt_file = defaultname+"depth.txt";
    assoc_txt_file = defaultname+"associate.txt";
  //  assert(boost::filesystem::exists(ConfigArgs::get().logFile.c_str()));

  //  fp = fopen(ConfigArgs::get().logFile.c_str(), "rb");

  currentFrame = 0;
  double dfy = atof( pd.getData("fy").c_str() );

  //  assert(fread(&numFrames, sizeof(int32_t), 1, fp));
    

  /*
    //load rgbnames from rgb.txt
    fstream rgbfile;
    double timestamp_temp; string rgbname;
    rgbfile.open(rgb_txt_file.c_str(),ios::in);
    if(!rgbfile.is_open()){
      printf("ERROR: Could not open File %s\n",rgb_txt_file.c_str());   
    }else{
      printf("Reading rgb File\n");
      while(!rgbfile.eof()){
	string temp("");
	getline(rgbfile,temp);
	stringstream stream(temp);
	if(temp[0] !='#')
	{
	stream >> timestamp_temp; stream >> rgbname;
	
	timestamps.push_back((int64_t)(timestamp_temp*1000000));
	rgbNames.push_back(defaultname+rgbname);
	}
      }
    }
    rgbfile.close();
    
    //load depthnames from depth.txt
    fstream depthfile;
    string depthname; int counter =0;
    depthfile.open(depth_txt_file.c_str(),ios::in);
    if(!depthfile.is_open()){
      printf("ERROR: Could not open File %s\n",depth_txt_file.c_str());   
    }else{
      printf("Reading depth File\n");
      while(!depthfile.eof()){
	string temp("");
	getline(depthfile,temp);
	stringstream stream(temp);
	if(temp[0] !='#')
	{
	stream >> timestamp_temp; stream >> depthname;
	//if((int64_t)(timestamp_temp*1000000) != timestamps[counter]){
	//  cout << "timestamps from depthfile don't match rgbfile\n"<<endl;
	//  break;
	//}
	depthNames.push_back(defaultname+depthname);
	counter++;
	}
      }
    }
    depthfile.close();
    */
  
   fstream rgbanddepthfile;
    double timestamp_temp_rgb,timestamp_temp_depth; string rgbname; string depthname;
    rgbanddepthfile.open(assoc_txt_file.c_str(),ios::in);
    if(!rgbanddepthfile.is_open()){
      printf("ERROR: Could not open File %s\n",assoc_txt_file.c_str());   
    }else{
      printf("Reading rgb File\n");
      while(!rgbanddepthfile.eof()){
	string temp("");
	getline(rgbanddepthfile,temp);
	stringstream stream(temp);
	if(temp[0] !='#')
	{
	 if(dfy<0)
	{
	  stream >> timestamp_temp_rgb; stream >> depthname;stream >> timestamp_temp_depth; stream >> rgbname;
	} 
	else
	{
	  stream >> timestamp_temp_rgb; stream >> rgbname;stream >> timestamp_temp_depth; stream >> depthname;
	} 
	  
	//stream >> timestamp_temp_rgb; stream >> depthname;stream >> timestamp_temp_depth; stream >> rgbname;
	//stream >> timestamp_temp_rgb; stream >> rgbname;stream >> timestamp_temp_depth; stream >> depthname;
	timestamps.push_back((int64_t)(timestamp_temp_rgb*1000000));
	rgbNames.push_back(defaultname+rgbname);
	depthNames.push_back(defaultname+depthname);
	}
      }
    }
    rgbanddepthfile.close();
  
    numFrames = timestamps.size();
  
    
    compressedDepth = new unsigned char[Resolution::get().numPixels() * 2];
    compressedImage = new unsigned char[Resolution::get().numPixels() * 3];

    decompressionBuffer = new Bytef[Resolution::get().numPixels() * 2];

    if(!ConfigArgs::get().totalNumFrames)
    {
        *const_cast<int *>(&ConfigArgs::get().totalNumFrames) = numFrames;
    }

    isCompressed = true;
}

RawLogReader::~RawLogReader()
{
    delete [] compressedDepth;
    delete [] compressedImage;
    delete [] decompressionBuffer;
    //fclose(fp);
    timestamps.clear();
    rgbNames.clear();
    depthNames.clear();
}

void RawLogReader::readNext()
{
  /*
    assert(fread(&timestamp, sizeof(int64_t), 1, fp));

    assert(fread(&compressedDepthSize, sizeof(int32_t), 1, fp));
    assert(fread(&compressedImageSize, sizeof(int32_t), 1, fp));

    assert(fread(compressedDepth, compressedDepthSize, 1, fp));

    if(compressedImageSize > 0)
    {
        assert(fread(compressedImage, compressedImageSize, 1, fp));
    }

    if(deCompImage != 0)
    {
        cvReleaseImage(&deCompImage);
    }

    CvMat tempMat = cvMat(1, compressedImageSize, CV_8UC1, (void *)compressedImage);

    if(compressedImageSize == Resolution::get().numPixels() * 3)
    {
        isCompressed = false;

        deCompImage = cvCreateImage(cvSize(Resolution::get().width(), Resolution::get().height()), IPL_DEPTH_8U, 3);

        memcpy(deCompImage->imageData, compressedImage, Resolution::get().numPixels() * 3);
    }
    else if(compressedImageSize > 0)
    {
        isCompressed = true;

        deCompImage = cvDecodeImage(&tempMat);
    }
    else
    {
        isCompressed = false;

        deCompImage = cvCreateImage(cvSize(Resolution::get().width(), Resolution::get().height()), IPL_DEPTH_8U, 3);

        memset(deCompImage->imageData, 0, Resolution::get().numPixels() * 3);
    }

    if(compressedDepthSize == Resolution::get().numPixels() * 2)
    {
        //RGB should not be compressed in this case
        assert(!isCompressed);

        memcpy(&decompressionBuffer[0], compressedDepth, Resolution::get().numPixels() * 2);
    }
    else if(compressedDepthSize > 0)
    {
        //RGB should also be compressed
        assert(isCompressed);

        unsigned long decompLength = Resolution::get().numPixels() * 2;

        uncompress(&decompressionBuffer[0], (unsigned long *)&decompLength, (const Bytef *)compressedDepth, compressedDepthSize);
    }
    else
    {
        isCompressed = false;

        memset(&decompressionBuffer[0], 0, Resolution::get().numPixels() * 2);
    }
 */
    
  
    //set compressed false
    isCompressed = false;
    
    if(deCompImage != 0)
    {
        cvReleaseImage(&deCompImage);
    }
        
    //cout<<"begin read colorImg and depthImg..."<<endl;
    
    compressedImageSize = Resolution::get().numPixels() * 3;
    compressedDepthSize = Resolution::get().numPixels() * 2;
    
    deCompImage = cvCreateImage(cvSize(Resolution::get().width(), Resolution::get().height()), IPL_DEPTH_8U, 3);
    Mat colorImg,depthImg;
       
    timestamp = timestamps[currentFrame];
    colorImg = imread(rgbNames[currentFrame]);
    cvtColor(colorImg,colorImg,CV_BGR2RGB); 
    
    memcpy(deCompImage->imageData, colorImg.data, Resolution::get().numPixels() * 3);
    
   
    
    depthImg = imread(depthNames[currentFrame],cv::IMREAD_ANYDEPTH);
    depthImg = depthImg/5;
    
    memcpy(&decompressionBuffer[0], depthImg.data, Resolution::get().numPixels() * 2);
    
    //cout<<"reading colorImg and depthImg..."<<endl;
    
    //load image and depth
    decompressedDepth = (unsigned short *)&decompressionBuffer[0];
    decompressedImage = (unsigned char *)deCompImage->imageData;
    

    compressedDepth = (unsigned char *)depthImg.data;
    compressedImage = (unsigned char *)colorImg.data;
    
    if(ConfigArgs::get().flipColors)
    {
        cv::Mat3b rgb(Resolution::get().rows(),
                      Resolution::get().cols(),
                      (cv::Vec<unsigned char, 3> *)deCompImage->imageData,
                      Resolution::get().width() * 3);

        cv::cvtColor(rgb, rgb, CV_RGB2BGR);
    }

    currentFrame++;
}

bool RawLogReader::grabNext(bool & returnVal, int & currentFrame)
{
    if(hasMore() && currentFrame < ConfigArgs::get().totalNumFrames)
    {
        readNext();
        ThreadDataPack::get().trackerFrame.assignAndNotifyAll(currentFrame);
        return true;
    }
	returnVal = false;
	return false;
}

bool RawLogReader::hasMore()
{
    return currentFrame + 1 < numFrames;
}

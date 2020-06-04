#ifndef _VIBE_H
#define _VIBE_H

//#include <iostream>
//#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/video/video.hpp"

//using namespace std;
//using namespace cv;


#ifdef  NULL
#undef  NULL
#endif
#define NULL    (0)

void VibeForground(unsigned char *CurImg, unsigned char* ForImg, unsigned int ImgWidth, unsigned int ImgHeight);

#endif 

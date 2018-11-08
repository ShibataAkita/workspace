#include <stdio.h>  
#include <iostream>    
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;  
using namespace cv;  

#define T_ANGLE_THRE 10

void getDiffImage(Mat, Mat, Mat, int);
vector<RotatedRect> armorDetect(vector<RotatedRect> );//检测装甲片
void drawBox(RotatedRect, Mat);//画出矩形

int main()
{
	VideoCapture capture("../video.avi");
	VideoWriter writer("../videopro.avi",CV_FOURCC('D','I','V','X'),30,Size(640,480),true);
	Mat src_img;
	vector<vector<Point> > contour;//轮廓
	vector<Vec4i>hierarchy;
	RotatedRect s;//旋转矩形
	vector<RotatedRect> vEllipse; //检测到的目标
	vector<RotatedRect> vRlt;//装甲片
	Mat bgr[3];	

	capture >> src_img;
	Size imgSize;
	while(1)
	{
		capture>>src_img;
		imgSize = src_img.size();
		Mat bin_img = Mat(imgSize, CV_8UC1);
		Mat rImage = Mat(imgSize, CV_8UC1);
		Mat gImage = Mat(imgSize, CV_8UC1);
		Mat bImage = Mat(imgSize, CV_8UC1);
		if(src_img.empty())
		{
			break;
		}
		else
		{
			split(src_img, bgr); //分离bgr
			bImage = bgr[0];
			gImage = bgr[1];
			rImage = bgr[2];
			getDiffImage(rImage, bImage, bin_img, 50); 
			findContours(bin_img, contour,hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			for(int i=0;i<contour.size();i++)
			{
				if(contour[i].size()>15)
				{
					s=fitEllipse(contour[i]);				
					if (s.center.y>240)
					{
						vEllipse.push_back(s); 
					}
				}
			}
			vRlt = armorDetect(vEllipse); 
			for (unsigned int nI = 0; nI <vRlt.size(); nI++) 
				{
					drawBox(vRlt[nI], src_img);
				}

			//imshow("video",src_img);
			writer<<src_img;
			//waitKey(1);
			vEllipse.clear();
			vRlt.clear();
		}
	}
	capture.release();
 
	return 0;
}

void getDiffImage(Mat src1, Mat src2, Mat dst, int nThre)
{
    for (int nI = 0; nI<src1.rows; nI++)
    {
        uchar* pchar1 = src1.ptr<uchar>(nI);
        uchar* pchar2 = src2.ptr<uchar>(nI);
        uchar* pchar3 = dst.ptr<uchar>(nI);
        for (int nJ = 0; nJ <src1.cols; nJ++)
        {
            if (pchar1[nJ] - pchar2[nJ]> nThre) 
            {
                pchar3[nJ] = 255;
            }
            else
            {
                pchar3[nJ] = 0;
            }
        }
    }
}

vector<RotatedRect> armorDetect(vector<RotatedRect> vEllipse)
{
    vector<RotatedRect> vRlt;
    RotatedRect armor; //装甲片
    int nL, nW;
    double dAngle;
    vRlt.clear();
	
    if (vEllipse.size() < 2) 
        return vRlt;
    for (unsigned int nI = 0; nI < vEllipse.size() - 1; nI++) //任意两个矩形的角度差
    {
        for (unsigned int nJ = nI + 1; nJ < vEllipse.size(); nJ++)
        {
		if(fabs(vEllipse[nI].center.x-vEllipse[nJ].center.x)<=150 
			&& fabs(vEllipse[nI].center.y-vEllipse[nJ].center.y)<=30)
			{
				dAngle = abs(vEllipse[nI].angle - vEllipse[nJ].angle);
				while (dAngle > 180)
				{
					dAngle -= 180;
				}	
					
				if (dAngle < T_ANGLE_THRE || 180 - dAngle < T_ANGLE_THRE )
				{
					armor.center.x = (vEllipse[nI].center.x + vEllipse[nJ].center.x) / 2; //装甲片中心x
					armor.center.y = (vEllipse[nI].center.y + vEllipse[nJ].center.y) / 2; //装甲片中心y
					armor.angle = (vEllipse[nI].angle + vEllipse[nJ].angle) / 2;   //装甲片角度
					if (180 - dAngle < T_ANGLE_THRE)
					{
						armor.angle += 90;
					}
					nL = (vEllipse[nI].size.height + vEllipse[nJ].size.height) / 2; //高度
					nW = sqrt((vEllipse[nI].center.x - vEllipse[nJ].center.x) * (vEllipse[nI].center.x - vEllipse[nJ].center.x) + (vEllipse[nI].center.y - vEllipse[nJ].center.y) * (vEllipse[nI].center.y - vEllipse[nJ].center.y)); //宽度
					if (nL < nW)
					{
						armor.size.height = nL;
						armor.size.width = nW;
					}
					else
					{
						armor.size.height = nW;
						armor.size.width = nL;
					}
					vRlt.push_back(armor); 
				}
			}
		}
	}
    return vRlt;
}

void drawBox(RotatedRect box, Mat img)
{
    Point2f pt[4];
	Point center;
    int i;
    for (i = 0; i<4; i++)
    {
        pt[i].x = 0;
        pt[i].y = 0;
    }
    box.points(pt); //计算盒子顶点
	
	// cout<<"左上："<<pt[3]<<","<<"左下："<<pt[2]<<","<<"右下："<<pt[1]<<","<<"右上："<<pt[0]<<endl;
    line(img, pt[0], pt[1], Scalar(0,255,0), 2, 8, 0);
    line(img, pt[2], pt[3], Scalar(0,255,0), 2, 8, 0);
	line(img, pt[0], pt[2], Scalar(191,241,231), 2, 8, 0);
	line(img, pt[3], pt[1], Scalar(255,255,0), 2, 8, 0);
	circle(img,box.center,2,Scalar(0,255,0),4,8,0);
}

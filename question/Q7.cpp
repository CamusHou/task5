#include "get_and_show_picture.h"

int main()
{
    //1.读取图像
    cv::Mat img = get_picture("img/imori.jpg");
    if(img.empty()) return -1;

    //2.计算池化后的尺寸
    int out_w = img.cols/2;
    int out_h = img.rows/2;

    //3.创建结果图
    cv::Mat result(out_h, out_w , CV_8UC3);

    //4.遍历后每个池化后的格子
    for(int y = 0 ; y<out_h ; y++)
    {
        for(int x=0 ; x<out_w ; x++)
        {
            //4.1 取2*2的四个像素
            cv::Vec3b p00 = img.at<cv::Vec3b>(2 * y , 2 * x);
            cv::Vec3b p01 = img.at<cv::Vec3b>(2 * y , 2 * x+1);
            cv::Vec3b p10 = img.at<cv::Vec3b>(2 * y+1 , 2 * x);
            cv::Vec3b p11 = img.at<cv::Vec3b>(2 * y+1 , 2 * x+1);

            //4.2对每个通道取平均值
            cv::Vec3b avg;
            for(int c = 0; c < 3 ; c++)
            {
                avg[c] = static_cast<unsigned char>((p00[c]+p01[c]+p10[c]+p11[c])/4);
            }

            //4.3 写入结果图
            result.at<cv::Vec3b>(y,x) = avg;
        }
    }
    //5.显示
    show_picture("averagr_pooling",result);

    return 0;
}
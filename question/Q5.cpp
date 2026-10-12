#include "get_and_show_picture.h"
#include <vector>
#include <algorithm>
#include <cmath>

int main()
{
    //1.读取图像
    cv::Mat img = get_picture("img/imori.jpg");
    if(img.empty()) return -1;

    //2.创建HSV结果图（三通道，每个通道0～255）
    cv::Mat hsv = img.clone();

    //3.遍历每个元素
    for (int y = 0 ; y<img.rows ; y++)
    {
        for (int x = 0 ; x<img.cols ; x++)
        {
            cv::Vec3b bgr = img.at<cv::Vec3b>(y,x);
            //归一到[0,1]
            float b = bgr[0] /255.0f;
            float g = bgr[1] /255.0f;
            float r = bgr[2] /255.0f;

            //求最大值，最小值和差值
            float cmax = std::max({r,g,b});
            float cmin = std::min({r,g,b});
            float delta = cmax - cmin;

            //算H
            float h = 0.0f;
            if (delta != 0.0f)
            {
                if (cmax == r)
                {
                    h = 60.0f * fmod((g-b)/delta,6.0f);
                }
                else if (cmax == g)
                {
                    h = 60.0f * ((b-r)/delta + 2.0f);
                }
                else 
                {
                    h = 60.0f * ((r-g)/delta + 4.0f);
                }
                if (h<0) h += 360.0f;
            }

            //算S
            float s = (cmax == 0.0f) ? 0.0f : (delta/cmax);//条件 ？ 条件为真时的值 ： 条件为假时的值

            //算V
            float v = cmax;

            //转回0~255
            hsv.at<cv::Vec3b>(y,x)[0] = static_cast<unsigned char>(h / 2.0f);
            hsv.at<cv::Vec3b>(y,x)[1] = static_cast<unsigned char>(s * 255.0f);
            hsv.at<cv::Vec3b>(y,x)[2] = static_cast<unsigned char>(v * 255.0f);
            /*
            H：标准范围是 0~360，但 unsigned char 最大 255。所以 OpenCV 把 H 压缩到 0~179。转回时是 h / 2.0f。

            S：标准范围是 0~1，要映射到 0~255，所以是 s * 255.0f。

            V：标准范围也是 0~1，映射到 0~255，所以是 
            */
        }
    }
    //4.分离通道
    std::vector<cv::Mat> channels;
    cv::split(hsv,channels);


    std::cout << "准备显示照片..." << std::endl;
    //5.显示图片
    cv::imshow("Hue",channels[0]);
    cv::imshow("Saturation",channels[1]);
    cv::imshow("Value",channels[2]);
    cv::waitKey(0);

    return 0;

}
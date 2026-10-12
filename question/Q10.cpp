#include "get_and_show_picture.h"
#include <vector>
#include <algorithm>

int main()
{
    //1.获取图片
    cv::Mat img = get_picture("img/imori_noise.jpg");
    if(img.empty()) return-1;

    //2.创建补零图像
    cv::Mat padded = cv::Mat::zeros(img.rows + 2,img.cols + 2,CV_8UC3);
    for(int y=0 ; y<img.rows ; y++)
    {
        for(int x=0 ; x<img.cols ; x++)
        {
            padded.at<cv::Vec3b>(y+1,x+1) = img.at<cv::Vec3b>(y,x);
        }
    }

    //3.结果图像
    cv::Mat result = img.clone();

    //4.遍历每一个像素
    for(int y=0 ; y<img.rows ; y++)
    {
        for(int x=0 ; x<img.cols ;x++)
        {
            //4.1.对于三通道做中值滤波
            for(int c=0 ; c<3 ; c++)
            {
                //4.2.收集3*3领域9个数值
                std::vector<unsigned char>values;
                for(int ky=0 ; ky<3 ; ky++)
                {
                    for(int kx=0 ; kx<3 ; kx++)
                    {
                        unsigned char pixel = padded.at<cv::Vec3b>(y,x)[c];
                        values.push_back(pixel);
                    }
                }
            
                //4.3.对9个数值进行排序
                std::sort(values.begin(),values.end());

                //4.4.取中值（第五个数据，下标为4）
                result.at<cv::Vec3b>(y,x)[3] = values[4];

            }
        }
    }

    //5.显示结果
    show_picture("Median",result);

    return 0;
}
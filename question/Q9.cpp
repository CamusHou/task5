#include "get_and_show_picture.h"
#include <vector>

int main()
{
    //1.读取图像
    cv::Mat img = get_picture("img/imori_noise.jpg");
    if(img.empty()) return -1;

    //2.高斯核（3*3,总和16）
    int kernel[3][3] = 
    {
        {1,2,1},
        {2,4,2},
        {1,2,1},
    };
    int kernel_sum = 16;

    //3.创建补零后的图(尺寸+2)
    cv::Mat padded = cv::Mat::zeros(img.rows + 2,img.cols + 2,CV_8UC3);

    //4.把原图复制到中间
    for(int y=0 ; y<img.rows ; y++)
    {
        for(int x=0 ; x<img.cols ; x++)
        {
            padded.at<cv::Vec3b>(y + 1,x + 1) = img.at<cv::Vec3b>(y,x);
        }
    }

    //5.创建结果图
    cv::Mat result = img.clone();

    //6.遍历原图的每个像素（包括最外圈）
    for(int y=0 ; y<img.rows ;y++)
    {
        for(int x=0; x<img.cols ;x++)
        {
            //6.1.遍历三个通道
            for(int c=0 ; c<3 ; c++)
            {
                int sum = 0;

                //6.2.在补零图上取3*3领域
                for (int ky=0 ; ky<3 ; ky++)
                {
                    for(int kx=0 ; kx<3 ; kx++)
                    {
                        int pixel = padded.at<cv::Vec3b>(y+ky,x+kx)[c];
                        sum += pixel * kernel[ky][kx];
                    }
                }

        //6.3 除以核总和，写回结果
        result.at<cv::Vec3b>(y,x)[c] = static_cast<unsigned char>(sum / kernel_sum);
        
            }
        }
    }
    


    //7.显示图片
    show_picture("Gaussian",result);
    return 0; 
}


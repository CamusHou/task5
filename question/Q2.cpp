#include "get_and_show_picture.h"


/*
0------B
1------G
2------R
*/
/*
灰度的计算式子
Y = 0.2126R + 0.7152G + 0.0722B
*/

int main()
{
    //获取图像
    cv::Mat img = get_picture("img/imori.jpg");
    if (img.empty()) return -1;

    //复制图片
    cv::Mat result = img.clone();

    //遍历每一个元素
    for(int y = 0; y<img.rows ; y++)
    {
        for(int x = 0; x<img.cols ; x++)
        {
            cv::Vec3b pixel = img.at<cv::Vec3b>(y,x);//获取原图片的BGR值
            unsigned char gray = static_cast<unsigned char>(0.2126 * pixel[0] + 0.7152 * pixel[1] + 0.0722 * pixel[2]);

            result.at<cv::Vec3b>(y,x)[0] = gray;
            result.at<cv::Vec3b>(y,x)[1] = gray;
            result.at<cv::Vec3b>(y,x)[2] = gray;
        }
    }

    //获取图片
    show_picture("Result", result);
    while (cv::waitKey(30) != 27){};

    return 0;
}








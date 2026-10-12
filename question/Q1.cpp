#include "get_and_show_picture.h"
#include <iostream>
#include <opencv2/opencv.hpp>

//通道交换，大多数显示器是RGB但是opencv里的默认顺序是BGR，要调换R和B的顺序
//    0------B
//    1------G
//    2------R

int main() {
    //读取图片,调用创建的get_picture函数
    cv::Mat img = get_picture("img/imori.jpg");
    if (img.empty()) return -1 ;
    


    //2.创建一个和原图大小一样的新图
    cv::Mat result = img.clone();

    //3.遍历每一个元素，并且实现BR的换位
    for(int y = 0; y < img.rows; y++) 
    {//每一行，从0递增，rows是行数
        for(int x = 0; x < img.cols ; x++)
        {//每一列从0递增，cols是列数
            cv::Vec3b pixel = img.at<cv::Vec3b>(y,x); //取出原图中这个像素的BGR值 
            result.at<cv::Vec3b>(y,x)[0] = pixel[2];  //新图中的B---原图R
            result.at<cv::Vec3b>(y,x)[1] = pixel[1];  //新图中的G---原图G
            result.at<cv::Vec3b>(y,x)[2] = pixel[0];  //新图中的R---原图B
        }
    
    }

    //4.显示原图和交换后的图像
    //注意cv::imshow是按照BGR来解释数据的:读进来BGR--->原样传给imshow--->imshow按BGR理解--->颜色显示正常
    cv::imshow("Origin(BGR)",img);
    cv::imshow("New(RGB)",result);
    while (cv::waitKey(30) != 27){}

    //5.保存结果
    cv::imwrite("output_new_imori.jpg",result);

    return 0;
}
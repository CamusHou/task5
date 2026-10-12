#include "get_and_show_picture.h"
#include <iostream>


cv::Mat get_picture(const std::string& path)
{
    //获取图片
    cv::Mat img = cv::imread(path);
    if (img.empty())
    {
        std::cerr << "图像读取失败，请检查路径" << std::endl;
    }
    return img;
}

void show_picture(const std::string& window_name, const cv::Mat& img)
{
    std::cout << "准备显示:" << window_name << "尺寸：" << img.rows << "x" <<img.cols << std::endl;
    cv::imshow(window_name, img);
    cv::waitKey(0); //按任意键退出
    std::cout << "显示结束" << std:: endl;
}
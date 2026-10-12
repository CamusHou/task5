#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <string>

//获取并且最后输出图片
cv::Mat get_picture(const std::string& path);
void show_picture(const std::string& window_name,const cv::Mat& img);

#endif

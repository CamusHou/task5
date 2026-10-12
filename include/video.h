#ifndef VIDEO_H
#define VIDEO_H

#include <opencv2/opencv.hpp>
#include <string>

class Video{
    public:
        Video();
        ~Video();

        //打开视频文件
        bool open(const std::string& path);

        //获取下一帧
        bool getFrame(cv::Mat& frame);

        //关闭视频
        void close();
    private:
        cv::VideoCapture m_cap;//视频句柄
        bool m_isOpened = false;//标记是否打开
};

#endif
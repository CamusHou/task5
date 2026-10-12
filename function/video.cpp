#include "video.h"
#include <iostream>

Video::Video(){}

Video::~Video()
{
    close();
}

bool Video::open(const std::string& path)
{
    m_isOpened = m_cap.open(path);
    if(!m_isOpened)
    {
        std::cerr << "打开视频失败" << path << std::endl;
    }
    return m_isOpened;
}

bool Video::getFrame(cv::Mat& frame)
{
    if(!m_isOpened) return false;

    m_cap >> frame;
    if(frame.empty()) return false;
    //视频结束
    return true;
}

void Video::close()
{
    if(m_isOpened)
    {
        m_cap.release();
        m_isOpened = false;
    }
}
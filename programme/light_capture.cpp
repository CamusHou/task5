#include "get_and_show_picture.h"
#include "video.h"
#include <vector>
#include <algorithm>

//筛选颜色===============================================================================
cv::Mat filter_color(const cv::Mat& frame,bool is_blue)
{
    //1.转化为HSV
    cv::Mat hsv;
    cv::cvtColor(frame,hsv,cv::COLOR_BGR2HSV);

    //2.筛选颜色
    cv::Mat mask;//用来装二值图
    if(is_blue)
    {
        //蓝色：H 100~124
        cv::inRange(hsv,cv::Scalar(100,43,46)/*HSV下限*/,cv::Scalar(124,255,255)/*HSV上限*/,mask);
    }
    else
    {
        //红色：H 0～10 或 156～180
        cv::Mat mask1,mask2;
        cv::inRange(hsv,cv::Scalar(0,43,46),cv::Scalar(10,255,255),mask1);
        cv::inRange(hsv,cv::Scalar(156,43,46),cv::Scalar(180,255,255),mask2);
        cv::bitwise_or(mask1,mask2,mask);//把两个输出的mask合并
    }

    //3.形态学处理（这里选择膨胀，可以让灯条更加连贯）
    cv::Mat kernel = cv::getStructuringElement/*创建一个结构元素*/(cv::MORPH_RECT,cv::Size(3,3));
    cv::dilate(mask,mask,kernel);//膨胀

    return mask;
}

//处理单帧=================================================================================
cv::Mat process_frame(const cv::Mat& frame,bool is_blue)
{
    //1.颜色筛选
    cv::Mat mask = filter_color(frame,is_blue);
    //2.找轮廓
    std::vector<std::vector<cv::Point>>contours;
    cv::findContours(mask,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    //找选候选灯条
    std::vector<cv::RotatedRect>light_bars;
    for(const auto& contour : contours)
    {
        cv::RotatedRect rect = cv::minAreaRect(contour);

        float long_side = std::max(rect.size.width,rect.size.height);
        float short_side = std::min(rect.size.width,rect.size.height);
        float ratio = long_side /short_side;
        float area = long_side * short_side;

        //3.1用长边方向向量判断竖直
        cv::Point2f pts[4];//声明一个长度为4的数组，放入元素类型为cv::Point2f的结构体
        rect.points(pts);
        //算每个点之间的距离
        float len01 = cv::norm(pts[0]-pts[1]);
        float len12 = cv::norm(pts[1]-pts[2]);
        float len23 = cv::norm(pts[2]-pts[3]);
        float len30 = cv::norm(pts[3]-pts[0]);
        //找出最长边，得到最长边的方向向量
        cv::Point2f long_dir;
        if(len01 >= len12 && len01 >= len23 && len01 >=len30)
        {
            long_dir = pts[1]-pts[0];
        }
        else if(len12 >= len23 &&len12 >= len30)
        {
            long_dir = pts[2]-pts[1];
        }
        else if(len23 >= len30)
        {
            long_dir = pts[3]-pts[2];
        }
        else
        {
            long_dir = pts[0]-pts[3];
        }
        
        //长边与竖直方向的夹角为（0 =完全竖直，90 =完全水平）
        float angle_deg = std::atan2(std::abs(long_dir.x),std::abs(long_dir.y)) * 180.0f /CV_PI;
        //灯条基本筛选条件
        if(ratio > 0.8f && ratio <2.5f && area>0 && area<1000 && angle_deg <40.0f)
        {
            light_bars.push_back(rect);
        }

    }
    //4.两两配对
    std::vector<cv::Rect>current_armors;//装这一帧检测的装甲板容器
    cv::Mat result = frame.clone();

    for(int i=0 ; i<(int)light_bars.size() ; i++)
    {
        for(int j=i+1 ; j<(int)light_bars.size() ; j++)
        {
            cv::RotatedRect A = light_bars[i];
            cv::RotatedRect B = light_bars[j];

            //4.1.长度相近
            float lenA = std::max(A.size.width,A.size.height);
            float lenB = std::max(B.size.width,B.size.height);
            if (std::abs(lenA-lenB)/std::max(lenA,lenB)>0.3f) continue;
            //4.2.距离合适 中心距在灯条的0.5-5.0之间
            float dist = cv::norm(A.center - B.center);
            if (dist < lenA*0.5f || dist>lenA*2.0f) continue;
            //4.3.高度相似
            if(std::abs(A.center.y - B.center.y)>lenA*0.5f) continue;
            //4.4宽度相似：短边长度差不超过50%
            float wA = std::min(A.size.width,A.size.height);
            float wB = std::min(B.size.width,B.size.height);
            if(std::abs(wA-wB)/std::max(wA,wB)>0.5f) continue;
            //4.5.配对成功,画两根灯棒
            cv::Point2f points[4];
            A.points(points);
            for (int k=0 ; k<4 ; k++) cv::line(result,points[k],points[(k+1)%4],cv::Scalar(0,255,0),2);
            B.points(points);
            for (int k=0 ; k<4 ; k++) cv::line(result,points[k],points[(k+1)%4],cv::Scalar(0,255,0),2);
            //4.6.画出装甲板外框
            cv::Point2f armor_center = (A.center + B.center) /2.0f;
            float armor_width = cv::norm(A.center - B.center);
            float armor_height = std::max(lenA,lenB);
            cv::Rect armor_rect(
                (int)armor_center.x - armor_width /2 ,
                (int)armor_center.y - armor_height /2,
                (int)armor_width,
                (int)armor_height
            );
            current_armors.push_back(armor_rect);

        }
    }

    //5.掉帧处理：若是这一帧没有检测到，沿用上一帧的结果
    static std::vector<cv::Rect> last_armors;//用static，这是一个静态变量。函数结束后不销毁，下次调用依然还在
    static int lost_count = 0;

    if(current_armors.empty())
    {
        lost_count++;
        if(lost_count<10)
        {
            //连续丢失小于十帧，沿用上一帧
            current_armors = current_armors;
        }
        else
        {
            //连续丢失超过十帧，清空
            last_armors.clear();
        }
    }
    else
    {
        last_armors = current_armors; //更新记录
        lost_count = 0; //重置丢失帧数
    }


    //画装甲板框
    for(const auto& rect : current_armors)
    {
        cv::rectangle(result,rect,cv::Scalar(0,255,255),2);
    }
    return result;
}


//主函数====================================================================================
int main(int argc,char** argv)
{

    //从命令行参数读取视频路径
    if (argc<2)
    {
        std::cerr << "用法：" << argv[0] <<"<视频路径>" << std::endl;
        return -1;
    }
    //1.创建视频对象
    Video video;

    //2.打开视频
    if(!video.open(argv[1]))
    {
        return -1;
    }

    //3.循环读帧
    cv::Mat frame;
    while(video.getFrame(frame))
    {
        //处理这一帧（true=筛蓝色，false=筛红色）
        cv::Mat result = process_frame(frame,true);

        //显示原图和处理结果
        cv::imshow("Original",frame);
        cv::imshow("Processed",result);
        
        //退出按ESC
        if(cv::waitKey(30) == 27) break;
    }

    //4.释放资源
    cv::destroyAllWindows();
    return 0;


}
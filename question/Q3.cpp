#include "get_and_show_picture.h"

int main()
{
    //1.获取图片
    cv::Mat img = get_picture("img/imori.jpg");
    if (img.empty()) return -1;

    //2.克隆图片
    cv::Mat gray = img.clone();

    //3.灰化图片
    for (int y=0 ; y<img.rows ; y++)
    {
        for (int x=0 ; x<img.cols ; x++)
        {
            cv::Vec3b pixel = img.at<cv::Vec3b>(y,x);//获取图像BGR值
            unsigned char gray_value = static_cast<unsigned char>(0.2126 * pixel[2] + 0.7152 * pixel[1] + 0.0722 *pixel[0]);

            gray.at<cv::Vec3b>(y,x)[0] = gray_value;
            gray.at<cv::Vec3b>(y,x)[1] = gray_value;
            gray.at<cv::Vec3b>(y,x)[2] = gray_value;
        }
    }

    //4.二值化图片
    cv::Mat binary = gray.clone();
    //4.1.设定阈值
    int threshold = 128;
    //4.2.遍历每个像素，做二值化
    for(int y = 0 ; y < gray.rows ; y++)
    {
        for(int x = 0 ; x < gray.cols ; x++)
        {
            cv::Vec3b pixel = gray.at<cv::Vec3b>(y,x);//读取一个像素的三通道（因为gray是三通道图）
            unsigned char gray_value = pixel[0];//因为三通道值相同，任取一个作为灰度值
            {
                if(gray_value > threshold)
                {
                    binary.at<cv::Vec3b>(y,x)[0] = 255;
                    binary.at<cv::Vec3b>(y,x)[1] = 255;
                    binary.at<cv::Vec3b>(y,x)[2] = 255;
                }
                else
                {
                    binary.at<cv::Vec3b>(y,x)[0] = 0;              
                    binary.at<cv::Vec3b>(y,x)[1] = 0;
                    binary.at<cv::Vec3b>(y,x)[2] = 0;
                }
            }
        }
    }

    show_picture("Result",binary);
    while (cv::waitKey(30) != 27){};

    return 0;

}/*那我有些不理解了，我的遍历方式就是行和列，按照你的说法是要遍历字节而非像素，那我不应该还要在同一个字节的三个高度分别遍历一遍吗，我不应该在x里面再嵌套一个for循环吗。而且如果从三维的角度而言，我的遍历是二维的，照理代码要么跑不了，要么随机挑选一个通道全部遍历一遍，最终的结果也不应是左三分之一二值化，右边三分之二灰度化



你这个疑惑非常合理，而且说明你在**认真思考内存布局**。我来把它彻底讲清楚。

---

### 🎯 先纠正一个概念：“行 × 列”遍历的到底是什么？

在 OpenCV 里，`img.rows` 是**图像的行数**（高度），`img.cols` 是**图像的列数**（宽度）。

**`img.rows × img.cols` = 图像的总像素数。**

所以你的双层循环：
```cpp
for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
```
遍历的是**每一个像素**。**不是每一个字节，也不是每一个通道。**

**每个像素里装着 3 个通道（B、G、R）**，所以你拿到一个像素后，再通过 `[0]`、`[1]`、`[2]` 去访问它的三个通道。

**不需要再嵌套一层循环。** 因为 `[0]`、`[1]`、`[2]` 是三个固定的位置，不是“一组需要循环的东西”。

---

### 🎯 那为什么“错着写”会出现“左边三分之一二值化”？

关键在于：**`at<unsigned char>(y, x)` 和 `at<cv::Vec3b>(y, x)` 算出来的地址不一样。**

**假设图像宽 100，高 100，每个像素 3 字节。**

**`at<cv::Vec3b>(y, x)` 算的地址是：**
```
基地址 + (y × 100 + x) × 3
```
这是“第 x 个像素”的起始地址。

**`at<unsigned char>(y, x)` 算的地址是：**
```
基地址 + (y × 100 + x) × 1
```
这是“第 x 个字节”的起始地址，**不是第 x 个像素**。

因为每个像素占 3 个字节，所以 `at<unsigned char>` 走的步长只有 1 字节，而 `at<cv::Vec3b>` 走的步长是 3 字节。

**这导致：**
*   当 `x` 从 0 到 99 变化时，`at<unsigned char>` 访问的地址只覆盖了**前 100 个字节**，而一行实际有 300 个字节。
*   也就是说，`at<unsigned char>` 只访问了**第一行前 1/3 的像素（准确说是前 1/3 的字节）**。

**所以，当你用 `at<unsigned char>` 遍历整张图时，你实际上只访问了整张图前 1/3 的字节，后面 2/3 从未被写到。** 这就是“左 1/3 变了，右 2/3 没变”的根本原因。

---

### 🎯 你不需要嵌套三层循环

因为你已经通过 `at<cv::Vec3b>(y, x)` 拿到了**一个像素**，而这个像素内部有三个通道，用 `[0]`、`[1]`、`[2]` 直接访问就行。

**不需要再 `for (int c = 0; c < 3; c++)` 遍历通道。**

如果你想写成三层循环，也可以：
```cpp
for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
        for (int c = 0; c < 3; c++) {
            // 访问 img.at<cv::Vec3b>(y, x)[c]
        }
    }
}
```
但这是多余的，因为三个通道是固定的，直接写三行更清晰。

---

### 📌 一句话总结

*   **`rows × cols` 遍历的是像素，不是字节。**
*   **`at<cv::Vec3b>(y, x)` 是像素级访问，`at<unsigned char>(y, x)` 是字节级访问。** 三通道图只能用前者。
*   你不需要嵌套三层循环，因为一个像素的三个通道是固定的，用 `[0]`、`[1]`、`[2]` 直接访问。
*   **错位导致“左 1/3 变了”，是因为 `at<unsigned char>` 的步长是 1 字节，而像素是 3 字节。**

**你现在把二值化循环改成 `at<cv::Vec3b>` 的写法，就能得到完整的黑白图。** 去改吧，有问题再问我。*/
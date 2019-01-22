#include "image.h"
#include <cmath>

//灰度图像自适应二值化
std::vector<std::vector<uint8_t>>
threshold(std::vector<std::vector<uint8_t>> &image,
          image_attribute_t &image_attribute)
{
    std::vector<int> colour, colour_CDF, colour_map;
    colour.resize(256);
    colour_CDF.resize(256);
    int colour_threshold = 127; //二值化阈值
    //计算灰度直方图
    for (int i = 0; i < image_attribute.image_heigh; i++)
    {
        for (int j = 0; j < image_attribute.image_width; j++)
        {
            colour[image[i][j]] += 1;
        }
    }

    //计算灰度直方图累积分布函数
    colour_CDF[0] = colour[0];
    int colour_CDF_min;
    int frequency_threshold = 5; //灰度频率阈值
    for (int i = 1; i < 256; i++)
    {
        colour_CDF[i] = colour_CDF[i - 1] + colour[i];
    }
    for (int i = 5; i < 256; i++)
    {
        if (colour_CDF[i] != 0 && colour[i] >= frequency_threshold)
        {
            colour_CDF_min = i;
            break;
        }
    }
    int colour_diff = 256;
    int selected_colour;
    for (int i = 0; i < 256; i++)
    {
        if (colour[i] != 0 && (colour_CDF[i] - colour_CDF_min) >= 0)
        {
            colour_map.push_back((int)round(
                (colour_CDF[i] - colour_CDF_min) /
                (double)((image_attribute.image_width * image_attribute.image_heigh) -
                         colour_CDF_min) *
                255));
            // if ((colour_map[i] - colour[i]) <= colour_diff) {
            //   colour_threshold = i;
            // }
        }
        else
        {
            colour_map.push_back(0);
        }
    }

    for (int i = 0; i < image_attribute.image_heigh; i++)
    {
        for (int j = 0; j < image_attribute.image_width; j++)
        {
            image[i][j] = colour_map[image[i][j]];
            if (image[i][j] <= colour_threshold)
            {
                image[i][j] = 0x00;
            }
            else
            {
                image[i][j] = 0xFF;
            }
        }
    }

    return image;
}

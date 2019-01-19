#include "image.h"
#include <cmath>
#include <cstdint>
#include <vector>

image_attribute_t image_attribute;
centre_point_t centre_point;
circle_point_t circle_point;
data_fluctuation_t data_fluctuation;
angle_calc_point_t angle_calc_point;
location_point_t location_point;
rotation_angle_t rotation_angle;
int scan_radius = 80; //道路扫描半径
int road_width = image_attribute.image_heigh,
    real_width; //宽度的平方 单位为像素点

//灰度图像自适应二值化
std::vector<std::vector<uint8_t>>
threshold(std::vector<std::vector<uint8_t>> &image,
          image_attribute_t &image_attribute) {
  std::vector<int> colour, colour_CDF, colour_map;
  colour.resize(256);
  colour_CDF.resize(256);
  int colour_threshold = 127; //二值化阈值
  //计算灰度直方图
  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      colour[image[i][j]] += 1;
    }
  }

  //计算灰度直方图累积分布函数
  colour_CDF[0] = colour[0];
  int colour_CDF_min;
  int frequency_threshold = 5; //灰度频率阈值
  for (int i = 1; i < 256; i++) {
    colour_CDF[i] = colour_CDF[i - 1] + colour[i];
  }
  for (int i = 0; i < 256; i++) {
    if (colour_CDF[i] != 0 && colour[i] >= frequency_threshold) {
      colour_CDF_min = i;
      break;
    }
  }
  int colour_diff = 256;
  int selected_colour;
  for (int i = 0; i < 256; i++) {
    if (colour[i] != 0 && (colour_CDF[i] - colour_CDF_min) >= 0) {
      colour_map.push_back((int)round(
          (colour_CDF[i] - colour_CDF_min) /
          (double)((image_attribute.image_width * image_attribute.image_heigh) -
                   colour_CDF_min) *
          255));
      // if ((colour_map[i] - colour[i]) <= colour_diff) {
      //   colour_threshold = i;
      // }
    } else {
      colour_map.push_back(0);
    }
  }

  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      image[i][j] = colour_map[image[i][j]];
      if (image[i][j] <= colour_threshold) {
        image[i][j] = 0x00;
      } else {
        image[i][j] = 0xFF;
      }
    }
  }

  return image;
}

double angle_calc(centre_point_t &centre_point,
                  rotation_angle_t &rotation_angle,
                  std::vector<std::vector<uint8_t>> &image,
                  image_attribute_t image_attribute) {
  //计算所有计算圆周上的点
  std::vector<circle_point_t> circle;
  int i = 0;
  for (int x = centre_point.x - scan_radius; x <= centre_point.x + scan_radius;
       x++) {
    circle_point_t circle_cache;
    circle_cache.x = x;
    circle_cache.y = (int)round(
        sqrt(scan_radius * scan_radius - pow(x - centre_point.x, 2)));
    circle.push_back(circle_cache);
  }

  //找到所有跳变点，存入fluctuation中
  std::vector<data_fluctuation_t>
      fluctuation; // white-to-black = false ; black-to-white =true;
  for (i = 0; i < circle.size() - 1; i++) {
    data_fluctuation_t fluctuation_cache;
    if (circle[i].x < image_attribute.image_width &&
        circle[i + 1].x < image_attribute.image_width &&
        circle[i].y < image_attribute.image_heigh &&
        circle[i + 1].y < image_attribute.image_heigh) {
      if (image[circle[i].y][circle[i].x] !=
          image[circle[i + 1].y][circle[i + 1].x]) {
        fluctuation_cache.x = circle[i].x;
        fluctuation_cache.y = circle[i].y;
        if (image[circle[i].y][circle[i].x] == 0xFF) {
          fluctuation_cache.changetype = false;
        } else {
          fluctuation_cache.changetype = true;
        }
        fluctuation.push_back(fluctuation_cache);
      }
    }
  }

  //道路识别
  int selected_points; //选定第一个点（靠左侧）
  int width_diff = image_attribute.image_width;
  int width_diff_cache;
  for (i = 0; i < (int)(fluctuation.size() - 1); i++) {
    if (fluctuation[i].changetype == false &&
        fluctuation[i + 1].changetype == true) {
      real_width =
          (int)round(pow((fluctuation[i].x - fluctuation[i + 1].x), 2) +
                     pow((fluctuation[i].y - fluctuation[i + 1].y), 2));
      width_diff_cache = abs(road_width - real_width);
      if (width_diff_cache <= width_diff) {
        selected_points = i;
        width_diff = width_diff_cache;
      }
    }
  }

  if (road_width == 0)
    road_width = real_width;
  else
    road_width = 0.3 * road_width + 0.7 * real_width;

  //角度计算
  double angle;
  if (fluctuation.size() < 2) {
    return 0;
  } else {
    angle_calc_point.x =
        (fluctuation[selected_points].x + fluctuation[selected_points + 1].x) >>
        1;
    angle_calc_point.y =
        (fluctuation[selected_points].y + fluctuation[selected_points + 1].y) >>
        1;
    angle = atan((angle_calc_point.x - location_point.x) /
                 (angle_calc_point.y - location_point.y));
    centre_point.y = angle_calc_point.y;
    rotation_angle.total = angle;
  }
  return (angle - rotation_angle.now);
}

rotation_angle_t image_process(std::vector<std::vector<uint8_t>> image,
                               image_attribute_t &image_attribute) {

  centre_point.x = 100;
  centre_point.y = 0; //起始点
  rotation_angle.now = 0;

  image = threshold(image, image_attribute); //此处二值化 black:0xFF white:0x00

  rotation_angle.now =
      angle_calc(centre_point, rotation_angle, image, image_attribute);
  rotation_angle.next =
      angle_calc(centre_point, rotation_angle, image, image_attribute);
  return rotation_angle;
}

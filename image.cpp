#include "image.h"
#include <cmath>
#include <cstdint>
#include <vector>

image_attribute_t image_attribute;
point_t centre_point{
    .x = 100,
    .y = 0,
};
data_fluctuation_t data_fluctuation;
point_t angle_calc_point;
rotation_angle_t rotation_angle;
int scan_radius = 60; //道路扫描半径
int road_width = image_attribute.image_heigh;
double distortion_coefficient = 0.5;


double angle_calc(point_t &centre_point, rotation_angle_t &rotation_angle,
                  std::vector<std::vector<uint8_t>> &image,
                  image_attribute_t image_attribute) {
  //计算所有计算圆周上的点
  std::vector<point_t> circle;
  int i = 0;
  for (int x = centre_point.x - scan_radius; x <= centre_point.x + scan_radius;
       x++) {
    point_t circle_cache;
    circle_cache.x = x;
    int b, a = scan_radius; // a为椭圆长半轴，b为椭圆短半轴
    b = a * distortion_coefficient;
    if (b > image_attribute.image_heigh - 1) {
      b = image_attribute.image_heigh - 1;
    }
    circle_cache.y = (int)round(
        sqrt(b * b *
             (1 - (double)pow(abs(x - centre_point.x), 2) / (double)(a * a))));
    circle.push_back(circle_cache);
  }
  centre_point.y = image_attribute.image_heigh - 1 - centre_point.y;
  //找到所有跳变点，存入fluctuation中
  std::vector<data_fluctuation_t>
      fluctuation; // white-to-black = false ; black-to-white =true;
  for (i = 0; i < circle.size() - 1; i++) {
    data_fluctuation_t fluctuation_cache;
    if (circle[i].y >= 0 && circle[i + 1].y >= 0) {
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
  int real_width;      //宽度的平方 单位为像素点
  int selected_points; //选定第一个点（靠左侧）
  int width_diff = image_attribute.image_width;
  int width_diff_cache;
  for (i = 0; i < (int)(fluctuation.size() - 1); i++) {
    if (fluctuation[i].changetype == true &&
        fluctuation[i + 1].changetype == false) {
      real_width =
          (int)round(sqrt(pow((fluctuation[i].x - fluctuation[i + 1].x), 2) +
                          pow((fluctuation[i].y - fluctuation[i + 1].y), 2)));
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
    angle_calc_point.y = image_attribute.image_heigh - 1 - angle_calc_point.y;
centre_point.y = image_attribute.image_heigh - 1 - centre_point.y;    angle = atan((double)(angle_calc_point.x - centre_point.x) /
                 (double)(angle_calc_point.y - centre_point.y)) *
            180 / M_PI;
    centre_point.x = angle_calc_point.x;
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
  //第二次预测计算
  // rotation_angle.next =
  //     angle_calc(centre_point, rotation_angle, image, image_attribute);
  return rotation_angle;
}

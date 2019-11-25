#include "image.h"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace image_process {

/*
  function: Otsu method
  last edited: 2019/11/24 17:44
  edited by: Plectranthus_fg
  description: use Otsu method to get the binarizing threshold
*/
uint8_t otsu_threshold(std::vector<int> &histogram,
                       image_attribute_t &image_attribute) {
  int sumB = 0;
  int sum1 = 0;
  double wB = 0.0;
  double wF = 0.0;
  double mF = 0.0;
  double max_var = 0.0;
  double inter_var = 0.0;
  uint8_t threshold = 0;
  int index_histo = 0;
  int pixel_total = image_attribute.image_heigh * image_attribute.image_width;

  for (index_histo = 0; index_histo < histogram.size(); index_histo++) {
    sum1 += index_histo * histogram[index_histo];
  }

  for (index_histo = 0; index_histo < histogram.size(); index_histo++) {
    wB = wB + histogram[index_histo];
    wF = pixel_total - wB;
    if (wB == 0 || wF == 0) {
      continue;
    }
    sumB = sumB + index_histo * histogram[index_histo];
    mF = (sum1 - sumB) / wF;
    inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
    if (inter_var >= max_var) {
      threshold = index_histo;
      max_var = inter_var;
    }
  }

  return threshold;
}

/*
  function: Calculate frequency distribution histogram
  last edited: 2019/11/24 17:44
  edited by: Plectranthus_fg
*/
std::vector<int>
frequency_distribution_function(std::vector<std::vector<uint8_t>> &image,
                                image_attribute_t &image_attribute) {
  // calculate grayscale histogram
  std::vector<int> grayscale;
  grayscale.resize(256);
  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      grayscale[image[i][j]] += 1;
    }
  }
  return grayscale;
}

/*
  function: histogram equalization
  last edited: 2019/11/24 17:44
  edited by: Plectranthus_fg
*/
std::vector<std::vector<uint8_t>>
image_equalize(std::vector<std::vector<uint8_t>> &image,
               image_attribute_t &image_attribute) {
  std::vector<int> grayscale, grayscale_CDF, grayscale_map;
  grayscale.resize(256);
  grayscale_CDF.resize(256);
  int grayscale_threshold;

  grayscale = frequency_distribution_function(image, image_attribute);

  // calculate cumulative distribution function
  grayscale_CDF[0] = grayscale[0];
  int grayscale_CDF_min;
  for (int i = 1; i < 256; i++) {
    grayscale_CDF[i] = grayscale_CDF[i - 1] + grayscale[i];
  }

  for (int i = 5; i < 256; i++) {
    if (grayscale_CDF[i] != 0) {
      grayscale_CDF_min = i;
      break;
    }
  }

  // remap the value of grayscale
  double numberator =
      (image_attribute.image_width * image_attribute.image_heigh) -
      grayscale_CDF_min;
  for (int i = 0; i < 256; i++) {
    if (grayscale[i] != 0 && (grayscale_CDF[i] - grayscale_CDF_min) >= 0) {
      grayscale_map.push_back((int)round(
          (grayscale_CDF[i] - grayscale_CDF_min) / numberator * 255));
    } else {
      grayscale_map.push_back(0);
    }
  }

  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      image[i][j] = grayscale_map[image[i][j]];
    }
  }

  return image;
}
/*
  function: image binarize
  last edited: 2019/11/24 17:44
  edited by: Plectranthus_fg
  description: just convert origin image to binary images with provided
  threshold
*/
std::vector<std::vector<uint8_t>>
image_binarize(std::vector<std::vector<uint8_t>> &image,
               image_attribute_t &image_attribute, uint8_t threshold) {
  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      if (image[i][j] <= threshold) {
        image[i][j] = 0;
      } else {
        image[i][j] = 255;
      }
    }
  }
  return image;
}

/*
  function: angle calculate
  last edited: 2019/11/24 22:46
  edited by: Plectranthus_fg
  description: calculate the yaw, the yaw means the angle between the course
  angle and the midline of road
  !!! unusable, algorithm should be thoroughly checked !!!
*/
double angle_calc(point_t &centre_point, rotation_angle_t &rotation_angle,
                  std::vector<std::vector<uint8_t>> &image,
                  image_attribute_t image_attribute) {

  data_fluctuation_t data_fluctuation;
  point_t angle_calc_point;

  int scan_radius = 60; //道路扫描半径
  int road_width = image_attribute.image_heigh;
  /*
  the eccentricity of ellipse:
  e^2 = √(1-b^2/a^2)
  a > b
  0 < e < 1
  if e = 0, the ellipse become a circle
  */
  double eccentricity = 0.5;

  //计算所有计算圆周上的点
  // the default situation is that major axis is on the x axis
  int a_square, b_square; // a is major axis，b is minor axis
  a_square = pow(scan_radius, 2);
  b_square = a_square - a_square * eccentricity * eccentricity;
  int b_square_max = pow((image_attribute.image_heigh - 1), 2);
  if (b_square > b_square_max) {
    b_square = b_square_max;
  }

  std::vector<point_t> circle;
  int i = 0;
  for (int x = centre_point.x - scan_radius; x <= centre_point.x + scan_radius;
       x++) {
    point_t circle_cache;
    circle_cache.x = x;

    circle_cache.y = (int)round(
        sqrt(b_square *
             (1 - (double)pow(abs(x - centre_point.x), 2) / (double)a_square)));
    std::cout << circle_cache.x << " " << circle_cache.y << std::endl;
    circle.push_back(circle_cache);
  }
  centre_point.y = image_attribute.image_heigh - 1 - centre_point.y;

  //找到所有跳变点，存入fluctuation中
  std::vector<data_fluctuation_t>
      fluctuation; // white-to-black = false ; black-to-white =true;
  for (i = 0; i < circle.size() - 1; i++) {
    data_fluctuation_t fluctuation_cache;

    if (image[circle[i].y][circle[i].x] !=
        image[circle[i + 1].y][circle[i + 1].x]) {
      fluctuation_cache.coordinatel.x = circle[i].x;
      fluctuation_cache.coordinatel.y = circle[i].y;
      if (image[circle[i].y][circle[i].x] == 0xFF) {
        fluctuation_cache.changetype = false;
      } else {
        fluctuation_cache.changetype = true;
      }
      fluctuation.push_back(fluctuation_cache);
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
      real_width = (int)round(sqrt(
          pow((fluctuation[i].coordinatel.x - fluctuation[i + 1].coordinatel.x),
              2) +
          pow((fluctuation[i].coordinatel.y - fluctuation[i + 1].coordinatel.y),
              2)));
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

  std::cout << fluctuation[selected_points].coordinatel.x << " "
            << fluctuation[selected_points].coordinatel.y << std::endl;
  std::cout << fluctuation[selected_points + 1].coordinatel.x << " "
            << fluctuation[selected_points + 1].coordinatel.y << std::endl;
  //角度计算
  double angle;
  if (fluctuation.size() < 2) {
    return 0;
  } else {
    angle_calc_point.x = (fluctuation[selected_points].coordinatel.x +
                          fluctuation[selected_points + 1].coordinatel.x) >>
                         1;
    angle_calc_point.y = (fluctuation[selected_points].coordinatel.y +
                          fluctuation[selected_points + 1].coordinatel.y) >>
                         1;
    angle_calc_point.y = image_attribute.image_heigh - 1 - angle_calc_point.y;
    centre_point.y = image_attribute.image_heigh - 1 - centre_point.y;
    angle = atan((double)(angle_calc_point.x - centre_point.x) /
                 (double)(angle_calc_point.y - centre_point.y)) *
            180 / M_PI;
    centre_point.x = angle_calc_point.x;
    centre_point.y = angle_calc_point.y;
    rotation_angle.total = angle;
  }
  return (angle - rotation_angle.now);
}

/*
  function: image preprocess
  last edited: 2019/11/24 22:46
  edited by: Plectranthus_fg
  description: preprocess the image, included config the param of angle calc and
  binarize the image
  *** Should be revised after optimized angle calc!!! ***
*/
rotation_angle_t image_preprocess(std::vector<std::vector<uint8_t>> image,
                                  image_attribute_t &image_attribute) {

  point_t centre_point{
      .x = 100,
      .y = 0,
  }; //起始点
  rotation_angle_t rotation_angle{
      .now = 0,
  };
  std::vector<int> grayscale;
  grayscale = frequency_distribution_function(image, image_attribute);
  uint8_t threshold = otsu_threshold(grayscale, image_attribute);
  image_binarize(image, image_attribute, threshold);
  //此处二值化 black:0x00 white:0xFF

  rotation_angle.now =
      angle_calc(centre_point, rotation_angle, image, image_attribute);
  //第二次预测计算
  // rotation_angle.next =
  //     angle_calc(centre_point, rotation_angle, image, image_attribute);
  return rotation_angle;
}
} // namespace image_process
#include "image.h"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define filename "(1).txt"

int main() {
  image_attribute_t image_attribute;
  image_attribute.image_heigh = 40;
  image_attribute.image_width = 200;
  std::vector<std::vector<uint8_t>> image;
  std::ifstream image_txt(filename, std::ios::binary);
  rotation_angle_t rotation_angle;

  if (!image_txt.is_open()) {
    std::cout << "failed to open " << filename << '\n';
  }

  for (int i = 0; i < 40; i++) {
    std::vector<uint8_t> line;
    for (int j = 0; j < 200; j++) {

      std::string word;
      std::stringstream buf;
      int input_value;
      buf.clear();
      image_txt >> word;
      buf << std::hex << word;
      buf >> input_value;
      line.push_back(input_value);
    }
    image.push_back(line);
  }
  rotation_angle = image_process(image, image_attribute);
  std::cout << rotation_angle.now << '\n';

  // std::ofstream f_out("(1)threshold.txt");
  // for (int i = 0; i < 40; i++) {
  //   f_out << "[ ";
  //   for (int j = 0; j < 200; j++) {
  //     f_out << std::to_string(image[i][j]) << "\t";
  //   }
  //   f_out << "]" << std::endl;
  // }
  // image = threshold(image, image_attribute);
  //
  // /************************************************
  // 输出图像到标准输出流
  // ************************************************/
  //
  // for (int i = 0; i < 40; i++) {
  //   std::cout << "[ ";
  //   for (int j = 0; j < 200; j++) {
  //     std::cout << std::to_string(image[i][j]) << " ";
  //   }
  //   std::cout << "]" << std::endl;
  // }
  //
  // /************************************************
  // 输出图像到文件（x）fix.txt
  // ************************************************/
  //
  // for (int i = 0; i < 40; i++) {
  //   f_out << "[ ";
  //   for (int j = 0; j < 200; j++) {
  //     f_out << std::to_string(image[i][j]) << "\t";
  //   }
  //   f_out << "]" << std::endl;
  // }

  return 0;
}

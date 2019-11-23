#include "image.h"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define filename "picture1.txt"

int main() {
  image_attribute_t image_attribute{
    .image_heigh = 40,
    .image_width = 200,
  };
  std::vector<std::vector<uint8_t>> image;

  std::ifstream image_txt(filename, std::ios::binary);
  rotation_angle_t rotation_angle;

  if (!image_txt.is_open()) {
    std::cout << "failed to open " << filename << '\n';
  }

  // std::string str;
  // std::ofstream f2_out("image_txt.txt");
  // for (int i = 0; i < 86400; i++) {
  //   image_txt >> str;
  //   f2_out << str;
  // }

  for (int i = 0; i < image_attribute.image_heigh; i++) {
    std::vector<uint8_t> line;
    for (int j = 0; j < image_attribute.image_width; j++) {

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
  std::ofstream f_out("picture_fixed.txt");
  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      f_out << std::to_string(image[i][j]) << "\t";
    }
    f_out << std::endl;
  }

  image = image_process::image_equalize(image, image_attribute);

  std::ofstream f1_out("picture_threshold.txt");
  for (int i = 0; i < image_attribute.image_heigh; i++) {
    for (int j = 0; j < image_attribute.image_width; j++) {
      f1_out << std::to_string(image[i][j]) << "\t";
    }
    f1_out << std::endl;
  }
}

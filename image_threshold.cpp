#include "image.h"
#include <cmath>
#include <iostream>

namespace image_process{
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
    uint8_t index_histo = 0;
    int pixel_total = image_attribute.image_heigh * image_attribute.image_width;

    for (index_histo = 1; index_histo < 256; ++index_histo) {
      sum1 += index_histo * histogram[index_histo];
    }

    for (index_histo = 1; index_histo < 256; ++index_histo) {
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

  std::vector<int>
  frequency_distribution_function(std::vector<std::vector<uint8_t>> &image,
                                  image_attribute_t &image_attribute) {
    // calculate grayscale histogram
    std::vector<int> grayscale;
    for (int i = 0; i < image_attribute.image_heigh; i++) {
      for (int j = 0; j < image_attribute.image_width; j++) {
        grayscale[image[i][j]] += 1;
      }
    }
    return grayscale;
  }

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

  std::vector<std::vector<uint8_t>>
  image_threshold(std::vector<std::vector<uint8_t>> &image,
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
}
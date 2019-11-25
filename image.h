#include <cstdint>
#include <vector>
#include <cmath>

class ImageAttribute {
  public:
    int image_heigh_;
    int image_width_;
};

class Point {
  public:
    int x_;
    int y_;
};

class DataFluctuation {
  public:
    bool changetype_;
    Point coordinatel;
};

class RotationAngle {
  public:
    double now_;
    double next_;
    double total_;
};

namespace image_process {
// image preprocess
uint8_t OtsuMethod(std::vector<int> &histogram,
                       ImageAttribute &image_attribute);
std::vector<int>
FrequencyDistributionFunction(std::vector<std::vector<uint8_t>> &image,
                                ImageAttribute &image_attribute);
std::vector<std::vector<uint8_t>>
ImageEqualize(std::vector<std::vector<uint8_t>> &image,
               ImageAttribute &image_attribute);
std::vector<std::vector<uint8_t>>
ImageBinarize(std::vector<std::vector<uint8_t>> &image,
               ImageAttribute &image_attribute, uint8_t threshold);

} // namespace image_process

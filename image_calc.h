#include "image.h"

// image calculate
RotationAngle ImagePreprocess(std::vector<std::vector<uint8_t>>,
                               ImageAttribute &image_attribute);
    
double AngleCalc(Point &centre_point, RotationAngle &rotation_angle,
                  std::vector<std::vector<uint8_t>> &image,
                  ImageAttribute image_attribute);
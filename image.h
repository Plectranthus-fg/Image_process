#include <cstdint>
#include <vector>

extern int scan_radius;
extern int image_heigh, image_width;
extern std::vector<double> control_angle;


class image_attribute_t {
public:
  int image_heigh;
  int image_width;
};

class point_t {
public:
  int x;
  int y;
// template <typename T>
  // T assign_y(const T &a){
  //     y = image_attribute.image_heigh - 1 - a;
  // }
};


class data_fluctuation_t {
public:
  bool changetype;
  int x;
  int y;
};

class rotation_angle_t {
public:
  double now;
  double next;
  double total;
};

rotation_angle_t
image_process(std::vector<std::vector<uint8_t>> , image_attribute_t &image_attribute);
std::vector<std::vector<uint8_t>>
threshold(std::vector<std::vector<uint8_t>> &image , image_attribute_t &image_attribute);

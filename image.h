#include <cstdint>
#include <vector>

extern int scan_radius;
extern int image_heigh, image_width;
extern std::vector<double> control_angle;

struct image_attribute_t {
  int image_heigh;
  int image_width;
};

class centre_point_t {
public:
  int x;
  int y;
};

class circle_point_t {
public:
  int x;
  int y;
};

class data_fluctuation_t {
public:
  bool changetype;
  int x;
  int y;
};

class angle_calc_point_t {
public:
  int x;
  int y;
};

class location_point_t {
public:
  int x;
  int y;
};

class rotation_angle_t {
public:
  double now;
  double next;
  double total;
};

rotation_angle_t image_process(std::vector<std::vector<uint8_t>>);
std::vector<std::vector<uint8_t>>
threshold(std::vector<std::vector<uint8_t>> &image , image_attribute_t &image_attribute);

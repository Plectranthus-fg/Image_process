#include "image_calc.h"
#include <iostream>

/*
    function: image preprocess
    last edited: 2019/11/24 22:46
    edited by: Plectranthus_fg
    description: preprocess the image, included config the param of angle calc
   and binarize the image
    *** Should be revised after optimized angle calc!!! ***
*/
RotationAngle ImagePreprocess(std::vector<std::vector<uint8_t>> image,
                              ImageAttribute &image_attribute) {

    Point centre_point{
        .x_ = 100,
        .y_ = 0,
    }; //起始点
    RotationAngle rotation_angle{
        .now_ = 0,
    };
    std::vector<int> grayscale;
    grayscale =
        image_process::FrequencyDistributionFunction(image, image_attribute);
    uint8_t threshold = image_process::OtsuMethod(grayscale, image_attribute);
    image_process::ImageBinarize(image, image_attribute, threshold);
    //此处二值化 black:0x00 white:0xFF

    rotation_angle.now_ =
        AngleCalc(centre_point, rotation_angle, image, image_attribute);
    //第二次预测计算
    // rotation_angle.next_ =
    //     AngleCalc(centre_point, rotation_angle, image, image_attribute);
    return rotation_angle;
}

/*
    function: angle calculate
    last edited: 2019/11/24 22:46
    edited by: Plectranthus_fg
    description: calculate the yaw, the yaw means the angle between the course
    angle and the midline of road
    !!! unusable, algorithm should be thoroughly checked !!!
*/
double AngleCalc(Point &centre_point, RotationAngle &rotation_angle,
                 std::vector<std::vector<uint8_t>> &image,
                 ImageAttribute image_attribute) {
    DataFluctuation data_fluctuation;
    Point angle_calc_point;

    int scan_radius = 60; //道路扫描半径
    int road_width = image_attribute.image_heigh_;
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
    int b_square_max = pow((image_attribute.image_heigh_ - 1), 2);
    if (b_square > b_square_max) {
        b_square = b_square_max;
    }

    std::vector<Point> circle;
    int i = 0;
    for (int x = centre_point.x_ - scan_radius;
         x <= centre_point.x_ + scan_radius; x++) {
        Point circle_cache;
        circle_cache.x_ = x;

        circle_cache.y_ = (int)round(
            sqrt(b_square * (1 - (double)pow(abs(x - centre_point.x_), 2) /
                                     (double)a_square)));
        std::cout << circle_cache.x_ << " " << circle_cache.y_ << std::endl;
        circle.push_back(circle_cache);
    }
    centre_point.y_ = image_attribute.image_heigh_ - 1 - centre_point.y_;

    //找到所有跳变点，存入fluctuation中
    std::vector<DataFluctuation>
        fluctuation; // white-to-black = false ; black-to-white =true;
    for (i = 0; i < circle.size() - 1; i++) {
        DataFluctuation fluctuation_cache;

        if (image[circle[i].y_][circle[i].x_] !=
            image[circle[i + 1].y_][circle[i + 1].x_]) {
            fluctuation_cache.coordinatel.x_ = circle[i].x_;
            fluctuation_cache.coordinatel.y_ = circle[i].y_;
            if (image[circle[i].y_][circle[i].x_] == 0xFF) {
                fluctuation_cache.changetype_ = false;
            } else {
                fluctuation_cache.changetype_ = true;
            }
            fluctuation.push_back(fluctuation_cache);
        }
    }

    //道路识别
    int real_width;      //宽度的平方 单位为像素点
    int selected_points; //选定第一个点（靠左侧）
    int width_diff = image_attribute.image_width_;
    int width_diff_cache;
    for (i = 0; i < (int)(fluctuation.size() - 1); i++) {
        if (fluctuation[i].changetype_ == true &&
            fluctuation[i + 1].changetype_ == false) {
            real_width =
                (int)round(sqrt(pow((fluctuation[i].coordinatel.x_ -
                                     fluctuation[i + 1].coordinatel.x_),
                                    2) +
                                pow((fluctuation[i].coordinatel.y_ -
                                     fluctuation[i + 1].coordinatel.y_),
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

    std::cout << fluctuation[selected_points].coordinatel.x_ << " "
              << fluctuation[selected_points].coordinatel.y_ << std::endl;
    std::cout << fluctuation[selected_points + 1].coordinatel.x_ << " "
              << fluctuation[selected_points + 1].coordinatel.y_ << std::endl;
    //角度计算
    double angle;
    if (fluctuation.size() < 2) {
        return 0;
    } else {
        angle_calc_point.x_ =
            (fluctuation[selected_points].coordinatel.x_ +
             fluctuation[selected_points + 1].coordinatel.x_) >>
            1;
        angle_calc_point.y_ =
            (fluctuation[selected_points].coordinatel.y_ +
             fluctuation[selected_points + 1].coordinatel.y_) >>
            1;
        angle_calc_point.y_ =
            image_attribute.image_heigh_ - 1 - angle_calc_point.y_;
        centre_point.y_ = image_attribute.image_heigh_ - 1 - centre_point.y_;
        angle = atan((double)(angle_calc_point.x_ - centre_point.x_) /
                     (double)(angle_calc_point.y_ - centre_point.y_)) *
                180 / M_PI;
        centre_point.x_ = angle_calc_point.x_;
        centre_point.y_ = angle_calc_point.y_;
        rotation_angle.total_ = angle;
    }
    return (angle - rotation_angle.now_);
}

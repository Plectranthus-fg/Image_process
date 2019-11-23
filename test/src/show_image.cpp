#include <opencv4/opencv2/opencv.hpp>
#include <iostream>

int main(){
    cv::Mat image(40,200,CV_8U);
    cv::Mat image_origin(40,200,CV_8U);
    std::ifstream img(
        "/run/media/storage/repos/Image_process/picture_threshold.txt");
    std::ifstream img_ori(
        "/run/media/storage/repos/Image_process/picture_fixed.txt");
    if (!img.is_open()) {
      std::cout << "failed to open "
                << "picture_threshold.txt" << '\n';
    }
    int value;
    for(int i = 0; i < 40; i++){
        for(int j = 0 ; j < 200; j++){
            img >> value;
            //std::cout << value << " " << i  << " " << j << std::endl;
            image.at<u_int8_t>(i,j) = value;
        }
    }


    for (int i = 0; i < 40; i++) {
      for (int j = 0; j < 200; j++) {
        img_ori >> value;
        // std::cout << value << " " << i  << " " << j << std::endl;
        image_origin.at<u_int8_t>(i, j) = value;
      }
    }
    cv::imwrite("pic_equalization_01.png", image);
    cv::imwrite("pic_origin.png", image_origin);
    return 0;
}
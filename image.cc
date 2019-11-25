#include "image.h"
#include <iostream>

namespace image_process {

/*
    function: Otsu method
    last edited: 2019/11/24 17:44
    edited by: Plectranthus_fg
    description: use Otsu method to get the binarizing threshold
*/
uint8_t OtsuMethod(std::vector<int> &histogram,
                       ImageAttribute &image_attribute) {
    int sumB = 0;
    int sum1 = 0;
    double wB = 0.0;
    double wF = 0.0;
    double mF = 0.0;
    double max_var = 0.0;
    double inter_var = 0.0;
    uint8_t threshold = 0;
    int index_histo = 0;
    int pixel_total =
        image_attribute.image_heigh_ * image_attribute.image_width_;

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
FrequencyDistributionFunction(std::vector<std::vector<uint8_t>> &image,
                                ImageAttribute &image_attribute) {
    // calculate grayscale histogram
    std::vector<int> grayscale;
    grayscale.resize(256);
    for (int i = 0; i < image_attribute.image_heigh_; i++) {
        for (int j = 0; j < image_attribute.image_width_; j++) {
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
ImageEqualize(std::vector<std::vector<uint8_t>> &image,
               ImageAttribute &image_attribute) {
    std::vector<int> grayscale, grayscale_CDF, grayscale_map;
    grayscale.resize(256);
    grayscale_CDF.resize(256);
    int grayscale_threshold;

    grayscale = FrequencyDistributionFunction(image, image_attribute);

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
        (image_attribute.image_width_ * image_attribute.image_heigh_) -
        grayscale_CDF_min;
    for (int i = 0; i < 256; i++) {
        if (grayscale[i] != 0 && (grayscale_CDF[i] - grayscale_CDF_min) >= 0) {
            grayscale_map.push_back((int)round(
                (grayscale_CDF[i] - grayscale_CDF_min) / numberator * 255));
        } else {
            grayscale_map.push_back(0);
        }
    }

    for (int i = 0; i < image_attribute.image_heigh_; i++) {
        for (int j = 0; j < image_attribute.image_width_; j++) {
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
ImageBinarize(std::vector<std::vector<uint8_t>> &image,
               ImageAttribute &image_attribute, uint8_t threshold) {
    for (int i = 0; i < image_attribute.image_heigh_; i++) {
        for (int j = 0; j < image_attribute.image_width_; j++) {
            if (image[i][j] <= threshold) {
                image[i][j] = 0;
            } else {
                image[i][j] = 255;
            }
        }
    }
    return image;
}
} // namespace image_process

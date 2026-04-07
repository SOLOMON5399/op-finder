#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main() {
    // Create a dummy image (100x100)
    Mat image = Mat::ones(100, 100, CV_8UC1) * 150;

    Mat blurred;

    // Apply blur
    blur(image, blurred, Size(3,3));

    std::cout << "Blur done\n";
    return 0;
}

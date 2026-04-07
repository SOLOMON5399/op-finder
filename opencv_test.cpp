#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("test.jpg");

    if (img.empty()) {
        std::cout << "Image not found\n";
        return -1;
    }

    cv::imshow("Image", img);
    cv::waitKey(0);

    return 0;
}

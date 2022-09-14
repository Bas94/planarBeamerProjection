#include <iostream>
#include <map>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>


char imageKey = '1';
int  pntIndex = 0;
bool showAnotations = true;
bool imageChanged = true;

std::map<char, cv::Mat3b> imgLib;
std::map<char, std::vector<cv::Point> > points;
std::map<char, cv::Scalar> colors {
    { '1', cv::Scalar(255, 0, 0) },
    { '2', cv::Scalar(0, 255, 0) },
    { '3', cv::Scalar(0, 0, 255) },
    { '4', cv::Scalar(255, 255, 0) },
    { '5', cv::Scalar(255, 0, 255) },
    { '6', cv::Scalar(0, 255, 255) },
    { '7', cv::Scalar(127, 127, 0) },
    { '8', cv::Scalar(127, 0, 127) },
    { '9', cv::Scalar(0, 127, 127) },
};

bool keyIsValid(char key) {
    return '1' <= key && key <= '9' && imgLib.find(key) != imgLib.end();
}

int nearestIndex(cv::Point queryPnt, std::vector<cv::Point> const & pnts, double& maxDist) {
    int index = -1;
    for(int i = 0; i < static_cast<int>(pnts.size()); ++i) {
        auto const & p = pnts[i];
        cv::Vec2i diff = queryPnt-p;
        double dist = sqrt(diff.ddot(diff));
        if( maxDist > dist ) {
            maxDist = dist;
            index = i;
        }
    }
    return index;
}

void mouseCb(int event, int x, int y, int flags, void* userdata) {
    static bool dragging = false;
    if( keyIsValid(imageKey) && event == cv::MouseEventTypes::EVENT_LBUTTONDOWN ) {
        if( points[imageKey].size() < 4 ) {
            points[imageKey].push_back(cv::Point2i(x,y));
            if(points[imageKey].size() == 4) {
                if( keyIsValid(imageKey+1) ) {
                    imageKey++;
                    pntIndex = -1;
                    imageChanged = true;
                    return;
                } else {
                    showAnotations = false;
                }
            }
        } else {
            double maxDist = 20;
            pntIndex = -1;
            for( char i = '1'; keyIsValid(i); ++i) {
                int index = nearestIndex(cv::Point2i(x,y), points[i], maxDist);
                if( index >= 0 ) {
                    pntIndex = index;
                    imageKey = i;
                }
            }
            if(pntIndex >= 0) {
                points[imageKey][pntIndex] = cv::Point2i(x,y);
            }
        }
        imageChanged = true;
        dragging = true;
    }
    if( dragging && pntIndex >= 0 ) {
        points[imageKey][pntIndex] = cv::Point2i(x,y);
        imageChanged = true;
    }
    if( keyIsValid(imageKey) && event == cv::MouseEventTypes::EVENT_LBUTTONUP && pntIndex >= 0 ) {
        dragging = false;
        imageChanged = true;
        points[imageKey][pntIndex] = cv::Point2i(x,y);
        if( points[imageKey].size() < 4 ) {
            pntIndex = (pntIndex + 1) % 4;
        }
    }

}

cv::Mat3b paintImage(int width, int height) {
    cv::Mat3b projectionImage = cv::Mat::zeros(cv::Size(width,height),CV_8UC3);
    for( auto const & entry : points) {
        std::vector<cv::Point> points = entry.second;
        char imgKey = entry.first;
        cv::Mat3b image = imgLib[imgKey];
        if( points.size() == 4 ) {
            std::vector<cv::Point> srcPnts {cv::Point2i(0,0), cv::Point2i(image.cols,0), cv::Point2i(image.cols,image.rows), cv::Point2i(0,image.rows)};
            cv::Mat homography = cv::findHomography(srcPnts, points);
            cv::Mat3b homImage(height, width);
            cv::warpPerspective(image, homImage, homography, cv::Size(projectionImage.cols, projectionImage.rows));
            projectionImage += homImage;
        }
        if( showAnotations ) {
            cv::polylines(projectionImage, points, true, colors[imgKey], 10);
            cv::polylines(projectionImage, points, true, cv::Scalar(255,255,255), 3);
            for( auto const &point : points ) {
                cv::rectangle(projectionImage, point - cv::Point(5,5), point + cv::Point(5,5), colors[imgKey], -1);
            }
        }

    }
    return projectionImage;
}

int main(int argc, char** argv)
{
    if( argc < 4 ) {
        std::cerr << "usage: " << argv[0] << " screenWidth screenHeight image1 [image2 ...]" << std::endl;
        return -1;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    for( int i = 3; i < std::min(argc, 11); ++i) {
        imgLib[('1' + i - 3)] = cv::imread(argv[i]);
        std::cout << "loaded image " << argv[i] << std::endl;
    }

    cv::namedWindow("beamerImage");
    cv::setMouseCallback("beamerImage", mouseCb);

    cv::Mat3b projectionImage;
    char key = 0;
    while(key != 27) { // not ESC?
        key = cv::waitKey(20);
        if( keyIsValid(key) ) {
            imageKey = key;
            pntIndex = 0;
            imageChanged = true;
        }

        if(key == ' ') {
            showAnotations = !showAnotations;
            imageChanged = true;
        }

        if(imageChanged) {
            imageChanged = false;
            projectionImage = paintImage(width, height);
            cv::imshow("beamerImage", projectionImage);
        }
    }

    showAnotations = false;
    projectionImage = paintImage(width, height);
    std::string projectedImageName = "projected_image.png";
    cv::imwrite(projectedImageName, projectionImage);
    std::cout << "saved image as " << projectedImageName << std::endl;

    return 0;
}

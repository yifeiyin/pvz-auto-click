#include "pvzsunautocollect.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <deque>

using namespace std;
using namespace cv;

enum CliclickAction { CLICK = 0, MOVE = 1 };
struct PositionAndTime {
    Point pos;  double time;
    PositionAndTime(Point _pos, double _time) : pos(_pos), time(_time) {}
};

Mat PvzSunAutoCollect::AutoResizeForDisplay(InputArray input, double perferedSideLength) {

    const double PERFERED_AREA = pow(perferedSideLength, 2);
    double currentArea = input.cols() * input.rows();
    double scalingFactor = sqrt(PERFERED_AREA / currentArea);
    Mat out;
    Size dstSize = Size(int(scalingFactor * input.cols()), int(scalingFactor * input.rows()));
    resize(input, out, dstSize, 0, 0, INTER_NEAREST);
    return out;
}
void PvzSunAutoCollect::imautoshow(const string& winname, const Mat& img, double perferedSideLength) {
    imshow(winname, AutoResizeForDisplay(img, perferedSideLength));
}
Mat PvzSunAutoCollect::_AutoCrop(const Mat& src) {
    assert(src.channels() == 1);
    assert(src.isContinuous());
    uchar * pixcels = src.data;
    int width = src.cols;
    int height = src.rows;

    int dtop = 0, dbottom = 0, dleft = 0, dright = 0;
    // dtop
    for (int i = 0; i < width * height; i++)
        if (pixcels[i] != 0) { dtop = i / width; break; }

    // dbottom
    for (int i = width * height - 1; i >= 0; i--)
        if (pixcels[i] != 0) { dbottom = height - i / width; break; }

    // dleft
    for (int col = 0; col < width; col++) {
        bool isEmptyFlag = true;
        for (int row = 0; row < height; row++)
            if (pixcels[row*width+col] != 0) { isEmptyFlag = false; break; }
        if (!isEmptyFlag) { dleft = col; break; }
    }

    // dright
    for (int col = width - 1; col >= 0; col--) {
        bool isEmptyFlag = true;
        for (int row = 0; row < height; row++)
            if (pixcels[row*width+col] != 0) { isEmptyFlag = false; break; }
        if (!isEmptyFlag) { dright = width - col; break; }
    }

    return Mat(src, Rect(dleft, dtop, width-dleft-dright, height-dtop-dbottom));
}
Mat PvzSunAutoCollect::AutoCrop(const Mat& src) {
    uint n = uint(src.channels());
    Mat *channels = new Mat [n];
    split(src, channels);
    Mat *resultChannels = new Mat [n];

    for (uint i = 0; i < n; i++) {
        resultChannels[i] = _AutoCrop(channels[i]);
    }

    Mat result;
    merge(resultChannels, size_t(n), result);

    delete [] channels;
    delete [] resultChannels;

    return result;
}
void PvzSunAutoCollect::MoveMouse(vector<Point> points, CliclickAction action, int delay) {
    if (points.size() == 0) return;
    // cliclick c:x,y w:200
    string commandPrefix, commandParameter;
    commandPrefix += "/usr/local/bin/cliclick";
    for (unsigned long i = 0; i < points.size(); i++) {
        int x = points[i].x; int y = points[i].y;
        if (action == MOVE)
            commandParameter += " m:";
        else if (action == CLICK)
            commandParameter += " c:";
        else
            assert(false);
        commandParameter += to_string(x);
        commandParameter += ",";
        commandParameter += to_string(y);
    }
    if (delay != 0) {
        commandParameter += " w:";
        commandParameter += to_string(delay);
    }
    cout << "cliclick"<< commandParameter << endl;
    system(String(commandPrefix+commandParameter).c_str());
}
void PvzSunAutoCollect::MoveMouse(Point point, CliclickAction action, int delay) {
    vector<Point> points;
    points.push_back(point);
    MoveMouse(points, action, delay);
}
Point PvzSunAutoCollect::GetCurrentMouseLocation() {
    string result = exec("/usr/local/bin/cliclick p:.");
    string xstring = "", ystring = "";
    unsigned long commaPos = result.find(",");
    for (unsigned long i = 0; i < commaPos; i++)
        xstring.push_back(result[i]);
    for (unsigned long i = commaPos + 1; i < result.length(); i++)
        ystring.push_back(result[i]);
    int x = stoi(xstring), y = stoi(ystring);
    return Point(x, y);
}
double PvzSunAutoCollect::GetCurrentTime() {
    const unsigned long CLOCK_PER_SEC = 1000000;
    double currentTime = double(clock()) / double(CLOCK_PER_SEC);
    return currentTime;
}
double PvzSunAutoCollect::GetTimeSince(double previousTime) {
    return GetCurrentTime() - previousTime;
}
void PvzSunAutoCollect::RecordMousePositionHistory() {
    PositionAndTime elementToPush (GetCurrentMouseLocation(), GetCurrentTime());
    mousePositionHistory.push_front(elementToPush);
    if (mousePositionHistory.size() > MOUSE_POSITION_HISTORY_MAX_SIZE) {
        mousePositionHistory.pop_back();
    }
}
double PvzSunAutoCollect::GetTimeForMouseStayedStillAtCurrentLocation() {
    Point currentPosition = GetCurrentMouseLocation();
    for (unsigned long i = 0; i < mousePositionHistory.size(); i++) {
        if (currentPosition != mousePositionHistory[i].pos)
            return GetCurrentTime() - mousePositionHistory[i].time;
    }
    return mousePositionHistory.back().time;
}

//
// PUBLIC
//
PvzSunAutoCollect::PvzSunAutoCollect() {
    namedWindow("imgMarked", WINDOW_AUTOSIZE);
    moveWindow("imgMarked", 800, 0);
    //    namedWindow("imgCropped", WINDOW_AUTOSIZE);
    //    moveWindow("imgCropped", 929, 480);
}

//====================

void PvzSunAutoCollect::Capture() {
    system("screencapture -R '0, 23, 799, 622' /Users/yinyifei/Developer/OpenCV-Projects/_IMGLIB/real-time-data/a.png");
    imgSrc.release();
    imgSrc = imread("/Users/yinyifei/Developer/OpenCV-Projects/_IMGLIB/real-time-data/a.png", IMREAD_COLOR);
    assert(imgSrc.data != nullptr);
    imgCropped.release();
    imgCropped = AutoCrop(imgSrc);
}

void PvzSunAutoCollect::Detect() {
    imgCvt.release();
    cvtColor(imgCropped, imgCvt, COLOR_BGR2HSV);

    _timeDetectionStarts = GetTimeSince(0);

    imgMarked.release();
    imgMarked = imgCropped;

    Mat maskInRange;
    const Scalar lowerb = Scalar(LOW_H, LOW_S, LOW_V);
    const Scalar upperb = Scalar(HIGH_H, HIGH_S, HIGH_V);
    inRange(imgCvt, lowerb, upperb, maskInRange);

    Mat maskOpening;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(kernelSize, kernelSize));
    kernel = Scalar(1);
    morphologyEx(maskInRange, maskOpening, MORPH_OPEN, kernel, Point(-1, -1), iterations);

    Mat maskOutlineEliminate;

    int w = imgMarked.cols;
    int h = imgMarked.rows;
    maskOutlineEliminate = getStructuringElement(MORPH_RECT, Size(imgMarked.cols, imgMarked.rows));
    maskOutlineEliminate = Scalar(0);
    Point pt1 (int(w * RATIO_LEFT), int(h * RATIO_TOP));
    Point pt2 (int(w - w * RATIO_RIGHT), int(h - h * RATIO_BOTTOM));
    Scalar color (255, 255, 255);
    rectangle(maskOutlineEliminate, pt1, pt2, color, -1);

    Mat maskCombined;
    bitwise_and(maskOutlineEliminate, maskOutlineEliminate, maskCombined, maskOpening);

    contours.clear();
    findContours(maskCombined, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    unsigned long contourLength = contours.size();

    rectsSun.clear();
    rectsNotSun.clear();
    for (unsigned long i = 0; i < contourLength; i++) {
        vector<Point> contour = contours[i];

        Rect boundingRectangle = boundingRect(contour);
        double ratio = boundingRectangle.width / boundingRectangle.height;
        bool isCorrectRatio = ratio > 0.9 && ratio < 1.1;

        bool exp1 = contourArea(contour) > 1000.0;
        bool exp2 = contourArea(contour) < 1500.0;
        bool isBigSun = exp1 && exp2 && isCorrectRatio;

        bool exp3 = contourArea(contour) > 250.0;
        bool exp4 = contourArea(contour) < 265.0;
        bool isSmallSun = exp3 && exp4 && isCorrectRatio;

        if (isBigSun || isSmallSun) {
            rectsSun.push_back(boundingRect(contours[i]));
        } else {
            rectsNotSun.push_back(boundingRect(contours[i]));
        }
    }

    numberOfSuns = rectsSun.size();
    numberOfNotSuns = rectsNotSun.size();

    // Calculate Locations from Rects
    sunLocations.clear();
    sunLocations = vector<Point> (numberOfSuns);
    for (unsigned long i = 0; i < numberOfSuns; i++) {
        sunLocations[i] = Point( rectsSun[i].x + rectsSun[i].width / 2, rectsSun[i].y + rectsSun[i].height / 2);
    }

    // Calculate Global Positions
    targetsGlobelPositions.clear();
    for (unsigned long i = 0; i < numberOfSuns; i++) {
        int targetPosX = sunLocations[i].x / 2;
        int targetPosY = sunLocations[i].y / 2 + TOP_OFFSET;
        targetsGlobelPositions.push_back(Point(targetPosX, targetPosY));
    }
}

void PvzSunAutoCollect::ExecuteMouseMoves() {
    Point previousMouseLocation = GetCurrentMouseLocation();
    MoveMouse(targetsGlobelPositions, CLICK, 0);
    MoveMouse(previousMouseLocation, MOVE, 0);
}


void PvzSunAutoCollect::Start() {

    double lastTimeDetected = 0.0;
    double lastTimeMouseMoveExecuted = 0.0;
    double lastTimeMousePositionRecorded = 0.0;

    double timeAtLastFrame = 0.0;
    int counter10Frames = 0;
    double timeAtLast10Frames = 0.0;
    double fps = 0.0;
    double fpsAvg = 0.0;
    double fpsAvgHistory = 0.0;

    while(true) {
        if (GetTimeSince(lastTimeMousePositionRecorded) > MOUSE_POSITION_RECORD_MIN_INTERVAL) {
            RecordMousePositionHistory();
            lastTimeMousePositionRecorded = GetCurrentTime();
        }

        if (GetTimeSince(lastTimeDetected) > DETECTION_TIME_INTERVAL) {
            Capture();
            Detect();
            lastTimeDetected = GetCurrentTime();
        }

        // Execute Mouse Moves
        bool areThereSuns = numberOfSuns > 0;
        bool satisfyTimeLimits = GetTimeSince(lastTimeMouseMoveExecuted) > EXECUTE_MOUSE_MOVES_MIN_INTERVAL;
        bool inTheExecutionRect = Rect(Point(17,50),Point(75,100)).contains(GetCurrentMouseLocation()) ||
                Rect(Point(0,0),Point(20,640)).contains(GetCurrentMouseLocation());
        bool isNotActive = GetTimeForMouseStayedStillAtCurrentLocation() > MOUSE_INACTIVE_MIN_INTERVAL;
        bool inThePvzGameRect = Rect(Point(0,23),Point(799,622)).contains(GetCurrentMouseLocation());

        if (areThereSuns && satisfyTimeLimits && inThePvzGameRect && (inTheExecutionRect || isNotActive)) {
            lastTimeMouseMoveExecuted = GetCurrentTime();
            ExecuteMouseMoves();

        }

        fps = 1 / ( GetCurrentTime() - timeAtLastFrame );
        timeAtLastFrame = GetCurrentTime();

        counter10Frames++;
        if (counter10Frames >= 10) {
            fpsAvg = 1 / ( (GetCurrentTime() - timeAtLast10Frames) / 10 );
            fpsAvgHistory = fpsAvg;
            timeAtLast10Frames = GetCurrentTime();
            counter10Frames = 0;
        }

        cout << "Suns " << numberOfSuns << " | " << numberOfNotSuns  << " \t"
             << (satisfyTimeLimits?"TimeLimits YES":"TimeLimits NO")  << "\t"
             << (isNotActive?"MouseInactive YES":"MouseInactive YES") << "\t"
             << (inTheExecutionRect?"ExecRect YES":"ExecRect NO")     << "\t"
             << (inThePvzGameRect?"GameRect YES":"GameRect NO")       << endl;

        double timeConsumed = GetTimeSince(_timeDetectionStarts);
        cout << "\t\tTimeConsumed " << to_string(timeConsumed) << "\t";
        cout << "\tfps: " << to_string(fps) << "\t( " << to_string(fpsAvgHistory) <<" )\t#END" << endl << endl;


        // Draw Rects and Dots on the image for suns
        for (unsigned long i = 0; i < numberOfSuns; i++) {
            rectangle(imgMarked, rectsSun[i], Scalar(255, 0, 255), 5);
            circle(imgMarked, sunLocations[i], 8, Scalar(255, 0, 0), -100);
        }

        // Draw Rects for everything but suns
        for (unsigned long i = 0; i < numberOfNotSuns; i++) {
            rectangle(imgMarked, rectsNotSun[i], Scalar(0, 0, 255), 5);
        }

        imautoshow("imgMarked", imgMarked, 565.0);

        // imautoshow("imgCropped", imgCropped, 450.0);

        int k = waitKey(10);
        if (k == 27) break;
    }


    destroyAllWindows();
}

#ifndef PVZSUNAUTOCOLLECT_H
#define PVZSUNAUTOCOLLECT_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include "execcommand.h"

using namespace std;
using namespace cv;

#define PERFERED_SIDE_LENGTH 450.0

class PvzSunAutoCollect {

    struct PositionAndTime {
        Point pos;
        double time;
        PositionAndTime(Point _pos, double _time) : pos(_pos), time(_time) {}
    };

private:

    const static int LOW_H = 28;
    const static int LOW_S = 11;
    const static int LOW_V = 251;
    const static int HIGH_H = 31;
    const static int HIGH_S = 255;
    const static int HIGH_V = 255;
    const static int kernelSize = 9;
    const static int iterations = 1;
    const double RATIO_LEFT = 0.02753442;
    const double RATIO_RIGHT = 0.0390625;
    const double RATIO_TOP = 0.17676768;
    const double RATIO_BOTTOM = 0.04292929;
    const int TOP_OFFSET = 23;
    enum CliclickAction { CLICK = 0, MOVE = 1 };

    const double DETECTION_TIME_INTERVAL = 0.2;
    const double EXECUTE_MOUSE_MOVES_MIN_INTERVAL = 0.2;
    const double MOUSE_INACTIVE_MIN_INTERVAL = 0.1;





    const double MOUSE_POSITION_RECORD_MIN_INTERVAL = 0.05;
    deque<PositionAndTime> mousePositionHistory;
    const double MOUSE_POSITION_HISTORY_MAX_SIZE = 100;


    Mat imgSrc;
    Mat imgCropped;
    Mat imgCvt;
    Mat imgMarked;

    vector< vector<Point> > contours;
    unsigned long numberOfSuns = 0;
    vector<Rect> rectsSun;
    unsigned long numberOfNotSuns = 0;
    vector<Rect> rectsNotSun;
    vector<Point> sunLocations;
    vector<Point> targetsGlobelPositions;

    double _timeDetectionStarts = 0.0;


public:
    PvzSunAutoCollect();
    Mat AutoResizeForDisplay(InputArray input, double perferedSideLength = PERFERED_SIDE_LENGTH);
    void imautoshow(const string& winname, const Mat& img, double perferedSideLength = PERFERED_SIDE_LENGTH);
    //? Mat Shrink(InputArray src, int factor);
    Mat _AutoCrop(const Mat& src);
    Mat AutoCrop(const Mat& src);
    void MoveMouse(vector<Point> points, CliclickAction action, int delay = 100);
    void MoveMouse(Point point, CliclickAction action, int delay = 100);
    Point GetCurrentMouseLocation();
    double GetCurrentTime();
    double GetTimeSince(double previousTime = 0);
    void RecordMousePositionHistory();
    double GetTimeForMouseStayedStillAtCurrentLocation();

    void Capture();
    void Detect();
    void ExecuteMouseMoves();
    void Start();

};

#endif // PVZSUNAUTOCOLLECT_H

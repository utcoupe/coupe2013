
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "helper.h"


using namespace std;
using namespace cv;

Point point1, point2; /* vertical points of the bounding box */
int drag = 0;
Rect rect; /* bounding box */
Mat img, roiImg; /* roiImg - the part of the image in the bounding box */
int select_flag = 0;
Mat result;

/**
 * Faites pas double-click!
 * @param event [description]
 * @param x     [description]
 * @param y     [description]
 * @param flags [description]
 * @param param [description]
 */
void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        /* left button clicked. ROI selection begins */
        point1 = Point(x, y);
        drag = 1;
        select_flag = 0;
    }
     
    if (event == CV_EVENT_MOUSEMOVE && drag)
    {
        /* mouse dragged. ROI being selected */
        point2 = Point(x, y);
        // rectangle(img, point1, point2, CV_RGB(80, 0, 0), 3, 8, 0);
    }
     
    if (event == CV_EVENT_LBUTTONUP && drag)
    {
        point2 = Point(x, y);
        rect = Rect(point1.x,point1.y,x-point1.x,y-point1.y);
        drag = 0;
        roiImg = img(rect);
        roiImg = roiImg.clone();
    }
     
    if (event == CV_EVENT_LBUTTONUP)
    {
       /* ROI selected */
        select_flag = 1;
        drag = 0;
    }

    else
        return;
}
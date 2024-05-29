#include <opencv2/opencv.hpp>
#include <iostream>
#include "camera.h"
#include "utils.h"

using namespace cv;
using namespace std;

void calculate_current_position_and_speed(cv::Mat &mask, cv::Mat &frame, Camera &camera)
{
    float areaThreshold = 50.0f;
    vector<vector<Point>> contours = findContoursInMask(mask, areaThreshold);
    if (!contours.empty())
    {
        {
            get_PositionFromContours(frame, contours, camera.current_tracker_position, camera.previous_tracker_position);
        }
        camera.tracker_speed = camera.current_tracker_position - camera.previous_tracker_position;

        //visualizeOpticalFlow(camera.previous_tracker_position, camera.current_tracker_position, frame);

        camera.previous_tracker_position = camera.current_tracker_position;
    }
    else
    {
        // camera.previous_tracker_position = Point2f(-1, -1);
        cout << "previous tracker position: " << camera.previous_tracker_position << endl;
    }
}

void tracker_position_by_tracking(Camera &camera)
{
    
    Mat &frame = camera.currentFrame;
    Mat &background = camera.background;

    if (frame.empty() || background.empty())
    {
        cerr << "Error: Frame or background is empty." << endl;
        return;
    }

    // HSV range for the pink ball
    Scalar lower_pink(130, 50, 50);
    Scalar upper_pink(180, 255, 255);

    Mat mask, diff, foregroundMask, hsvFrame;

    // Convert frame to HSV
    cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

    // Background subtraction
    absdiff(frame, background, diff);
    cvtColor(diff, diff, COLOR_BGR2GRAY);
    threshold(diff, foregroundMask, 50, 255, THRESH_BINARY);

    // Color keying in HSV
    inRange(hsvFrame, lower_pink, upper_pink, mask);

    // Combine the masks
    bitwise_and(mask, foregroundMask, mask);

    // Apply some preprocessing (e.g., dilate and erode to clean up the mask)
    dilate(mask, mask, Mat(), Point(-1, -1), 2);
    erode(mask, mask, Mat(), Point(-1, -1), 2);

    if (mask.empty())
    {
        cerr << "Error: Mask is empty." << endl;
        return;
    }

    calculate_current_position_and_speed(mask, frame, camera);
}


void trackBallInFrame(Camera &camera, int frame_index, int cameras_num)
{

    camera.is_tracking_active = check_tracking_active(frame_index, cameras_num, camera);

   // if (camera.is_tracking_active)
  //  {
        tracker_position_by_tracking(camera);

  //  }
   // else 
  //  {
   //     camera.current_tracker_position = camera.previous_tracker_position + camera.tracker_speed;
        //visualizeOpticalFlow(camera.previous_tracker_position, camera.current_tracker_position, camera.currentFrame);
   //     camera.previous_tracker_position = camera.current_tracker_position;
   // /}
    
    

}



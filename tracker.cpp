#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()

bool __left= false;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "__left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        __left = true;
    }
}


int main(int argc, char **argv)
{
    string trackerTypes[7] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "CSRT"};
    // vector <string> trackerTypes(types, std::end(types));
    string trackerType = trackerTypes[4];

    Ptr<Tracker> tracker;

#if(2<1)//CV_MINOR_VERSION < 3)
    {
        tracker = Tracker::create(trackerType);
    }
#else
    {
        if (trackerType == "BOOSTING")
            tracker = TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker = TrackerMIL::create();
        if (trackerType == "KCF")
            tracker = TrackerKCF::create();
        if (trackerType == "TLD")
            tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW")
            tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")
            tracker = TrackerGOTURN::create();
        if (trackerType == "CSRT")
            tracker = TrackerCSRT::create();
    }
#endif
    namedWindow("Tracking", 1);
    VideoCapture video("/home/imad/Desktop/opencv-object-tracking/drone.mp4");
    int _fps = video.get(CAP_PROP_FPS);
    //VideoCapture video(0);
    if(!video.isOpened())
    {
        cout << "Could not read video file" << endl;
        return 1;
    }

    Mat frame;
    Rect2d bbox;

    while(video.read(frame))
    {
        setMouseCallback("Tracking", CallBackFunc, NULL);
        resize(frame, frame, Size(640,480));
        if(__left)
        {
            cout<<"left---------------------\n";
            bbox = selectROI("Tracking",frame, true);
            rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1 );
            tracker->init(frame, bbox);
            break;
        }
        imshow("Tracking", frame);
        waitKey(_fps);
    }

    while(video.read(frame))
    {
        resize(frame, frame, Size(640,480));
        double timer = (double)getTickCount();
        bool ok = tracker->update(frame, bbox);

        cout << "Center.X = " << bbox.x + bbox.width/2 <<endl;
        cout << "Center.Y = " << bbox.y + bbox.height/2 <<endl;

        float fps = getTickFrequency() / ((double)getTickCount() - timer);

        if (ok)
            rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
        else
            putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);
        putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

        imshow("Tracking", frame);
        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            break;
        }
    }
}



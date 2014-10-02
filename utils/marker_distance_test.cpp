#include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace aruco;

//Width of line to highlight marker with
const int LINE_WIDTH = 1;

//Color to highlight marker lines with
const Scalar COLOR = (0, 0, 255);

//Delay before next image retrieval
const int WAIT_TIME = 2;

//The size of the marker in meters
const float MARKER_SIZE = 0.16;

//Integer representation of which keyboard key was pressed
int inputKey = 0;

//Path to files
string boardConfigFile;

string cameraConfigFile;

VideoCapture videoCapture;

Mat inputImage,inputImageCopy;
BoardConfiguration boardConfig;
BoardDetector boardDetector;
CameraParameters cameraParams;


Vector<Marker> markers;

int main(int argc,char **argv)
{
    try
    {
        if (argc!=3) {
            cerr<<"Usage: boardConfig.yml camera_config.yml "<<endl;
            return -1;
        }

        //Get the camera config
        //cameraConfigFile = argv[2];
        //cameraParams.readFromFile(cameraConfigFile);

        //Read the board
        boardConfigFile = argv[1];
        boardConfig.readFromFile(boardConfigFile);

        //Open the camera
        videoCapture.open(0);

        //make sure camera is open
        if (!videoCapture.isOpened()) {
            cerr<<"Could not open the video"<<endl;
            return -1;
        }

        //read an image from camera
        videoCapture >> inputImage;

        //create the gui
        cv::namedWindow("output video", CV_WINDOW_AUTOSIZE);

        boardDetector.setParams(boardConfig);



        while ( inputKey != 27 && videoCapture.grab()) {

            //retrieve an image from the camera
            videoCapture.retrieve(inputImage);
            inputImage.copyTo(inputImageCopy);


            boardDetector.detect(inputImage);
            markers = boardDetector.getDetectedMarkers();
            for (unsigned int i = 0; i < markers.size(); i++) {
                Marker m = markers[i];

                float camData[] = {1.3089596458327883e+03, 0.0, 3.1716402612677911e+02, 0.0, 1.3249652390726415e+03, 2.3359932814285278e+02, 0., 0., 1.0};
                Mat cam = Mat(3, 3, CV_32F, camData);
                float distortionData[] = {1.8869609562810794e+00, 4.2431486029577115e+01,
                        -7.9002380674102313e-02, -1.0615309141897006e-02,
                        -1.2940788383601671e+03};
                Mat distortion = Mat(1, 5, CV_32F, distortionData);
                Size size = Size(640, 480);

                CameraParameters cp = CameraParameters(cam, distortion, size);
                m.calculateExtrinsics(MARKER_SIZE, cp);

                m.draw(inputImageCopy, COLOR, LINE_WIDTH);
                Mat t = m.Tvec;
                int x = t.at<Vec3f>(0, 0)[0];
//                cout << x << endl;

//                int x = markers[i].
            }

            cv::imshow("output video",inputImageCopy);


            //Check if the stop button has been pressed
            inputKey = cv::waitKey(WAIT_TIME);
        }

        return 0;

    } catch (std::exception &ex)
    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}



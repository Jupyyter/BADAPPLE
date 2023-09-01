#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <opencv2/core/utils/logger.hpp>
#include <string_view>
#include <fstream>
#include <SFML\Audio.hpp>

const double ASPECT_RATIO = 0.5;
//const std::string ASCII_CHARS = " .:-=+*#%@"; for some reason this dosen't work
const std::string ASCII_CHARS = "@%#*+=-:. ";
int numChars = int(ASCII_CHARS.length());
int step = 256 / numChars;
int index;
static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
COORD coord = { 0, 0 };

char grayscaleToASCII(int pixelValue) {
    index = (256-pixelValue) / step;  //invert the mapping
    return ASCII_CHARS[index];
}

void setCursorPosition0()
{
    SetConsoleCursorPosition(hOut, coord);
}

int main() {
    //removing synchronization between c++ and I/O Streams (go fast fast)
    std::ios_base::sync_with_stdio(false);
    //disable annoying warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
    ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
    std::string videoPath="badApple.mov";
    cv::VideoCapture video(videoPath);
    if (!video.isOpened()) {
        std::cout<<"no video?";
    }

    //get the cmd resolution
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    //set the ascii height
    const int terminalHeight = consoleHeight;

    //calculate the aspect ratio of characters
    const double charAspectRatio = static_cast<double>(video.get(cv::CAP_PROP_FRAME_WIDTH)) / video.get(cv::CAP_PROP_FRAME_HEIGHT);
    const int asciiHeight = terminalHeight;
    const int asciiWidth = static_cast<int>(terminalHeight * charAspectRatio / ASPECT_RATIO);
    cv::Mat frame;
    
    int rows;
    int cols;
    std::stringstream asciiLines;   //in this variable the whole ascii is saved
    int i;
    int j;
    int l;

    {
        cv::Mat fakeFrame=frame.clone();
        video.read(fakeFrame);
        cv::resize(fakeFrame, fakeFrame, cv::Size(asciiWidth, asciiHeight));
        rows=fakeFrame.rows;
        cols=fakeFrame.cols;
    }
    int spaceRemaining=(consoleWidth-cols)/2;
    std::string songPath = "badApple.flac";
    sf::Music song;
    if(!song.openFromFile(songPath)){
        std::cout<<"no audio?";
    }
    song.play();

    //variables used for fps
    using namespace std::chrono;
    using dsec = duration<double>;
    auto invFpsLimit = std::chrono::round<system_clock::duration>(dsec{1./video.get(cv::CAP_PROP_FPS)});
    auto m_BeginFrame = system_clock::now();
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    auto prev_time_in_seconds = time_point_cast<seconds>(m_BeginFrame);
    auto time_in_seconds = time_point_cast<seconds>(system_clock::now());

    cv::Size frameSize(asciiWidth, asciiHeight);
    int lastRow=rows-1;

    while (video.read(frame)) {

        //loading ascii
        cv::resize(frame, frame, frameSize);
        cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        asciiLines.str("");

        for (i = 0; i != rows; ++i) {
            for(l=0;l<spaceRemaining;l++){
                asciiLines<<" ";
            }
            for (j = 0; j != cols; j++) {
                asciiLines<< grayscaleToASCII((int)frame.at<uchar>(i, j));
            }
            if(i!=lastRow){
                asciiLines<<'\n';
            }
        }
        //drawing ascii
        setCursorPosition0();
        std::cout.flush();
        std::cout << asciiLines.str();
        setCursorPosition0();

        time_in_seconds = time_point_cast<seconds>(system_clock::now());
        if (time_in_seconds > prev_time_in_seconds)
        {
            prev_time_in_seconds = time_in_seconds;
        }

        //this part keeps the frame rate.
        std::this_thread::sleep_until(m_EndFrame);
        m_BeginFrame = m_EndFrame;
        m_EndFrame = m_BeginFrame + invFpsLimit;

    }

    std::cout << ":):):):):):):):):):):):):):):):):):):):):):):):):):):):):):):):)";
    std::cin.get();

    return 0;
}
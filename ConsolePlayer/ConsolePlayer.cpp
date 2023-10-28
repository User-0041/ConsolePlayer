#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Windows.h>
#include <iostream>
#include <queue>
#include <semaphore>
#include <thread>
using namespace cv;

using namespace std;


#define SIZE 100000000

int nScreenWidth = 960;
int nScreenHeight = 720;
std::counting_semaphore<SIZE> Free(SIZE);
std::counting_semaphore<SIZE> Used(0);



void Shader(int threadid) {



}

void Decoder(queue <wchar_t*>& Frames) {
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    string path = "BadApple.mp4";
    VideoCapture cap(path);
    Mat img, graymat;
    int Pixel;
    int Frame = 0;
    while (cap.isOpened())
    {
        cap.read(img);
        cvtColor(img, graymat, COLOR_BGR2GRAY);

        int x = 0;
        for (int j = 0; j < graymat.rows; j = j + 2)
        {


            for (int i = 0; i < graymat.cols; i++)
            {
                Pixel = img.at<char>(j, i * 3);
                    
                switch (Pixel)
                {
                case 0:
                    screen[x] = ' ';
                    break;
                default:
                    if (Pixel > 0 && Pixel < 240) {
                        screen[x] = '.';

                    }
                    else
                    {
                        screen[x] = 'a';

                    }
                }
                x++;

            }




        }
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        Free.acquire();
        Frames.push(screen);
        Used.release();


        /*Naztar*/
    }

    return;
}


void Render(queue <wchar_t*>& Frames) {

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;
   

    while (true)
    {

        Used.acquire();
        WriteConsoleOutputCharacter(hConsole, Frames.front(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
        Frames.pop();
        Free.release();
    }

}


int main() {
    queue <wchar_t*> Frames;


    std::thread t1(Decoder, std::ref(Frames));
    std::thread t2(Render, std::ref(Frames));
    t1.join();
    t2.join();
    return 0;


    
}

// OCVTessExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>

#include <nana/gui.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/place.hpp>
#include <nana/paint/image.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/timer.hpp>

using namespace std;
using namespace cv;
//using namespace nana;

Mat calcPerspectiveTransform(Point2f source[], float width, float height) {
	/*
		List<Point> source = new ArrayList<>();
		for(Pair<Double,Double> pair : screenPosition) {
			source.add(new Point(pair.getKey(),pair.getValue()));
		}
		Mat startM = Converters.vector_Point2f_to_Mat(source);
		Mat temp = getPerspectiveTransform(startM, screenWidth, screenHeight);
		if(UiTesterConfiguration.releaseMat() == true)
			startM.release();
		return temp;	
	*/

	Point2f dest[4];
	dest[0] = Point2f(0.0, 0.0);
	dest[1] = Point2f(0.0, height);
	dest[2] = Point2f(width, height);
	dest[3] = Point2f(width, 0.0);


	Mat res = getPerspectiveTransform(source, dest);
	return res;
}

Mat warp(Mat frame, Mat perspTrans, float targetWidth, float targetHeight) {

	Mat result(targetWidth, targetHeight, CV_8UC4);
	warpPerspective(frame, result, perspTrans, Size(targetWidth, targetHeight), INTER_AREA);
	return result;
}

String* doOcr(Mat transformed, tesseract::TessBaseAPI *ocr) {

	Mat grey;
	cvtColor(transformed, grey, COLOR_BGR2GRAY);

	blur(grey, grey, Size(3, 3));

	Mat thresh;
	adaptiveThreshold(grey, thresh, 250.0, AdaptiveThresholdTypes::ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 25, 39.0);

	imshow("threshed mat", thresh);

	ocr->SetImage(thresh.data, thresh.cols, thresh.rows, 1, thresh.step);
	ocr->SetSourceResolution(70);

	ocr->Recognize(0);

	tesseract::ResultIterator* ri = ocr->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
				word, conf, x1, y1, x2, y2);
			//delete[] word;
		} while (ri->Next(level));
	}

	String res[] = { "test" };
	return res;
}

void mouseClicked(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

int main(int argc, char* argv[])
{

	// set code page to utf8
	SetConsoleOutputCP(CP_UTF8);

	string outText = "test";
	string imPath = argv[1];

	// Create Tesseract object
	tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

	//ocr->SetVariable("tessedit_char_whitelist", "öäüÖÄÜabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789€,.:;");

	cout << ocr->Version() << endl;

	// Initialize tesseract to use English (eng) and the LSTM OCR engine. 
	//ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
	ocr->Init("tessdata", "deu", tesseract::OEM_LSTM_ONLY);

	// Set Page segmentation mode to PSM_AUTO (3)
	ocr->SetPageSegMode(tesseract::PSM_AUTO);
	// Open input image using OpenCV
	Mat im = cv::imread(imPath, IMREAD_COLOR);

	//vector<int> compression_params;
	//compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	//compression_params.push_back(9);

	////cv::imwrite("img.png", im, compression_params);
	cv::imwrite("img.png", im);
	im = cv::imread("img.png");

	// Set image data
	//ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);

	// Run Tesseract OCR on image
	// outText = string(ocr->GetUTF8Text());

	// print recognized text
	cout << outText << endl; // Destroy used object and release memory ocr->End();

	nana::form fm;
	fm.size(nana::size(800, 600));
	nana::picture pic(fm);

	nana::button btn(fm, "Select", true);

	nana::timer timer{ std::chrono::milliseconds{50} };

	nana::place plc(fm);    //Layout
	plc.div("margin=5 PIC");
	plc["PIC"] << pic;
	plc.collocate();

	//nana::paint::image img;
	//img.open("test.bmp");

	//pic.load(img); //Display the PNG file

	VideoCapture vCap(0, cv::VideoCaptureAPIs::CAP_DSHOW);
	vCap.open(0, 0);
	vCap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1280.0);
	vCap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 720.0);
	cout << "cap width" << vCap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH) << endl;
	cout << "cap height" << vCap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT) << endl;
	//vCap.set(cv::VideoCaptureProperties::CAP_PROP_FOCUS, 110.0);

	float scaledWidth = 360;// 240.0;
	float scaledHeight = 480.0;// 320.0;
	nana::paint::pixel_buffer pxbuf(scaledWidth, scaledHeight);
	//pxbuf.put(im.data, im.cols, im.rows, 8*im.channels(), im.cols*im.channels(), false);

	nana::drawing dw{pic};
	dw.draw([pxbuf](nana::paint::graphics& graph) {
		pxbuf.paste(graph.handle(), {});
	});

	cv::Point2f points[4];

	//points[0] = Point2f(211.8857119763433, 619.2);
	//points[1] = Point2f(911.3142757817484, 598.6285714285714);
	//points[2] = Point2f(911.3142757817484, 80.22857142857143);
	//points[3] = Point2f(197.48571213329083, 74.05714285714286);
	points[0] = Point2f(319, 551);
	points[1] = Point2f(846, 534);
	points[2] = Point2f(844, 145);
	points[3] = Point2f(308, 143);

	//points[0] = Point2f(472, 830);
	//points[1] = Point2f(1274, 805);
	//points[2] = Point2f(1272, 213);
	//points[3] = Point2f(455, 209);


	Mat persTr = calcPerspectiveTransform(points, scaledWidth, scaledHeight);

	std::vector<cv::Point> contour;
	contour.push_back(points[0]);
	contour.push_back(points[1]);
	contour.push_back(points[2]);
	contour.push_back(points[3]);

	std::vector<std::vector<cv::Point>> contours;
	contours.push_back(contour);

	cout << "perspTr" << persTr  << endl << endl;

	//namedWindow("orig_frame");
	//setMouseCallback("orig_frame", mouseClicked);

	Mat oldFrame;

	timer.elapse([&vCap, &pxbuf, &dw, persTr, scaledWidth, scaledHeight, ocr, contours, &oldFrame]{
		std::clock_t start;
		double duration;
		int match_method = TemplateMatchModes::TM_CCOEFF_NORMED;
		start = std::clock();
		if (vCap.isOpened()) {
			Mat frame;

			vCap.read(frame);

			//imshow("orig_frame", frame);

			Mat transformed = warp(frame, persTr, scaledWidth, scaledHeight);

			//drawContours(frame, contours, 0, Scalar(25, 25, 255), 2);

			if (!transformed.empty() && !oldFrame.empty()) {

				Mat templ = oldFrame(Rect(2, 2, oldFrame.cols-4, oldFrame.rows-4));

				Mat result;

				matchTemplate(transformed, templ, result, match_method);

				double minVal; double maxVal; Point minLoc; Point maxLoc;
				Point matchLoc;
				minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

				//cout << "min: " << minVal << ",  max: " << maxVal << endl;

				if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
				{
					matchLoc = minLoc;
				}
				else
				{
					matchLoc = maxLoc;
				}
				Mat toDisplay = transformed;

				if (maxVal < 0.96) {
					pxbuf.put(toDisplay.data, toDisplay.cols, toDisplay.rows, 8 * toDisplay.channels(), toDisplay.cols*toDisplay.channels(), false);
					String* resultStr = doOcr(transformed, ocr);
					rectangle(transformed, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
					dw.update();
				}
				//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

			}
			if (!transformed.empty())
				transformed.copyTo(oldFrame);

			//pyrMeanShiftFiltering(transformed, transformed, 20.0, 50.0, 1);

		}
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		//std::cout << "duration: " << duration << '\n';
	});

	timer.start();

	fm.show();
	nana::exec();

	return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

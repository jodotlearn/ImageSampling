#include <iostream>
#include <ctime>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

void downSampling(Mat input, Mat output){
	Vec3b pxTL, pxTR, pxBL, pxBR, outputPx;
	/*fd(m, n) = [f(2m, 2n) + f(2m, 2n + 1) + f(2m + 1, 2n) + f(2m + 1, 2n + 1)] / 4 */
	for (int i = 0; i < output.rows; i++){
		for (int j = 0; j < output.cols; j++){
			pxTL = input.at<Vec3b>(2 * i, 2 * j);
			pxTR = input.at<Vec3b>(2 * i, 2 * j + 1);
			pxBL = input.at<Vec3b>(2 * i + 1, 2 * j);
			pxBR = input.at<Vec3b>(2 * i + 1, 2 * j + 1);
			outputPx[0] = (pxTL[0] + pxTR[0] + pxBL[0] + pxBR[0]) / 4;//B
			outputPx[1] = (pxTL[1] + pxTR[1] + pxBL[1] + pxBR[1]) / 4;//G
			outputPx[2] = (pxTL[2] + pxTR[2] + pxBL[2] + pxBR[2]) / 4;//R
			output.at<Vec3b>(i, j) = outputPx;
		}
	}
}

void upSampling(Mat input, Mat output){
	Vec3b inputPxTL, inputPxTR, inputPxBL, inputPxBR;
	Vec3b outputTL, outputTR, outputBL, outputBR;
	int iNext, jNext;
	for (int i = 0; i < input.rows; i++){
		for (int j = 0; j < input.cols; j++){
			/*
			O[2m,2n]=I[m,n]
			O[2m,2n+1]=(I[m,n]+I[m, n+1])/2
			O[2m+1,2n]=(I[m,n]+I[m+1, n])/2
			O[2m+1,2n+1]=(I[m,n]+I[m,n+1]+I[m+1,n]+I[m+1,n+1])/4
			*/
			iNext = (i + 1) == input.rows ? i : (i + 1);
			jNext = (j + 1) == input.cols ? j : (j + 1);
			inputPxTL = input.at<Vec3b>(i, j);
			inputPxTR = input.at<Vec3b>(i, jNext);
			inputPxBL = input.at<Vec3b>(iNext, j);
			inputPxBR = input.at<Vec3b>(iNext, jNext);

			outputTL = inputPxTL;
			output.at<Vec3b>(2 * i, 2 * j) = outputTL;
			outputTR[0] = (inputPxTL[0] + inputPxTR[0]) / 2;//B
			outputTR[1] = (inputPxTL[1] + inputPxTR[1]) / 2;//G
			outputTR[2] = (inputPxTL[2] + inputPxTR[2]) / 2;//R
			output.at<Vec3b>(2 * i, 2 * j + 1) = outputTR;
			outputBL[0] = (inputPxTL[0] + inputPxBL[0]) / 2;//B
			outputBL[1] = (inputPxTL[1] + inputPxBL[1]) / 2;//G
			outputBL[2] = (inputPxTL[2] + inputPxBL[2]) / 2;//R
			output.at<Vec3b>(2 * i + 1, 2 * j) = outputBL;
			outputBR[0] = (inputPxTL[0] + inputPxTR[0] + inputPxBL[0] + inputPxBR[0]) / 4;//B
			outputBR[1] = (inputPxTL[1] + inputPxTR[1] + inputPxBL[1] + inputPxBR[1]) / 4;//G
			outputBR[2] = (inputPxTL[2] + inputPxTR[2] + inputPxBL[2] + inputPxBR[2]) / 4;//R
			output.at<Vec3b>(2 * i + 1, 2 * j + 1) = outputBR;
		}
	}
}

void processImage(string path, string fileName, int flag, int factor){
	clock_t begin_time = clock();
	//read an image from disk
	Mat img = imread(path + fileName, flag);
	//generate a new image for down-sampling
	Mat downSamplingImage;
	downSamplingImage.create(img.rows / factor, img.cols / factor, img.type());
	downSampling(img, downSamplingImage);

	//write down-sampling image to disk
	string downSamplingFilePath = path + "down_" + fileName;
	imwrite(downSamplingFilePath, downSamplingImage);
	cout << " down-sampling file is: " << downSamplingFilePath << endl;
	cout << " spent time: " << float(clock() - begin_time) << " ms" << endl;
	cout << " " << endl;

	//generate a new image for up-sampling
	Mat upSamplingImage;
	upSamplingImage.create(downSamplingImage.rows * 2, downSamplingImage.cols * 2, downSamplingImage.type());
	upSampling(downSamplingImage, upSamplingImage);

	//write up-sampling image to disk
	string upSamplingFilePath = path + "up_" + fileName;
	imwrite(upSamplingFilePath, upSamplingImage);
	cout << " up-sampling file is: " << upSamplingFilePath << endl;
	cout << " spent time: " << float(clock() - begin_time) << " ms" << endl;
}

int main(){
	const clock_t begin_time = clock();
	const int SAMPLINGFACTOR = 2;
	cout << "start processing 1.jpg ..." << endl;
	processImage("", "1.jpg", CV_LOAD_IMAGE_UNCHANGED, SAMPLINGFACTOR);
	cout << " " << endl;
	cout << "start processing 2.jpg ..." << endl;
	processImage("", "2.jpg", CV_LOAD_IMAGE_UNCHANGED, SAMPLINGFACTOR);
	cout << " " << endl;
	cout << "total spent time: " << float(clock() - begin_time) / CLOCKS_PER_SEC << " secs" << endl;
	return 0;
}
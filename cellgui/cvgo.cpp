#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;


int cvgo() {
	//double t = getTickCount();
	Mat src = imread("test9.jpg");
	if (!src.data) {
		printf("could not load image...\n");
		return -1;
	}
	//imshow("待提取图像", src);
	//waitKey(0);
	Mat gray_src;//转灰度
	cvtColor(src, gray_src, CV_BGR2GRAY);

	Mat blur_src;//滤波
	bilateralFilter(gray_src, blur_src, 7, 150, 50);
	//imshow("双边滤波", blur_src);
	//边缘发现
	Mat x;
	Mat y;
	Mat fin;
	Sobel(blur_src, x, CV_16S, 1, 0, 3);
	Sobel(blur_src, y, CV_16S, 0, 1, 3);
	convertScaleAbs(x, x);
	convertScaleAbs(y, y);
	//imshow(output_title, gray);

	int h = x.rows;
	int w = x.cols;
	fin = Mat(x.size(), x.type());

	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {

			int xg = x.at<uchar>(row, col);
			int yg = y.at<uchar>(row, col);
			int xyg = xg + yg;
			fin.at<uchar>(row, col) = saturate_cast<uchar>(xyg);

		}
	}
	threshold(fin, fin, /*230*/10/*（复杂细胞外形）most_num*/, 255, THRESH_BINARY);
	imshow("轮廓发现", fin);

	//膨胀（看情况使用，一般用于细胞边缘与背景极为接近）
	Mat str0;
	str0 = getStructuringElement(MORPH_RECT, Size(4, 4), Point(-1, -1));
	dilate(fin, fin, str0, Point(-1, -1), 1);//膨胀
	erode(fin, fin, str0, Point(-1, -1), 1);//腐蚀
	imshow("膨胀0", fin);



	//轮廓+填充
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(fin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());//轮廓发现
	Mat ddst = Mat::zeros(src.size(), gray_src.type());
	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 50) drawContours(ddst, contours, i, Scalar(255, 255, 255), -1);
	}
	imshow("fullfill image", ddst);

	fin = ddst;


	//填充孔洞,如果不执行这一步也可以，但是碰到较小细胞时容易被当作小物体去除，遇到复杂外形细胞时易于做膨胀操作
	Mat dstBw;
	Size m_Size = fin.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, fin.type());//延展图像
	fin.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
	cv::floodFill(Temp, Point(0, 0), Scalar(255));
	Mat cutImg;//裁剪延展的图像
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
	dstBw = fin | (~cutImg);
	//imshow("漫水填充", dstBw);


	//膨胀（看情况使用，一般用于细胞边缘与背景极为接近）
	Mat str;
	str = getStructuringElement(MORPH_RECT, Size(6, 6), Point(-1, -1));
	dilate(dstBw, dstBw, str, Point(-1, -1), 1);
	imshow("膨胀", dstBw);


	//轮廓发现、绘制、截取、保存
	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;
	findContours(dstBw, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//发现轮廓

	Mat bg;
	src.copyTo(bg);
	for (size_t i = 0; i < contours1.size(); i++) {
		if (contours1[i].size() > 20) {
			//绘制轮廓矩形
			Rect rect = boundingRect(contours1[i]);
			rect = rect + Point(-3, -3);	//平移
			rect = rect + Size(6, 6);	    //缩放，左上顶点不变
			rectangle(bg, rect, Scalar(0, 255, 230), 2, LINE_AA, 0);

			//割取矩形区域保存（改进）
			Mat src_ex;
			char file_name[] = "C:\\Users\\liuzhongtian\\Desktop\\2\\test0000.jpg";
			//sprintf_s(file_name, "C:\\Users\\liuzhongtian\\Desktop\\2\\test%d.jpg", int(i + 1));
			copyMakeBorder(src, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//扩大原图像并填充边缘
																								//imwrite(file_name, src_ex(rect + Point(8, 8)));
		}
	}

	imshow("提取结果", bg);
	//imwrite("C:\\Users\\liuzhongtian\\Desktop\\2\\test0.jpg", bg);
	//double timeconsume = (getTickCount() - t) / getTickFrequency();
	//printf("time consume %.3f", timeconsume);


	waitKey(0);

	return 0;

}

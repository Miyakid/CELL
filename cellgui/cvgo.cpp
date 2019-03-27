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
	//imshow("����ȡͼ��", src);
	//waitKey(0);
	Mat gray_src;//ת�Ҷ�
	cvtColor(src, gray_src, CV_BGR2GRAY);

	Mat blur_src;//�˲�
	bilateralFilter(gray_src, blur_src, 7, 150, 50);
	//imshow("˫���˲�", blur_src);
	//��Ե����
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
	threshold(fin, fin, /*230*/10/*������ϸ�����Σ�most_num*/, 255, THRESH_BINARY);
	imshow("��������", fin);

	//���ͣ������ʹ�ã�һ������ϸ����Ե�뱳����Ϊ�ӽ���
	Mat str0;
	str0 = getStructuringElement(MORPH_RECT, Size(4, 4), Point(-1, -1));
	dilate(fin, fin, str0, Point(-1, -1), 1);//����
	erode(fin, fin, str0, Point(-1, -1), 1);//��ʴ
	imshow("����0", fin);



	//����+���
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(fin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());//��������
	Mat ddst = Mat::zeros(src.size(), gray_src.type());
	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 50) drawContours(ddst, contours, i, Scalar(255, 255, 255), -1);
	}
	imshow("fullfill image", ddst);

	fin = ddst;


	//���׶�,�����ִ����һ��Ҳ���ԣ�����������Сϸ��ʱ���ױ�����С����ȥ����������������ϸ��ʱ���������Ͳ���
	Mat dstBw;
	Size m_Size = fin.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, fin.type());//��չͼ��
	fin.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
	cv::floodFill(Temp, Point(0, 0), Scalar(255));
	Mat cutImg;//�ü���չ��ͼ��
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
	dstBw = fin | (~cutImg);
	//imshow("��ˮ���", dstBw);


	//���ͣ������ʹ�ã�һ������ϸ����Ե�뱳����Ϊ�ӽ���
	Mat str;
	str = getStructuringElement(MORPH_RECT, Size(6, 6), Point(-1, -1));
	dilate(dstBw, dstBw, str, Point(-1, -1), 1);
	imshow("����", dstBw);


	//�������֡����ơ���ȡ������
	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;
	findContours(dstBw, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//��������

	Mat bg;
	src.copyTo(bg);
	for (size_t i = 0; i < contours1.size(); i++) {
		if (contours1[i].size() > 20) {
			//������������
			Rect rect = boundingRect(contours1[i]);
			rect = rect + Point(-3, -3);	//ƽ��
			rect = rect + Size(6, 6);	    //���ţ����϶��㲻��
			rectangle(bg, rect, Scalar(0, 255, 230), 2, LINE_AA, 0);

			//��ȡ�������򱣴棨�Ľ���
			Mat src_ex;
			char file_name[] = "C:\\Users\\liuzhongtian\\Desktop\\2\\test0000.jpg";
			//sprintf_s(file_name, "C:\\Users\\liuzhongtian\\Desktop\\2\\test%d.jpg", int(i + 1));
			copyMakeBorder(src, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//����ԭͼ������Ե
																								//imwrite(file_name, src_ex(rect + Point(8, 8)));
		}
	}

	imshow("��ȡ���", bg);
	//imwrite("C:\\Users\\liuzhongtian\\Desktop\\2\\test0.jpg", bg);
	//double timeconsume = (getTickCount() - t) / getTickFrequency();
	//printf("time consume %.3f", timeconsume);


	waitKey(0);

	return 0;

}

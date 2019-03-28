#include "cellgui.h"

using namespace cv;
using namespace std;

QImage cvMat2QImage(const Mat& mat);
Mat QImage2cvMat(QImage image);

void cellgui::Pic_detect()
{
	QImage img = imgLabel->pixmap()->toImage();     // ��ȡͼ��
	Mat src = QImage2cvMat(img);

	Mat gray_src;//ת�Ҷ�
	cvtColor(src, gray_src, CV_BGR2GRAY);

	Mat blur_src;//�˲�
	bilateralFilter(gray_src, blur_src, 7, 150, 50);
	//��Ե����
	Mat x;
	Mat y;
	Mat fin;
	Sobel(blur_src, x, CV_16S, 1, 0, 3);
	Sobel(blur_src, y, CV_16S, 0, 1, 3);
	convertScaleAbs(x, x);
	convertScaleAbs(y, y);

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

	//���ͣ������ʹ�ã�һ������ϸ����Ե�뱳����Ϊ�ӽ���
	Mat str0;
	str0 = getStructuringElement(MORPH_RECT, Size(4, 4), Point(-1, -1));
	dilate(fin, fin, str0, Point(-1, -1), 1);//����
	erode(fin, fin, str0, Point(-1, -1), 1);//��ʴ

	//����+���
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(fin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());//��������
	Mat ddst = Mat::zeros(src.size(), gray_src.type());
	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 50) drawContours(ddst, contours, i, Scalar(255, 255, 255), -1);
	}
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

	//���ͣ������ʹ�ã�һ������ϸ����Ե�뱳����Ϊ�ӽ���
	Mat str;
	str = getStructuringElement(MORPH_RECT, Size(6, 6), Point(-1, -1));
	dilate(dstBw, dstBw, str, Point(-1, -1), 1);

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
			char file_name[] = "D:\\SRTP\\test-split\\test0000.jpg";
			//sprintf_s(file_name, "D:\\SRTP\\test-split\\test%d.jpg", int(i + 1));
			copyMakeBorder(src, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//����ԭͼ������Ե
			//imwrite(file_name, src_ex(rect + Point(8, 8)));
		}
	}

	QImage imagesrc = cvMat2QImage(bg);
	imgLabel->setPixmap(QPixmap::fromImage(imagesrc));
	G0 = src;
	G = dstBw;
}

// cv::Matת����QImage
QImage cvMat2QImage(const Mat& mat)
{
	if (mat.type() == CV_8UC1)                          // ��ͨ��
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);                       // �Ҷȼ���256
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;                         // ����mat����
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)                     // 3ͨ��
	{
		const uchar *pSrc = (const uchar*)mat.data;     // ��������
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);    // R, G, B ��Ӧ 0,1,2
		return image.rgbSwapped();                      // rgbSwapped��Ϊ����ʾЧ��ɫ�ʺ�һЩ��
	}
	else if (mat.type() == CV_8UC4)                     // 4ͨ��
	{
		const uchar *pSrc = (const uchar*)mat.data;     // ��������
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);        // B,G,R,A ��Ӧ 0,1,2,3
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

Mat QImage2cvMat(QImage image)//QImageת����Mat
{
	Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

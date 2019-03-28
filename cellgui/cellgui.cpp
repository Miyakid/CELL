#include "cellgui.h"

using namespace cv;
using namespace std;

QImage cvMat2QImage(const Mat& mat);
Mat QImage2cvMat(QImage image);
Mat G0;
Mat G;

cellgui::cellgui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// �����С
	setWindowTitle("	CELL	");

	// ���ͼ��
	setWindowIcon(QIcon("../Image/icon/ϸ��_cells.png"));

	// �����С
	resize(QApplication::desktop()->width()*0.5, QApplication::desktop()->height()*0.7);
	move(QApplication::desktop()->width()*0.2, QApplication::desktop()->height()*0.1);

	Menu_File();        // �ļ��˵�
	InitImage();        // ��ʼ��ͼ��QLabel

}

cellgui::~cellgui()
{
}

void cellgui::Menu_File()
{
	// �˵���
	QMenu *file = menuBar()->addMenu(tr("�ļ�"));

	QAction *Act_pic_open = new QAction(QIcon("../Image/file/�ļ�.png"), tr("��ͼ��"), this);
	Act_pic_open->setShortcuts(QKeySequence::Open);// ��ݼ� Ctrl+O
	connect(Act_pic_open, SIGNAL(triggered()), this, SLOT(Pic_open()));

	QAction *Act_pic_detect = new QAction(QIcon("../Image/file/������.png"), tr("ϸ��ʶ��"), this);
	Act_pic_detect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));// ��ݼ�
	connect(Act_pic_detect, SIGNAL(triggered()), this, SLOT(Pic_detect()));

	QAction *Act_pic_saveas = new QAction(QIcon("../Image/file/��.png"), tr("�����ע��ͼ��"), this);
	Act_pic_saveas->setShortcuts(QKeySequence::Save);// ��ݼ� 
	connect(Act_pic_saveas, SIGNAL(triggered()), this, SLOT(Pic_saveas()));

	QAction *Act_pic_save = new QAction(QIcon("../Image/file/����.png"), tr("����ָ��ͼ��"), this);
	Act_pic_save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));// ��ݼ�
	connect(Act_pic_save, SIGNAL(triggered()), this, SLOT(Pic_save()));

	QAction *Act_file_close = new QAction(QIcon("../Image/file/���.png"), tr("�ر�"), this);
	Act_file_close->setShortcuts(QKeySequence::Close);// ��ݼ�
	connect(Act_file_close, SIGNAL(triggered()), this, SLOT(close()));

	QAction *Act_pic_autobatch = new QAction(QIcon("../Image/file/����.png"), tr("�Զ�������"), this);
	Act_pic_autobatch->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));// ��ݼ�
	connect(Act_pic_autobatch, SIGNAL(triggered()), this, SLOT(autobatch()));

	// ��������ӵ��˵���
	file->addAction(Act_pic_open);
	file->addAction(Act_pic_detect);
	file->addAction(Act_pic_saveas);
	file->addAction(Act_pic_save);
	file->addSeparator();                       //��ӷָ���
	file->addAction(Act_pic_autobatch);
	file->addSeparator();                       //��ӷָ���
	file->addAction(Act_file_close);

	// ������
	ui.mainToolBar->addAction(Act_pic_open);
	ui.mainToolBar->addAction(Act_pic_detect);
	ui.mainToolBar->addAction(Act_pic_saveas);
	ui.mainToolBar->addAction(Act_pic_save);
	ui.mainToolBar->addAction(Act_pic_autobatch);


	// ������
	Act_pic_open->setStatusTip(tr("��ͼ��"));
	Act_pic_detect->setStatusTip(tr("ϸ��ʶ��"));
	Act_pic_save->setStatusTip(tr("����ָ��ͼ��"));
	Act_pic_saveas->setStatusTip(tr("�����ע��ͼ��"));
	Act_file_close->setStatusTip(tr("�ر����"));
	Act_pic_autobatch->setStatusTip(tr("�Զ�������"));


}


void cellgui::Pic_open()
{
	QString path = QFileDialog::getOpenFileName(this, tr("ѡ��ͼ��"), ".", tr("Images(*.jpg *.png *.bmp)"));                            // �ļ�ѡ���
	if (!path.isEmpty())                                    // ��⵱ǰ·���Ƿ���ȷ
	{
		QImage* img = new QImage();
		if (!(img->load(path)))
		{
			QMessageBox::information(this, tr("����"), tr("��ͼ��ʧ�ܣ�"));
			delete img;
			return;
		}
		imgLabel->setPixmap(QPixmap::fromImage(*img));
		imgLabel->resize(img->width(), img->height());
		currentPath = path;
	}
}

void cellgui::Pic_saveas()      
{
	QString path = QFileDialog::getSaveFileName(this, tr("ͼ�񱣴浽"), ".", tr("Images(*.jpg *.png *.bmp)"));
	if (!path.isEmpty())
	{
		QImage img = imgLabel->pixmap()->toImage();
		img.save(path);
		currentPath = path;
	}
}

void cellgui::Pic_save()
{
	QString path = QFileDialog::getExistingDirectory(
		this,
		tr("����Ŀ¼"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	string paths = path.toStdString();

	if (!path.isEmpty())
	{
		//�������֡����ơ���ȡ������
		vector<vector<Point>> contours1;
		vector<Vec4i> hierarchy1;
		findContours(G, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//��������

		string file_name;
		string str1;
		for (size_t i = 0; i < contours1.size(); i++) {
			if (contours1[i].size() > 20) {
				//��ȡ�������򱣴�
				Mat src_ex;
				Rect rect = boundingRect(contours1[i]);
				rect = rect + Point(-3, -3);	//ƽ��
				rect = rect + Size(6, 6);	    //���ţ����϶��㲻��

				stringstream ss1, ss2;
				ss1 << i;
				ss1 >> str1;
				file_name = paths + "/out" + str1 + ".jpg";
				copyMakeBorder(G0, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//����ԭͼ������Ե
				imwrite(file_name, src_ex(rect + Point(8, 8)));
			}
		}
		currentPath = path;
	}
}


void cellgui::InitImage()        // ��ʼ��ͼ��
{
	// ��ʼ��QDockWidget.���Ժ�ὲ�����ǿ��ƶ����ص�С���ڣ�
	// ����ʵ��PS��VSͣ�����ڵ�Ч����Ŀǰֻ���˽�
	dock_Image = new QDockWidget(tr("ͼ��"), this);          //   ͼ��
	setCentralWidget(dock_Image);

	// ��ʼ��QLabel
	imgLabel = new QLabel(dock_Image);
	imgLabel->setScaledContents(true);  // ����QLabel�Զ���Ӧͼ���С

										// ��ʼͼ��
	QImage image = QImage(600, 550, QImage::Format_RGB32);  // �½�ͼ��
	image.fill(qRgb(255, 255, 255));                        // ȫ��
	imgLabel->setPixmap(QPixmap::fromImage(image));         // ��ʾͼ��
	imgLabel->resize(image.width(), image.height());        // ͼ����imgLabelͬ��С

															// ���ӹ�����,���ͼ���imgLabel�󣬾ͻ���ֹ�����
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setWidget(imgLabel);
	dock_Image->setWidget(scrollArea);
}


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
		}
	}

	QImage imagesrc = cvMat2QImage(bg);
	imgLabel->setPixmap(QPixmap::fromImage(imagesrc));
	src.copyTo(G0);
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

void cellgui::autobatch()
{
	QString pathname = QFileDialog::getExistingDirectory(
		this,
		tr("ѡ��������Դͼ��Ŀ¼"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	QDir dir(pathname);
	if (!dir.exists())  return;
	QDir dirs1, dirs2;
	QString dirs1_str, dirs2_str;
	dirs1_str = pathname + "/" + "marked pics";
	dirs2_str = pathname + "/" + "split pics";
	dirs1.mkpath(dirs1_str);
	dirs2.mkpath(dirs2_str);

	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QStringList filters;  filters << QString("*.png");
	dir.setNameFilters(filters);
	int dir_cout = dir.count();
	cout << dir_cout << endl;
	if (dir_cout <= 0)  return;

	QChar separator = QChar('/');
	if (!pathname.contains(separator))
	{
		separator = QChar('\\');
	}
	QChar last_char = pathname.at(pathname.length() - 1);
	if (last_char == separator)
	{
		separator = QChar();
	}

	string path1 = dirs1_str.toStdString();
	string path2 = dirs2_str.toStdString();
	string pic_name1;
	string pic_name2;
	string str1,str2;
	Mat Image;

	QTime time;
	time.start();
	for (int i = 0; i<dir_cout; i++)
	{

		QString file_name = dir[i];  //�ļ�����
		QString file_path = pathname + separator + file_name;   //�ļ�ȫ·��

		//��ʾԭͼ
		QImage* img = new QImage();
		if (!(img->load(file_path)))
		{
			QMessageBox::information(this, tr("����"), tr("��ͼ��ʧ�ܣ�"));
			delete img;
			return;
		}
		imgLabel->setPixmap(QPixmap::fromImage(*img));
		imgLabel->resize(img->width(), img->height());
		currentPath = file_path;

		//ϸ��ʶ��
		Pic_detect();

		//�����עͼ��
		stringstream ss1;
		ss1 << i;
		ss1 >> str1;
		pic_name1 = path1 + "/out" + str1 + ".jpg";
		QImage image = imgLabel->pixmap()->toImage();
		Image = QImage2cvMat(image);
		imwrite(pic_name1, Image);
		currentPath = file_path;

		//�����ȡͼ��
		vector<vector<Point>> contours1;
		vector<Vec4i> hierarchy1;
		findContours(G, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//��������

		for (size_t j = 0; j < contours1.size(); j++) {
			if (contours1[j].size() > 20) {
				//��ȡ�������򱣴�
				Mat src_ex;
				Rect rect = boundingRect(contours1[j]);
				rect = rect + Point(-3, -3);	//ƽ��
				rect = rect + Size(6, 6);	    //���ţ����϶��㲻��

				stringstream ss1,ss2;
				ss1 << i;
				ss2 << j;
				ss1 >> str1;
				ss2 >> str2;
				pic_name2 = path2 + "/result" + str1 + "-" +str2 + ".jpg";
				copyMakeBorder(G0, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//����ԭͼ������Ե
				imwrite(pic_name2, src_ex(rect + Point(8, 8)));
			}
		}

	}
	double a = time.elapsed() / 1000.0;
	QString b = QString::number(a, 10, 3) + "s";
	QString c = QString::number(dir_cout, 10, 0);
	QString d = "input pic-number: " + c + "; total time consume: " + b;
	QMessageBox::information(this, tr("Time consume"), d);
}

int ma()
{
	string fileName, grayFile;

	string str1, str2;
	Mat srcImage, grayImage;
	for (int i = 0; i <= 10; i++)
	{
		stringstream ss1, ss2;

		ss1 << i;
		ss1 >> str1;
		//cout << str1 << endl;
		fileName = "" + str1 + ".jpg";
		srcImage = imread(fileName);

		grayFile = "out" + str1 + ".jpg";
		cvtColor(srcImage, grayImage, CV_BGR2GRAY);
		imwrite(grayFile, grayImage);

	}

	system("pause");
	return 0;
}

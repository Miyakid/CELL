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

	// 标题大小
	setWindowTitle("	CELL	");

	// 软件图标
	setWindowIcon(QIcon("../Image/icon/细胞_cells.png"));

	// 界面大小
	resize(QApplication::desktop()->width()*0.5, QApplication::desktop()->height()*0.7);
	move(QApplication::desktop()->width()*0.2, QApplication::desktop()->height()*0.1);

	Menu_File();        // 文件菜单
	InitImage();        // 初始化图像QLabel

}

cellgui::~cellgui()
{
}

void cellgui::Menu_File()
{
	// 菜单栏
	QMenu *file = menuBar()->addMenu(tr("文件"));

	QAction *Act_pic_open = new QAction(QIcon("../Image/file/文件.png"), tr("打开图像"), this);
	Act_pic_open->setShortcuts(QKeySequence::Open);// 快捷键 Ctrl+O
	connect(Act_pic_open, SIGNAL(triggered()), this, SLOT(Pic_open()));

	QAction *Act_pic_detect = new QAction(QIcon("../Image/file/计算器.png"), tr("细胞识别"), this);
	Act_pic_detect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));// 快捷键
	connect(Act_pic_detect, SIGNAL(triggered()), this, SLOT(Pic_detect()));

	QAction *Act_pic_saveas = new QAction(QIcon("../Image/file/包.png"), tr("保存标注后图像"), this);
	Act_pic_saveas->setShortcuts(QKeySequence::Save);// 快捷键 
	connect(Act_pic_saveas, SIGNAL(triggered()), this, SLOT(Pic_saveas()));

	QAction *Act_pic_save = new QAction(QIcon("../Image/file/保存.png"), tr("保存分割后图像"), this);
	Act_pic_save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));// 快捷键
	connect(Act_pic_save, SIGNAL(triggered()), this, SLOT(Pic_save()));

	QAction *Act_file_close = new QAction(QIcon("../Image/file/错的.png"), tr("关闭"), this);
	Act_file_close->setShortcuts(QKeySequence::Close);// 快捷键
	connect(Act_file_close, SIGNAL(triggered()), this, SLOT(close()));

	QAction *Act_pic_autobatch = new QAction(QIcon("../Image/file/播放.png"), tr("自动批处理"), this);
	Act_pic_autobatch->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));// 快捷键
	connect(Act_pic_autobatch, SIGNAL(triggered()), this, SLOT(autobatch()));

	// 将动作添加到菜单上
	file->addAction(Act_pic_open);
	file->addAction(Act_pic_detect);
	file->addAction(Act_pic_saveas);
	file->addAction(Act_pic_save);
	file->addSeparator();                       //添加分割线
	file->addAction(Act_pic_autobatch);
	file->addSeparator();                       //添加分割线
	file->addAction(Act_file_close);

	// 工具栏
	ui.mainToolBar->addAction(Act_pic_open);
	ui.mainToolBar->addAction(Act_pic_detect);
	ui.mainToolBar->addAction(Act_pic_saveas);
	ui.mainToolBar->addAction(Act_pic_save);
	ui.mainToolBar->addAction(Act_pic_autobatch);


	// 任务栏
	Act_pic_open->setStatusTip(tr("打开图像"));
	Act_pic_detect->setStatusTip(tr("细胞识别"));
	Act_pic_save->setStatusTip(tr("保存分割后图像"));
	Act_pic_saveas->setStatusTip(tr("保存标注后图像"));
	Act_file_close->setStatusTip(tr("关闭软件"));
	Act_pic_autobatch->setStatusTip(tr("自动批处理"));


}


void cellgui::Pic_open()
{
	QString path = QFileDialog::getOpenFileName(this, tr("选择图像"), ".", tr("Images(*.jpg *.png *.bmp)"));                            // 文件选择框
	if (!path.isEmpty())                                    // 检测当前路径是否正确
	{
		QImage* img = new QImage();
		if (!(img->load(path)))
		{
			QMessageBox::information(this, tr("错误"), tr("打开图像失败！"));
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
	QString path = QFileDialog::getSaveFileName(this, tr("图像保存到"), ".", tr("Images(*.jpg *.png *.bmp)"));
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
		tr("保存目录"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	string paths = path.toStdString();

	if (!path.isEmpty())
	{
		//轮廓发现、绘制、截取、保存
		vector<vector<Point>> contours1;
		vector<Vec4i> hierarchy1;
		findContours(G, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//发现轮廓

		string file_name;
		string str1;
		for (size_t i = 0; i < contours1.size(); i++) {
			if (contours1[i].size() > 20) {
				//割取矩形区域保存
				Mat src_ex;
				Rect rect = boundingRect(contours1[i]);
				rect = rect + Point(-3, -3);	//平移
				rect = rect + Size(6, 6);	    //缩放，左上顶点不变

				stringstream ss1, ss2;
				ss1 << i;
				ss1 >> str1;
				file_name = paths + "/out" + str1 + ".jpg";
				copyMakeBorder(G0, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//扩大原图像并填充边缘
				imwrite(file_name, src_ex(rect + Point(8, 8)));
			}
		}
		currentPath = path;
	}
}


void cellgui::InitImage()        // 初始化图像
{
	// 初始化QDockWidget.在以后会讲到，是可移动隐藏的小窗口，
	// 可以实现PS、VS停靠窗口的效果，目前只需了解
	dock_Image = new QDockWidget(tr("图像"), this);          //   图像
	setCentralWidget(dock_Image);

	// 初始化QLabel
	imgLabel = new QLabel(dock_Image);
	imgLabel->setScaledContents(true);  // 设置QLabel自动适应图像大小

										// 初始图像
	QImage image = QImage(600, 550, QImage::Format_RGB32);  // 新建图像
	image.fill(qRgb(255, 255, 255));                        // 全白
	imgLabel->setPixmap(QPixmap::fromImage(image));         // 显示图像
	imgLabel->resize(image.width(), image.height());        // 图像与imgLabel同大小

															// 增加滚动条,如果图像比imgLabel大，就会出现滚动条
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setWidget(imgLabel);
	dock_Image->setWidget(scrollArea);
}


void cellgui::Pic_detect()
{
	QImage img = imgLabel->pixmap()->toImage();     // 读取图像
	Mat src = QImage2cvMat(img);

	Mat gray_src;//转灰度
	cvtColor(src, gray_src, CV_BGR2GRAY);

	Mat blur_src;//滤波
	bilateralFilter(gray_src, blur_src, 7, 150, 50);
	//边缘发现
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
	threshold(fin, fin, /*230*/10/*（复杂细胞外形）most_num*/, 255, THRESH_BINARY);

	//膨胀（看情况使用，一般用于细胞边缘与背景极为接近）
	Mat str0;
	str0 = getStructuringElement(MORPH_RECT, Size(4, 4), Point(-1, -1));
	dilate(fin, fin, str0, Point(-1, -1), 1);//膨胀
	erode(fin, fin, str0, Point(-1, -1), 1);//腐蚀

											//轮廓+填充
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(fin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());//轮廓发现
	Mat ddst = Mat::zeros(src.size(), gray_src.type());
	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 50) drawContours(ddst, contours, i, Scalar(255, 255, 255), -1);
	}
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

	//膨胀（看情况使用，一般用于细胞边缘与背景极为接近）
	Mat str;
	str = getStructuringElement(MORPH_RECT, Size(6, 6), Point(-1, -1));
	dilate(dstBw, dstBw, str, Point(-1, -1), 1);

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
		}
	}

	QImage imagesrc = cvMat2QImage(bg);
	imgLabel->setPixmap(QPixmap::fromImage(imagesrc));
	src.copyTo(G0);
	G = dstBw;
}


// cv::Mat转换成QImage
QImage cvMat2QImage(const Mat& mat)
{
	if (mat.type() == CV_8UC1)                          // 单通道
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);                       // 灰度级数256
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;                         // 复制mat数据
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)                     // 3通道
	{
		const uchar *pSrc = (const uchar*)mat.data;     // 复制像素
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);    // R, G, B 对应 0,1,2
		return image.rgbSwapped();                      // rgbSwapped是为了显示效果色彩好一些。
	}
	else if (mat.type() == CV_8UC4)                     // 4通道
	{
		const uchar *pSrc = (const uchar*)mat.data;     // 复制像素
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);        // B,G,R,A 对应 0,1,2,3
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

Mat QImage2cvMat(QImage image)//QImage转换成Mat
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
		tr("选择批处理源图像目录"),
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

		QString file_name = dir[i];  //文件名称
		QString file_path = pathname + separator + file_name;   //文件全路径

		//显示原图
		QImage* img = new QImage();
		if (!(img->load(file_path)))
		{
			QMessageBox::information(this, tr("错误"), tr("打开图像失败！"));
			delete img;
			return;
		}
		imgLabel->setPixmap(QPixmap::fromImage(*img));
		imgLabel->resize(img->width(), img->height());
		currentPath = file_path;

		//细胞识别
		Pic_detect();

		//保存标注图像
		stringstream ss1;
		ss1 << i;
		ss1 >> str1;
		pic_name1 = path1 + "/out" + str1 + ".jpg";
		QImage image = imgLabel->pixmap()->toImage();
		Image = QImage2cvMat(image);
		imwrite(pic_name1, Image);
		currentPath = file_path;

		//保存割取图像
		vector<vector<Point>> contours1;
		vector<Vec4i> hierarchy1;
		findContours(G, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());//发现轮廓

		for (size_t j = 0; j < contours1.size(); j++) {
			if (contours1[j].size() > 20) {
				//割取矩形区域保存
				Mat src_ex;
				Rect rect = boundingRect(contours1[j]);
				rect = rect + Point(-3, -3);	//平移
				rect = rect + Size(6, 6);	    //缩放，左上顶点不变

				stringstream ss1,ss2;
				ss1 << i;
				ss2 << j;
				ss1 >> str1;
				ss2 >> str2;
				pic_name2 = path2 + "/result" + str1 + "-" +str2 + ".jpg";
				copyMakeBorder(G0, src_ex, 8, 8, 8, 8, BORDER_CONSTANT, Scalar(255, 255, 255));	//扩大原图像并填充边缘
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

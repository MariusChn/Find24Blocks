#include"Find24Blocks.h"
/*
	Description:自定义的工具类函数，用来判别下一个Rect类型是否不在数组之中
*/
bool IsNotRectRepeat(vector<Rect> rcts, Rect newRct) {
	bool result = true;
	if (rcts.empty()) return result;
	else {
		for (int i = 0/*rcts.size() - 1*/; i < rcts.size()/*>= 0*/; i++) {
			//if (i == rcts.size() - 6) break;
			if ((abs(rcts[i].x - newRct.x) < 15) & (abs(rcts[i].y - newRct.y) < 15)) {
				//用于观察第一轮筛选后的结果
				//cout << "删掉" << "(" << newRct.x << "," << newRct.y << ")" << "点";
				result = false;
				break;
			}
			else continue;
		}
	}
	return result;
}
/*
	Description:自定义工具类，根据输入的Rect类型数组，
	筛选出呈阵列分布的Rect数组
*/
vector<Rect> GetEligibleBlock(vector<Rect> iniRect) {
	vector<Rect> rslRect;
	for (int i = 0; i < iniRect.size(); i++) {
		int bndMinX = iniRect[i].x - 0.5*iniRect[i].width;
		int bndMaxX = iniRect[i].x + 0.5*iniRect[i].width;
		int bndMinY = iniRect[i].y - 0.5*iniRect[i].height;
		int bndMaxY = iniRect[i].y + 0.5*iniRect[i].height;
		int xNm = 0; int yNm = 0;
		for (int j = 0; j < iniRect.size(); j++) {
			if (iniRect[j].x >= bndMinX && iniRect[j].x <= bndMaxX)
				xNm++;
			if (iniRect[j].y >= bndMinY && iniRect[j].y <= bndMaxY)
				yNm++;
		}
		if (xNm >= 3 && yNm >= 4) {
			rslRect.push_back(iniRect[i]);
		}
	}
	return rslRect;
}
/*
	Description:工具方法，自定义的比较函数
*/
bool cmp(Rect a, Rect b)
{
	return a.x - b.x < 0;
}
/*
	Description:工具方法，用于将色板上没有找到的位置找到
*/
vector<Rect> FillUpVector(vector<Rect> r, vector<Rect> stdr) {
	Rect replaceRect;
	for (int i = 0; i < stdr.size(); i++) {
		if (i > r.size() - 1) {
			replaceRect.x = stdr[i].x;
			replaceRect.y = r[i - 1].y;
			replaceRect.width = stdr[i].width;
			replaceRect.height = stdr[i].height;
			r.push_back(replaceRect);
			continue;
		}
		int pace = 0.7*max(r[i].width, stdr[i].width);
		if (r[i].x > stdr[i].x - pace && r[i].x < stdr[i].x + pace) continue;
		else {
			replaceRect.x = stdr[i].x;
			replaceRect.y = r[i].y;
			replaceRect.width = stdr[i].width;
			replaceRect.height = stdr[i].height;
			r.insert(r.begin() + i, replaceRect);
		}
	}
	//验证此函数是否功能正确，勿删！
	/*cout << endl;
	cout << "大小是：" << r.size() << endl;*/
	return r;
}
/*
	Description:
	工具函数；
	对Rect集中的每一个Rectangle做收缩处理
*/
void Shrink(Rect *srcRects) {
	srcRects->x += 0.3*srcRects->width;
	srcRects->y += 0.3*srcRects->height;
	srcRects->width *= 0.4;
	srcRects->height *= 0.4;
}
/*
	Description:工具函数，用于求出三点所连直线之间夹角的余弦值
*/
double GetCosine(Point p0, Point p1, Point p2) {
	double dx1 = p1.x - p0.x;
	double dx2 = p2.x - p0.x;
	double dy1 = p1.y - p0.y;
	double dy2 = p2.y - p0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}
//计算图片在一个矩形框下的平均RGB值，并以scalar的形式返回
Scalar CalculateAverageRGB(Mat resizedImage, Rect r) {
	Mat inputImg;
	inputImg = resizedImage;
	Scalar result;
	int pixelNum = 0;
	result[0] = result[1] = result[2] = 0;
	for (int i = r.x; i < r.x + r.width; i++) {
		for (int j = r.y; j < r.y + r.width; j++) {
			uchar *p_img = inputImg.ptr<uchar>(j, i);
			pixelNum++;
			result[0] += p_img[2];
			result[1] += p_img[1];
			result[2] += p_img[0];
		}
	}
	result[0] = (int)result[0] / pixelNum;
	result[1] = (int)result[1] / pixelNum;
	result[2] = (int)result[2] / pixelNum;
	if (result[0] > 255 || result[1] > 255 || result[2] > 255)
		cout << "计算的平均RGB值有误！请重新检查代码" << endl;
	return result;
}
/*
	Description:将图片初始化处理，输出初始化后的图像
*/
Mat GetProcessedIage(Mat sourceImage) {
	Mat aftSizedImg = sourceImage;
	Mat gryImg;
	Mat aftCnnImage;
	cvtColor(aftSizedImg, gryImg, CV_RGB2GRAY);		//转灰度图

	//自适应阈值分割
	Mat AdthrshImg;		//初始化自适应阈值参数
	const int maxVal = 255;
	int blockSize = 21;	//取值3、5、7....等
	int constValue = 10;
	int adaptiveMethod = 0;
	int thresholdType = 1;
	adaptiveThreshold(gryImg, AdthrshImg, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
	//imwrite("afterAdptvGauss.jpg", AdthrshImg);

	//膨胀后腐蚀
	Mat DeImg;
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat kernel2 = getStructuringElement(MORPH_RECT, Size(15, 15));
	dilate(AdthrshImg, DeImg, kernel1);
	erode(DeImg, DeImg, kernel2);
	//imwrite("afterDeImg17.jpg", DeImg);

	//边缘检测描绘出轮廓
	Canny(DeImg, aftCnnImage, 100, 150);
	//imwrite("afterCnnImg.jpg", cnnImg);
	return aftCnnImage;
}
/*
	Description:从图像中取得色板上的非完整框,并显示出每层经筛选后得出的矩形框的信息
*/
vector<Rect> GetRoughBlocks(Mat processedImage) {
	Mat inputImg = processedImage;
	//轮廓提取
	vector<vector<Point>> contours;
	vector<Point> approx;
	vector<Rect> rectSqrs;
	Mat contourImg = inputImg.clone();
	findContours(contourImg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	int count1 = 0;
	int count = 0;
	int counttt = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		//记录轮廓数
		count1++;
		//用来验证，勿删
		//if (count1 == contours.size()) cout << "找到了" << count1 << "个轮廓" << endl;
		//逼近点集
		approxPolyDP(contours[i], approx, 9, true);
		if (approx.size() == 4 && fabs(contourArea(approx)) > 200 && isContourConvex(approx)) {
			count++;
			//用来验证第一轮筛选得出的二元组，勿删！
			//cout << "(" << approx[0].x << "," << approx[0].y << ")";
			Rect painting = boundingRect(approx);
			double maxCosine = 0;
			for (int j = 2; j < 5; j++) {
				double cosine = fabs(GetCosine(approx[j - 1], approx[j % 4], approx[j - 2]));
				maxCosine = max(maxCosine, cosine);
			}
			if (maxCosine < 0.2
				&& (float)painting.width / painting.height >= 0.8
				&& (float)painting.width / painting.height <= 1.2
				&& IsNotRectRepeat(rectSqrs, painting)) {
				//rectangle(vrfImg1, painting, Scalar(0, 0, 255), 1);
				counttt++;
				//用来验证，勿删！
				//cout << "  " << "(" << painting.x << "," << painting.y << ")" << endl;	//检验找到的色块左上角的位置坐标（唯一输出）
				rectSqrs.push_back(painting);
			}
		}
	}
	cout << "经过顶点、面积、凹凸判断后选出" << count << "个轮廓" << endl;
	cout << "再经过矩形、正方形、去重判断后选出" << counttt << "个轮廓" << endl;
	return rectSqrs;
}
/*
	Description:把有少量残缺的色板框补全为24个
*/
vector<Rect> GetCompleteBlocks_24(vector<Rect> roughRects, Mat afterResizedImage) {
	vector<Rect> inputRect = roughRects;
	vector<int> i;
	for (int j = 0; j < inputRect.size() - 1; j++) {
		if (i.size() == 3) break;
		if (abs(inputRect[j].y - inputRect[j + 1].y) > 0.5*max(inputRect[j].height, inputRect[j].height)) {
			i.push_back(j);
			if (i.size() > 3) cout << "出错，色板过度倾斜，或判断设置不合理！" << endl;
		}
	}
	cout << "行相交的地方位于：" << i[0] << " " << i[1] << " " << i[2] << endl;
	vector<Rect> r3;
	vector<Rect> r2;
	vector<Rect> r1;
	vector<Rect> r0;

	//将数组的应属于每一行的色块对号入行
	for (int k = 0; k < inputRect.size(); k++) {
		if (k <= i[0]) r3.push_back(inputRect[k]);
		else if (k <= i[1]) r2.push_back(inputRect[k]);
		else if (k <= i[2]) r1.push_back(inputRect[k]);
		else r0.push_back(inputRect[k]);
	}
	//显示原始数组
	cout << "原始的数组如下：" << endl;
	for (int k = 0; k < inputRect.size(); k++) {
		cout << "(" << inputRect[k].x << "," << inputRect[k].y << ")" << " ";
		if (k == i[0] || k == i[1] || k == i[2]) cout << endl;
	}

	//在数组中排序
	sort(r0.begin(), r0.end(), cmp);
	sort(r1.begin(), r1.end(), cmp);
	sort(r2.begin(), r2.end(), cmp);
	sort(r3.begin(), r3.end(), cmp);
	cout << '\n' << "排序后的数组如下：" << endl;
	for (int k = 0; k < r0.size(); k++) cout << "(" << r0[k].x << "," << r0[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r1.size(); k++) cout << "(" << r1[k].x << "," << r1[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r2.size(); k++) cout << "(" << r2[k].x << "," << r2[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r3.size(); k++) cout << "(" << r3[k].x << "," << r3[k].y << ")" << " ";
	cout << r0.size() << " " << r1.size() << " " << r2.size() << " " << r3.size() << endl;
	//找出有有6个元素的数组
	vector<Rect> stdRow;
	if (r0.size() == 6) stdRow = r0;
	else if (r1.size() == 6) stdRow = r1;
	else if (r2.size() == 6) stdRow = r2;
	else if (r3.size() == 6) stdRow = r3;
	cout << "含有六个元素的数组大小是" << stdRow.size() << ":" << endl;
	for (int j = 0; j < stdRow.size(); j++) {
		cout << "(" << stdRow[j].x << "," << stdRow[j].y << ")" << " ";
	}
	vector<Rect> prp3;
	vector<Rect> prp2;
	vector<Rect> prp1;
	vector<Rect> prp0;
	if (r0.size() != 6) r0 = FillUpVector(r0, stdRow);
	if (r1.size() != 6) r1 = FillUpVector(r1, stdRow);
	if (r2.size() != 6) r2 = FillUpVector(r2, stdRow);
	if (r3.size() != 6) r3 = FillUpVector(r3, stdRow);
	//分别显示每行
	Mat resizeImage = afterResizedImage;
	Mat vrfImg2 = resizeImage.clone();
	cout << endl;

	//以下代码负责分别显示每一行的（x，y）值
	//for (int k = 0; k < r0.size(); k++) {
	//	cout << "(" << r0[k].x << "," << r0[k].y << ")" << " ";
	//	rectangle(vrfImg2, r0[k], Scalar(0, 255, 0), 1);
	//}
	//cout << endl;
	//for (int k = 0; k < r1.size(); k++) {
	//	cout << "(" << r1[k].x << "," << r1[k].y << ")" << " ";
	//	rectangle(vrfImg2, r1[k], Scalar(0, 255, 0), 1);
	//}
	//cout << endl;
	//for (int k = 0; k < r2.size(); k++) {
	//	cout << "(" << r2[k].x << "," << r2[k].y << ")" << " ";
	//	rectangle(vrfImg2, r2[k], Scalar(0, 255, 0), 1);
	//}
	//cout << endl;
	//for (int k = 0; k < r3.size(); k++) {
	//	cout << "(" << r3[k].x << "," << r3[k].y << ")" << " ";
	//	rectangle(vrfImg2, r3[k], Scalar(0, 255, 0), 1);
	//}
	//cout << endl;
	vector<Rect> completeRect;
	completeRect.insert(completeRect.end(), r0.begin(), r0.end());
	completeRect.insert(completeRect.end(), r1.begin(), r1.end());
	completeRect.insert(completeRect.end(), r2.begin(), r2.end());
	completeRect.insert(completeRect.end(), r3.begin(), r3.end());
	cout << "补全数组后，其元素显示如下(未有显示宽和高)：" << endl;
	//验证数组完整性
	if (completeRect.size() != 24) {
		cout << "残缺的数组处理失败，请查看处理过程或者重新拍摄" << endl;
	}
	//用于展示
	for (int j = 0; j < completeRect.size(); j++) {
		Shrink(&completeRect[j]);
		rectangle(vrfImg2, completeRect[j], Scalar(0, 255, 0), 1);
		cout << "(" << completeRect[j].x << "," << completeRect[j].y << ")" << " ";
		if (j == 5 || j == 11 || j == 17) cout << endl;
	}
	imwrite("24_complete_blockImg.jpg", vrfImg2);
	return completeRect;
}

/*
	Description:从任何一张图片中找到色板上的24色块位置，
	并以数组的形式输出他们的RGB平均值
*/
vector<Scalar> Find24Blocks(Mat orgImage) {
	Mat originalImage = orgImage;
	//降低图像的分辨率以加快处理速度
	double fscale = 0.2;
	Size outsize;
	Mat resizeImage;
	outsize.width = originalImage.cols*fscale;
	outsize.height = originalImage.rows*fscale;
	resize(originalImage, resizeImage, outsize, 0, 0, INTER_AREA);

	//第一个模块得出处理后的图片
	Mat processedImg;
	processedImg = GetProcessedIage(resizeImage);

	//图片经过筛选得出色板上的矩形框
	Mat vrfImg1 = resizeImage.clone();
	vector<Rect> fltRect = GetEligibleBlock(GetRoughBlocks(processedImg));
	//用于输出验证信息及图片
	for (int i = 0; i < fltRect.size(); i++) {
		rectangle(vrfImg1, fltRect[i], Scalar(0, 255, 0), 1);
		//色板上rect框的(x,y)值
		//cout << "(" << fltRect[i].x << "," << fltRect[i].y << ")" << " ";
	}
	imwrite("EligibleBlock.jpg", vrfImg1);
	cout << "在色板上的的色块共有" << fltRect.size() << "个" << endl;
	//获取填满后的数组
	vector<Rect> finallyRects;
	finallyRects = GetCompleteBlocks_24(fltRect, resizeImage);
	//获得平均的RGB数组
	vector<Scalar> avrgRgbs;
	for (int i = 0; i < finallyRects.size(); i++) {
		avrgRgbs.push_back(CalculateAverageRGB(resizeImage, finallyRects[i]));
	}
	return avrgRgbs;
}
#include"Find24Blocks.h"
/*
	Description:�Զ���Ĺ����ຯ���������б���һ��Rect�����Ƿ�������֮��
*/
bool IsNotRectRepeat(vector<Rect> rcts, Rect newRct) {
	bool result = true;
	if (rcts.empty()) return result;
	else {
		for (int i = 0/*rcts.size() - 1*/; i < rcts.size()/*>= 0*/; i++) {
			//if (i == rcts.size() - 6) break;
			if ((abs(rcts[i].x - newRct.x) < 15) & (abs(rcts[i].y - newRct.y) < 15)) {
				//���ڹ۲��һ��ɸѡ��Ľ��
				//cout << "ɾ��" << "(" << newRct.x << "," << newRct.y << ")" << "��";
				result = false;
				break;
			}
			else continue;
		}
	}
	return result;
}
/*
	Description:�Զ��幤���࣬���������Rect�������飬
	ɸѡ�������зֲ���Rect����
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
	Description:���߷������Զ���ıȽϺ���
*/
bool cmp(Rect a, Rect b)
{
	return a.x - b.x < 0;
}
/*
	Description:���߷��������ڽ�ɫ����û���ҵ���λ���ҵ�
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
	//��֤�˺����Ƿ�����ȷ����ɾ��
	/*cout << endl;
	cout << "��С�ǣ�" << r.size() << endl;*/
	return r;
}
/*
	Description:
	���ߺ�����
	��Rect���е�ÿһ��Rectangle����������
*/
void Shrink(Rect *srcRects) {
	srcRects->x += 0.3*srcRects->width;
	srcRects->y += 0.3*srcRects->height;
	srcRects->width *= 0.4;
	srcRects->height *= 0.4;
}
/*
	Description:���ߺ��������������������ֱ��֮��нǵ�����ֵ
*/
double GetCosine(Point p0, Point p1, Point p2) {
	double dx1 = p1.x - p0.x;
	double dx2 = p2.x - p0.x;
	double dy1 = p1.y - p0.y;
	double dy2 = p2.y - p0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}
//����ͼƬ��һ�����ο��µ�ƽ��RGBֵ������scalar����ʽ����
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
		cout << "�����ƽ��RGBֵ���������¼�����" << endl;
	return result;
}
/*
	Description:��ͼƬ��ʼ�����������ʼ�����ͼ��
*/
Mat GetProcessedIage(Mat sourceImage) {
	Mat aftSizedImg = sourceImage;
	Mat gryImg;
	Mat aftCnnImage;
	cvtColor(aftSizedImg, gryImg, CV_RGB2GRAY);		//ת�Ҷ�ͼ

	//����Ӧ��ֵ�ָ�
	Mat AdthrshImg;		//��ʼ������Ӧ��ֵ����
	const int maxVal = 255;
	int blockSize = 21;	//ȡֵ3��5��7....��
	int constValue = 10;
	int adaptiveMethod = 0;
	int thresholdType = 1;
	adaptiveThreshold(gryImg, AdthrshImg, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
	//imwrite("afterAdptvGauss.jpg", AdthrshImg);

	//���ͺ�ʴ
	Mat DeImg;
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat kernel2 = getStructuringElement(MORPH_RECT, Size(15, 15));
	dilate(AdthrshImg, DeImg, kernel1);
	erode(DeImg, DeImg, kernel2);
	//imwrite("afterDeImg17.jpg", DeImg);

	//��Ե�����������
	Canny(DeImg, aftCnnImage, 100, 150);
	//imwrite("afterCnnImg.jpg", cnnImg);
	return aftCnnImage;
}
/*
	Description:��ͼ����ȡ��ɫ���ϵķ�������,����ʾ��ÿ�㾭ɸѡ��ó��ľ��ο����Ϣ
*/
vector<Rect> GetRoughBlocks(Mat processedImage) {
	Mat inputImg = processedImage;
	//������ȡ
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
		//��¼������
		count1++;
		//������֤����ɾ
		//if (count1 == contours.size()) cout << "�ҵ���" << count1 << "������" << endl;
		//�ƽ��㼯
		approxPolyDP(contours[i], approx, 9, true);
		if (approx.size() == 4 && fabs(contourArea(approx)) > 200 && isContourConvex(approx)) {
			count++;
			//������֤��һ��ɸѡ�ó��Ķ�Ԫ�飬��ɾ��
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
				//������֤����ɾ��
				//cout << "  " << "(" << painting.x << "," << painting.y << ")" << endl;	//�����ҵ���ɫ�����Ͻǵ�λ�����꣨Ψһ�����
				rectSqrs.push_back(painting);
			}
		}
	}
	cout << "�������㡢�������͹�жϺ�ѡ��" << count << "������" << endl;
	cout << "�پ������Ρ������Ρ�ȥ���жϺ�ѡ��" << counttt << "������" << endl;
	return rectSqrs;
}
/*
	Description:����������ȱ��ɫ���ȫΪ24��
*/
vector<Rect> GetCompleteBlocks_24(vector<Rect> roughRects, Mat afterResizedImage) {
	vector<Rect> inputRect = roughRects;
	vector<int> i;
	for (int j = 0; j < inputRect.size() - 1; j++) {
		if (i.size() == 3) break;
		if (abs(inputRect[j].y - inputRect[j + 1].y) > 0.5*max(inputRect[j].height, inputRect[j].height)) {
			i.push_back(j);
			if (i.size() > 3) cout << "����ɫ�������б�����ж����ò�����" << endl;
		}
	}
	cout << "���ཻ�ĵط�λ�ڣ�" << i[0] << " " << i[1] << " " << i[2] << endl;
	vector<Rect> r3;
	vector<Rect> r2;
	vector<Rect> r1;
	vector<Rect> r0;

	//�������Ӧ����ÿһ�е�ɫ��Ժ�����
	for (int k = 0; k < inputRect.size(); k++) {
		if (k <= i[0]) r3.push_back(inputRect[k]);
		else if (k <= i[1]) r2.push_back(inputRect[k]);
		else if (k <= i[2]) r1.push_back(inputRect[k]);
		else r0.push_back(inputRect[k]);
	}
	//��ʾԭʼ����
	cout << "ԭʼ���������£�" << endl;
	for (int k = 0; k < inputRect.size(); k++) {
		cout << "(" << inputRect[k].x << "," << inputRect[k].y << ")" << " ";
		if (k == i[0] || k == i[1] || k == i[2]) cout << endl;
	}

	//������������
	sort(r0.begin(), r0.end(), cmp);
	sort(r1.begin(), r1.end(), cmp);
	sort(r2.begin(), r2.end(), cmp);
	sort(r3.begin(), r3.end(), cmp);
	cout << '\n' << "�������������£�" << endl;
	for (int k = 0; k < r0.size(); k++) cout << "(" << r0[k].x << "," << r0[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r1.size(); k++) cout << "(" << r1[k].x << "," << r1[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r2.size(); k++) cout << "(" << r2[k].x << "," << r2[k].y << ")" << " ";
	cout << endl;
	for (int k = 0; k < r3.size(); k++) cout << "(" << r3[k].x << "," << r3[k].y << ")" << " ";
	cout << r0.size() << " " << r1.size() << " " << r2.size() << " " << r3.size() << endl;
	//�ҳ�����6��Ԫ�ص�����
	vector<Rect> stdRow;
	if (r0.size() == 6) stdRow = r0;
	else if (r1.size() == 6) stdRow = r1;
	else if (r2.size() == 6) stdRow = r2;
	else if (r3.size() == 6) stdRow = r3;
	cout << "��������Ԫ�ص������С��" << stdRow.size() << ":" << endl;
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
	//�ֱ���ʾÿ��
	Mat resizeImage = afterResizedImage;
	Mat vrfImg2 = resizeImage.clone();
	cout << endl;

	//���´��븺��ֱ���ʾÿһ�еģ�x��y��ֵ
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
	cout << "��ȫ�������Ԫ����ʾ����(δ����ʾ��͸�)��" << endl;
	//��֤����������
	if (completeRect.size() != 24) {
		cout << "��ȱ�����鴦��ʧ�ܣ���鿴������̻�����������" << endl;
	}
	//����չʾ
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
	Description:���κ�һ��ͼƬ���ҵ�ɫ���ϵ�24ɫ��λ�ã�
	�����������ʽ������ǵ�RGBƽ��ֵ
*/
vector<Scalar> Find24Blocks(Mat orgImage) {
	Mat originalImage = orgImage;
	//����ͼ��ķֱ����Լӿ촦���ٶ�
	double fscale = 0.2;
	Size outsize;
	Mat resizeImage;
	outsize.width = originalImage.cols*fscale;
	outsize.height = originalImage.rows*fscale;
	resize(originalImage, resizeImage, outsize, 0, 0, INTER_AREA);

	//��һ��ģ��ó�������ͼƬ
	Mat processedImg;
	processedImg = GetProcessedIage(resizeImage);

	//ͼƬ����ɸѡ�ó�ɫ���ϵľ��ο�
	Mat vrfImg1 = resizeImage.clone();
	vector<Rect> fltRect = GetEligibleBlock(GetRoughBlocks(processedImg));
	//���������֤��Ϣ��ͼƬ
	for (int i = 0; i < fltRect.size(); i++) {
		rectangle(vrfImg1, fltRect[i], Scalar(0, 255, 0), 1);
		//ɫ����rect���(x,y)ֵ
		//cout << "(" << fltRect[i].x << "," << fltRect[i].y << ")" << " ";
	}
	imwrite("EligibleBlock.jpg", vrfImg1);
	cout << "��ɫ���ϵĵ�ɫ�鹲��" << fltRect.size() << "��" << endl;
	//��ȡ�����������
	vector<Rect> finallyRects;
	finallyRects = GetCompleteBlocks_24(fltRect, resizeImage);
	//���ƽ����RGB����
	vector<Scalar> avrgRgbs;
	for (int i = 0; i < finallyRects.size(); i++) {
		avrgRgbs.push_back(CalculateAverageRGB(resizeImage, finallyRects[i]));
	}
	return avrgRgbs;
}
#include<iostream>
#include<vector>
#include"Find24Blocks.h"

using namespace std;

int main() {
	Mat srcImage = imread("F:\\2019暑期项目（华为）\\色号估计\\1.jpg");

	vector<Scalar> avrRgbs;
	avrRgbs = Find24Blocks(srcImage);

	//检验
	cout << endl;
	for (int i = 0; i < avrRgbs.size(); i++){
		cout << "(" << avrRgbs[i][0] << "," << avrRgbs[i][1] << "," << avrRgbs[i][2] << ")" << " ";
			if (i == 5 || i == 11 || i == 17) cout << endl;
	}
	getchar();
}
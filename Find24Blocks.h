#pragma once

#ifndef _FIND24BLOCK_H
#define _FIND24BLOCK_H


#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

vector<Scalar> Find24Blocks(Mat orgImage);

#endif // !_FIND24BLOCK_H
#pragma once
#include "stdafx.h"

int GetGray(BYTE *bytes,int w,int h,int strides,double positionX,double positionY)
{
	if (positionX>=0&&positionX<w&&
		positionY>=0&&positionY<h)
	{
		int x1 = int(positionX);
		int y1 = int(positionY);
		int x2 = x1+1;
		int y2 = y1+1;
		if (x2>=w||y2>=h)
		{
			return int(bytes[y1*strides+x1]);
		}
		double weight[4] = {0.0};
		weight[0] = (1-(positionX-x1))*(1-(positionY-y1));
		weight[1] = (1-(positionX-x1))*(positionY-y1);
		weight[2] = (positionX-x1)*(1-(positionY-y1));
		weight[3] = (positionX-x1)*(positionY-y1);
		return int(weight[0]*bytes[y1*strides+x1]+weight[1]*bytes[y2*strides+x1]+weight[2]*bytes[y1*strides+x2]+weight[3]*bytes[y2*strides+x2]);
	}
	else
	{
		return -1;
	}
}


int GetRgbValue(BYTE *bytes,int w,int h,int strides,double positionX,double positionY,RGBQUAD &rgb)
{
	if (positionX>=0&&positionX<w&&
		positionY>=0&&positionY<h)
	{
		int x1 = int(positionX);
		int y1 = int(positionY);
		int x2 = x1+1;
		int y2 = y1+1;
		if (x2>=w||y2>=h)
		{
			return int(bytes[y1*strides+x1]);
		}
		double weight[4] = {0.0};
		weight[0] = (1-(positionX-x1))*(1-(positionY-y1));
		weight[1] = (1-(positionX-x1))*(positionY-y1);
		weight[2] = (positionX-x1)*(1-(positionY-y1));
		weight[3] = (positionX-x1)*(positionY-y1);
		rgb.rgbBlue = int(weight[0]*bytes[y1*strides+3*x1]+weight[1]*bytes[y2*strides+3*x1]+weight[2]*bytes[y1*strides+3*x2]+weight[3]*bytes[y2*strides+3*x2]);
		rgb.rgbGreen = int(weight[0]*bytes[y1*strides+3*x1+1]+weight[1]*bytes[y2*strides+3*x1+1]+weight[2]*bytes[y1*strides+3*x2+1]+weight[3]*bytes[y2*strides+3*x2+1]);
		rgb.rgbRed = int(weight[0]*bytes[y1*strides+3*x1+2]+weight[1]*bytes[y2*strides+3*x1+2]+weight[2]*bytes[y1*strides+3*x2+2]+weight[3]*bytes[y2*strides+3*x2+2]);
		return 1;
	}
	else
	{
		return -1;
	}
}


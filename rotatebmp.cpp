#include<iostream>
#include<fstream>

#pragma pack(1)//取消对齐

using namespace std;

typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef long LONG;

class tagBITMAPFILEHEADER 
{
	public:
		WORD bfType; // 位图文件的类型，必须为“BM” 
		DWORD bfSize; // 位图文件的大小，以字节为单位 
		WORD bfReserved1; // 位图文件保留字，必须为0 
		WORD bfReserved2; // 位图文件保留字，必须为0 
		DWORD bfOffBits; // 位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位 
};
class tagBITMAPINFOHEADER 
{
	public:
		DWORD biSize; // 本结构所占用字节数 
		LONG biWidth; // 位图的宽度，以像素为单位 
		LONG biHeight; // 位图的高度，以像素为单位 
		WORD biPlanes; // 目标设备的平面数不清，必须为1 
		WORD biBitCount;// 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一 
		DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 
		DWORD biSizeImage; // 位图的大小，以字节为单位
		LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数 
		LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数 
		DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数 
		DWORD biClrImportant;// 位图显示过程中重要的颜色数 
};
class tagRGBQUAD 
{
	public:
		BYTE rgbBlue;// 蓝色的亮度(值范围为0-255) 
		BYTE rgbGreen; // 绿色的亮度(值范围为0-255) 
		BYTE rgbRed; // 红色的亮度(值范围为0-255) 
		//BYTE rgbReserved;// 24位真彩色无灰度属性
		tagRGBQUAD() :rgbBlue(0), rgbGreen(0), rgbRed(0) {}
};

int getDiff(tagBITMAPINFOHEADER & Info)//获取每行需要补0的个数
{
	int DataSizePerLine = (Info.biWidth * Info.biBitCount + 31) / 8;// 一个扫描行所占的字节数
	DataSizePerLine -= DataSizePerLine % 4;
	return DataSizePerLine - Info.biWidth * Info.biBitCount / 8;
}
void BMPTrans(ifstream & src, ofstream & dest, tagBITMAPFILEHEADER & head, tagBITMAPINFOHEADER & info)
{
	tagBITMAPFILEHEADER newhead = head;
	tagBITMAPINFOHEADER newinfo = info;
	newinfo.biWidth = info.biHeight;
	newinfo.biHeight = info.biWidth;//长宽互换
	int diff = getDiff(info);//获取每行需要补0的个数
	tagRGBQUAD* pic = new tagRGBQUAD[info.biHeight * info.biWidth];//原图
	for (int i = 0; i < info.biHeight; i++)
	{
		src.read((char*)(pic + i * info.biWidth), info.biWidth * sizeof(tagRGBQUAD));//逐行读入像素点
		src.seekg(diff, ios::cur);//定位读指针到当前位置（行首）
	}
	diff = getDiff(newinfo);//新图每行需要补0的个数
	char * zero = new char[diff + 1];//补0串
	memset(zero, 0, diff + 1);
	newinfo.biSizeImage = newinfo.biHeight * (newinfo.biWidth + diff);
	newhead.bfSize = newinfo.biSizeImage + sizeof(newhead) + sizeof(newinfo);
	tagRGBQUAD * newpic = new tagRGBQUAD[newinfo.biWidth * newinfo.biHeight];//新图
	for (int i = 0; i < newinfo.biHeight; i++)//新图像素与原图的对应
	{
		for (int j = 0; j < newinfo.biWidth ; j++)
		{
			*(newpic + (newinfo.biHeight - 1 - i) * newinfo.biWidth + j) = *(pic + j * info.biWidth + i);
		}
	}
	dest.write((char *) & newhead, sizeof(tagBITMAPFILEHEADER));//写入
	dest.write((char *) & newinfo, sizeof(tagBITMAPINFOHEADER));
	for (int i = 0; i < newinfo.biHeight; i++)
	{
		dest.write((char*)(newpic + newinfo.biWidth * i), newinfo.biWidth * sizeof(tagRGBQUAD));
		dest.write((char*)zero, diff);
	}
	return;
}

int main(int argc, char * argv[])
{
	char * src_name = argv[1];
	char * dest_name = argv[2];
	ifstream Src(src_name, ios::in | ios::binary);
	if (!Src)
	{
		cout << "Wrong File" << endl;
		return 0;
	}
	ofstream Dest(dest_name, ios::out | ios::binary);
	tagBITMAPFILEHEADER Head;
	tagBITMAPINFOHEADER Info;
	Src.read((char *) & Head, sizeof(tagBITMAPFILEHEADER));
	Src.read((char *) & Info, sizeof(tagBITMAPINFOHEADER));
	BMPTrans(Src, Dest, Head, Info);
	return 0;
}
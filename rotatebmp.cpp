#include<iostream>
#include<fstream>

#pragma pack(1)//ȡ������

using namespace std;

typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef long LONG;

class tagBITMAPFILEHEADER 
{
	public:
		WORD bfType; // λͼ�ļ������ͣ�����Ϊ��BM�� 
		DWORD bfSize; // λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ 
		WORD bfReserved1; // λͼ�ļ������֣�����Ϊ0 
		WORD bfReserved2; // λͼ�ļ������֣�����Ϊ0 
		DWORD bfOffBits; // λͼ���ݵ���ʼλ�ã��������λͼ�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ 
};
class tagBITMAPINFOHEADER 
{
	public:
		DWORD biSize; // ���ṹ��ռ���ֽ��� 
		LONG biWidth; // λͼ�Ŀ�ȣ�������Ϊ��λ 
		LONG biHeight; // λͼ�ĸ߶ȣ�������Ϊ��λ 
		WORD biPlanes; // Ŀ���豸��ƽ�������壬����Ϊ1 
		WORD biBitCount;// ÿ�����������λ����������1(˫ɫ), 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ 
		DWORD biCompression; // λͼѹ�����ͣ������� 0(��ѹ��),1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ 
		DWORD biSizeImage; // λͼ�Ĵ�С�����ֽ�Ϊ��λ
		LONG biXPelsPerMeter; // λͼˮƽ�ֱ��ʣ�ÿ�������� 
		LONG biYPelsPerMeter; // λͼ��ֱ�ֱ��ʣ�ÿ�������� 
		DWORD biClrUsed;// λͼʵ��ʹ�õ���ɫ���е���ɫ�� 
		DWORD biClrImportant;// λͼ��ʾ��������Ҫ����ɫ�� 
};
class tagRGBQUAD 
{
	public:
		BYTE rgbBlue;// ��ɫ������(ֵ��ΧΪ0-255) 
		BYTE rgbGreen; // ��ɫ������(ֵ��ΧΪ0-255) 
		BYTE rgbRed; // ��ɫ������(ֵ��ΧΪ0-255) 
		//BYTE rgbReserved;// 24λ���ɫ�޻Ҷ�����
		tagRGBQUAD() :rgbBlue(0), rgbGreen(0), rgbRed(0) {}
};

int getDiff(tagBITMAPINFOHEADER & Info)//��ȡÿ����Ҫ��0�ĸ���
{
	int DataSizePerLine = (Info.biWidth * Info.biBitCount + 31) / 8;// һ��ɨ������ռ���ֽ���
	DataSizePerLine -= DataSizePerLine % 4;
	return DataSizePerLine - Info.biWidth * Info.biBitCount / 8;
}
void BMPTrans(ifstream & src, ofstream & dest, tagBITMAPFILEHEADER & head, tagBITMAPINFOHEADER & info)
{
	tagBITMAPFILEHEADER newhead = head;
	tagBITMAPINFOHEADER newinfo = info;
	newinfo.biWidth = info.biHeight;
	newinfo.biHeight = info.biWidth;//������
	int diff = getDiff(info);//��ȡÿ����Ҫ��0�ĸ���
	tagRGBQUAD* pic = new tagRGBQUAD[info.biHeight * info.biWidth];//ԭͼ
	for (int i = 0; i < info.biHeight; i++)
	{
		src.read((char*)(pic + i * info.biWidth), info.biWidth * sizeof(tagRGBQUAD));//���ж������ص�
		src.seekg(diff, ios::cur);//��λ��ָ�뵽��ǰλ�ã����ף�
	}
	diff = getDiff(newinfo);//��ͼÿ����Ҫ��0�ĸ���
	char * zero = new char[diff + 1];//��0��
	memset(zero, 0, diff + 1);
	newinfo.biSizeImage = newinfo.biHeight * (newinfo.biWidth + diff);
	newhead.bfSize = newinfo.biSizeImage + sizeof(newhead) + sizeof(newinfo);
	tagRGBQUAD * newpic = new tagRGBQUAD[newinfo.biWidth * newinfo.biHeight];//��ͼ
	for (int i = 0; i < newinfo.biHeight; i++)//��ͼ������ԭͼ�Ķ�Ӧ
	{
		for (int j = 0; j < newinfo.biWidth ; j++)
		{
			*(newpic + (newinfo.biHeight - 1 - i) * newinfo.biWidth + j) = *(pic + j * info.biWidth + i);
		}
	}
	dest.write((char *) & newhead, sizeof(tagBITMAPFILEHEADER));//д��
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
#pragma once
#include <string>
#include "memory.h"
#include "filestream.h"
#include "image.h"

// ����� ���������� ��� ������ �����
// ������������� � �������������� �������
class MediaFileException {};

// ����� ����� BITMAP
class BMPFile {
public:

	// ��������� ��� ���������
	static const unsigned BITMAP_SIGNATURE = 0x4d42;

	// ������. � ������ ������ �� ������������, �� ���
	// ���������� ���������� �������������� �������� 
	// ���������� ��������� ������
	enum BmpCompression {
		NoCompression = 0x00
	};

	//��������� ������ ����� ������� � ���� ����
#pragma pack (push, 1)
	struct BmpFileHeader {
		uint16_t signature;
		uint32_t fileSize;
		uint16_t reserved1;
		uint16_t reserved2;
		uint32_t fileOffset;
	};

	struct BmpInfoHeader {
		uint32_t headerSize;
		uint32_t width;
		uint32_t height;
		uint16_t planes;
		uint16_t bitCount;
		uint32_t compression;
		uint32_t sizeImage;
		uint32_t pixelsPerMeterX;
		uint32_t pixelsPerMeterY;
		uint32_t colorsUsed;
		uint32_t importantColors;
	};

	struct BmpRequiredHeader {
		BmpFileHeader FileHeader;
		BmpInfoHeader InfoHeader;
	};

#pragma pack(pop)

	// �����������
	// fileName - ��� �����
	BMPFile(std::string fileName);

	// ����������
	~BMPFile();

	// ��������� �������� �� �����. ������ �������������
	// ������ ����������� �� ����, ��� ����� ����� ��������
	// ����� GetImage
	ResultCode ReadImageFromFile();

	// �������� �������� � ����
	// img - ����� ��� ������
	ResultCode SaveImageToFile(Frame* img);

	// �������� ������� ��������. ����� ����� 
	// �������� MediaFileException, ���� ��������������
	// ��� �������� �� ��������� ��� ���������
	Frame GetImage();

private:

	// ��������� ���������
	ResultCode ReadHeader();

	// �������� ��������� � ����
	ResultCode WriteHeader();

	// ����� �����
	FileStream*  m_pFileStream;

	// ��������� �����
	BmpRequiredHeader    m_header;

	// �����
	Frame*			m_pImageObject;

};

// ����� YCbCr �����
class YUVFile {
public:
	
	// ��� �������� ������ � �������� Y'UV. 
	// � ������ ������ �� ������������, �� ���
	// ���������� ���������� �������������� �������� 
	// ���������� ��������� ������
	enum YUVType {
		YUV420
	};

	// �����������
	// � Y'UV ����� ��� ���������, ������� ���������� ������� ����� ������ �����
	// fileName - ��� �����
	// size - ������ �����
	YUVFile(std::string fileName, Size2 frameSize);
	
	// ����������
	~YUVFile();

	// �������� ����� �� �����.
	// img - ��������� �� �����
	// ����������:
	// S_OK -����� ��� �������
	// S_FALSE -���� ������� � �����
	// Failed(x) -������
	ResultCode ReadFrame(Frame* img);

	// �������� ����� � ����.
	// img - ��������� �� �����
	// ����������:
	// S_OK -����� ��� �������
	// Failed(x) -������
	ResultCode SaveFrame(Frame* img);

private:

	// ����� �����
	FileStream*     m_pFileStream;

	// ������ ������
	Size2			m_frameSize;

};
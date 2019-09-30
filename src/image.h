#pragma once
#include <stdint.h>
#include "rescode.h"
#include <string>

// ���������� ������
class Frame;

// �������� ��� ������� ��� �����������
typedef struct Vector3 {
	uint8_t x;
	uint8_t y;
	uint8_t z;

	Vector3() { x = 0; y = 0; z = 0; }

	Vector3& operator=(const Vector3& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		return *this;
	};
} Color3;

// ��������������� ���� ��� ���������������
typedef struct Vector4 {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t a;

	Vector4() { x = 0; y = 0; z = 0; a = 0; }

	Vector4& operator=(const Vector4& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		a = Vec.a;
		return *this;
	};

	Vector4& operator=(const Vector3& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		a = 0;
		return *this;
	};
} Color4;


// ���� ������� ����� ���� �� �������� Size2...
// �� ��� ��� ��� ���� ���������������, �� 
// ��������� ��� �����. ���� ����� ��������� ������,
// �� ��� ����������� ����� �������
struct CommonFrame {
	Color3* pixels;
	size_t  size;
};


// ��������� ��� ������� ������ � ��������� �����������
struct Size2 {
	size_t width;
	size_t height;

	inline unsigned int GetElementsCount() { return width * height; }
};

// ����� �����
class Frame {
public:

	// ��� ����� .
	// TODO: ���� �� ������������, ���������� �������� ��������
	enum FrameType {
		RGB,
		YUV
	};

	enum OverlayPosition {
		LeftUp,
		MiddleUp,
		RightUp,
		LeftMiddle,
		Middle,
		RightMiddle,
		LeftDown,
		MiddleDown,
		RightDown
	};

	// �����������
	// frameSize - ������ �����
	Frame(Size2 frameSize);

	// �����������
	// frameSize - ������ �����
	// framePixels - ������ �������� �����
	Frame(Size2 frameSize, Vector3* framePixels);

	// ����������
	virtual ~Frame();

	// �������� ������ �����������
	inline Size2 GetSize() { return m_ImageSize; };

	// �������� ���������� ��������
	inline unsigned int GetPixelsCount() { return m_ImageSize.GetElementsCount(); };

	// �������� ��������� �� ������ �������� �����
	inline Vector3* GetPixels() { return m_pPixels; };

	// �������� ���� ���� �� ������
	// frame - ������������� ����
	// pos - ������� ���������
	ResultCode OverlayFrame(Frame &frame, Frame::OverlayPosition pos);

private:
	// ������ �����
	Size2 m_ImageSize;

	// ������� �����
	Vector3* m_pPixels;
};

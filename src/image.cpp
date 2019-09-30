#include <functional>

#include "image.h"

// �����������
Frame::Frame(Size2 frameSize)
{
	m_ImageSize.height = frameSize.height;
	m_ImageSize.width = frameSize.width;
	m_pPixels = new Vector3[m_ImageSize.GetElementsCount()];
}

// �����������
Frame::Frame(Size2 frameSize, Vector3 * framePixels)
{
	m_ImageSize.height = frameSize.height;
	m_ImageSize.width = frameSize.width;
	m_pPixels = new Vector3[m_ImageSize.GetElementsCount()];

	// �������� ������� ( ����������� Vector3 ��������� ����� ������)
	for (uint32_t i = 0; i < m_ImageSize.GetElementsCount(); i++)
		m_pPixels[i] = framePixels[i];
}

// ����������
Frame::~Frame()
{
	delete[] m_pPixels;
}

ResultCode Frame::OverlayFrame(Frame &frame, Frame::OverlayPosition pos)
{
	uint32_t width = m_ImageSize.width;
	uint32_t height = m_ImageSize.height;

	uint32_t overlayWidth = frame.GetSize().width;
	uint32_t overlayHeight = frame.GetSize().height;

	// ���� ������� ������ ������, �� ���������� �������������� ����
	if ((overlayWidth > width) ||
		overlayHeight > height)
		return ERR_INVALIDPARAMS;

	uint32_t hLB, wLB;

	switch (pos)
	{
	case Frame::LeftUp:
		hLB = 0;
		wLB = 0;
		break;
	case Frame::MiddleUp:
		hLB = 0;
		wLB = width / 2 - overlayWidth / 2;
		break;
	case Frame::RightUp:
		hLB = 0;
		wLB = width - overlayWidth;
		break;
	case Frame::LeftMiddle:
		hLB = height / 2 - overlayHeight / 2;
		wLB = 0;
		break;
	case Frame::Middle:
		hLB = height / 2 - overlayHeight / 2;
		wLB = width / 2 - overlayWidth / 2;
		break;
	case Frame::RightMiddle:
		hLB = height / 2 - overlayHeight / 2;
		wLB = width - overlayWidth;
		break;
	case Frame::LeftDown:
		hLB = height - overlayHeight;
		wLB = 0;
		break;
	case Frame::MiddleDown:
		hLB = height - overlayHeight;
		wLB =width / 2 - overlayWidth / 2;
		break;
	case Frame::RightDown:
		hLB = height - overlayHeight;
		wLB = width - overlayWidth;
		break;
	default:
		return ERR_INVALIDPARAMS;
		break;
	}

	// ���� �� �����. �������� � �� ����� �����
	//// ������� ������-��������� ��� ������� ������ �������
	/*std::function<bool(int, int, int)> inBounds =
	[&](int check, int lower, int upper) ->
	bool { return (check >= lower && check <= upper); };*/

	// ����������� �� ���� �������� ������������ � ��� �����
	// �� ����� ������������ �������� �� ������, ����� �������
	// �� ����� ��������
	for (unsigned int i = 0; i < overlayHeight - 1; i++) {
		for (unsigned int j = 0; j < overlayWidth - 1; j++) {
			m_pPixels[(i + hLB) * width + j + wLB].x =
				frame.m_pPixels[i * overlayWidth + j].x;
			m_pPixels[(i + hLB) * width + j + wLB].y =
				frame.m_pPixels[i * overlayWidth + j].y;
			m_pPixels[(i + hLB) * width + j + wLB].z =
				frame.m_pPixels[i * overlayWidth + j].z;
		}
	}

	// ���������� ������� ���������
	return S_OK;
}

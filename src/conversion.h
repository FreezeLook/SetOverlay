#pragma once
#include <condition_variable>
#include <vector>
#include <mutex>
#include "image.h"

// ������ ��� ������� ���������
class ConversionThreadData
{
public:

	// ������� ������� � ������

	// ���������� ��������
	inline void SetSource(CommonFrame source) { m_source = source; }

	// �������� ��������
	inline CommonFrame& GetSource() { return m_source; }

	// ���������� ���������
	inline void SetDestination(const CommonFrame& destination) { m_destination = destination; }

	// �������� ���������
	inline CommonFrame& GetDestination() { return m_destination; }

private:
	// ����� ����� ��� ���������������
	CommonFrame m_source;

	// �����, ���� ����� ����� ��������� ���������������� ������
	CommonFrame m_destination;
};


class Converter
{
public:
	// �����������
	// threadsCount - ���������� �������������� �������.
	// ���� threadsCount == 0, �� ���������� ������� ��� ����� 1.
	Converter(const unsigned int threadsCount);

	// ����������
	virtual ~Converter();

	// �������������
	void Initialize(Frame& sourceFrame);

	// ���������������
	ResultCode Convert(Frame* outputFrame);
	

private:

	// ���������� ��� ���������� ���� �������

	// �������� ��� ����\������ �������
	const int cLockThreads = -1;
	const int cUnlockThreads = -2;

	// ���������� ��� ������������� �������.
	// ��� ������ ������ ������� ������������ ��������, �������� ����
	int m_syncVariableValue = 0;

	// ���� ���������� ������ �������
	bool m_bJobDone = false;

	// ������ �����
	Size2	m_pictureSize;

	// �������������� ����
	CommonFrame m_source;

	// ��������� ���������������
	CommonFrame m_destination;

	// ��� �������
	std::vector<ConversionThreadData> m_threadPool;

	// ���������� �������, ����������� ������ � ������ ������
	unsigned int m_runningThreads = 0;

	// ���������� �������, ����������� ������
	unsigned int m_completedThreads = 0;

	// ������ ��� ������� � �������������� ������
	std::mutex m_mutex;

	// ���������� ������������� �������
	std::condition_variable m_sync;

	// �������, ������������ � ������ ��� ���������������
	void ConvertionTask(size_t threadIndex);

	// ������� �������������� RGB to YCbCr. �������� ������� �������
	void ConvertRgbToYuv(CommonFrame& source, CommonFrame& destination);

};

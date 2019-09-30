#pragma once
#include <fstream>
#include <string>
#include "rescode.h"


// �����-������� ��� ������ �����-������ ������.
// ����� ������������� ����� ������ � �������, �� � 
// ������������� ��������� - ��� ��������� ���� ������
// ������� � ����� ������������. ��� ����, ����� ������� 
// � ����������� ����� ����� ������������ Seek
class FileStream {
public:

	// �����������
	// fileName - ��� �����
	// isBinary - ���� ��������� �����
	FileStream(std::string fileName, bool isBinary = true);

	// ����������
	virtual ~FileStream();

	// ������ �� ������. ���� ���� ������ �� ReadMode,
	// �� ���������� ����������� �����, ��������� ��� 
	// ������� � �����
	// pObject - ��������� �� ����������, ���� ����� ����� ���������� (!= NULL)
	// objectSize - ���������� ������ ��� �����������
	ResultCode Read(char* pObject, size_t objectSize);

	// ������ � �����. ���� ���� ������ �� WriteMode,
	// �� ���������� ����������� �����, ��������� ��� 
	// ������� � �����
	// pObject - ��������� �� ����������, ������� ����� �������� (!= NULL)
	// objectSize - ���������� ������ ��� ������
	ResultCode Write(char* pObject, size_t objectSize);

	// ������� �����
	void Close();

	// ������������� �� ������� � �����
	// position - ������� � �����
	void Seek(size_t position);

	// ��������. ���������� true ���� ����� ������
	inline bool IsOpen() { return m_pStream->is_open(); };

	inline std::string GetFilename() { return m_strFileName; };

private:

	// ����� ������.
	enum StreamMode {
		NoMode,		// ����� �� ������ � �� �����
		ReadMode,	// ����� ������
		WriteMode   // ����� �����
	};

	// ����� ������/������
	std::fstream* m_pStream;

	// ��� �����
	std::string m_strFileName;

	// ����. ���� true, ������ ���� ��������
	bool		m_bIsBinaryStream;

	// ����� ������
	StreamMode  m_streamMode;

	// ������� ����� � ������������ �������
	ResultCode Open(StreamMode mode);

};
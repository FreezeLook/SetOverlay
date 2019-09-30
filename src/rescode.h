#pragma once

enum  ResultCode {
	///////////////////////////
	// ���� �������� - �����
	///////////////////////////

	S_OK,					// ����������� ��� ��������� ���������� ��������
	S_FALSE,				// �������� ����������� �� ������, �� ��� �� ������




	///////////////////////////
	// ���� �������� - ������
	///////////////////////////
	ERR_FAILURE,
	ERR_INVALIDPARAMS, // �������� ������������ ���������
	ERR_OUTOFMEMORY,
	ERR_NOTIMPLEMENTED,

	// ���� �������� ��� �����
	ERR_FILENOTFOUND,		// ���� �� ������
	ERR_READFILE,			// ������ ��� ������ �����
	ERR_WRITEFILE,			// ������ ��� ������ � ����
	ERR_FILECORRUPTED,


};

// �������� �� ���������� ��������
inline bool Failed(ResultCode& code) {
	return (code >= ERR_FAILURE);
}

// ������ �������� ������
#define ReturnIfFailed(rc)								\
	if (Failed(rc)) return rc;


#define ReturnIfNotOK(rc)								\
	if (rc != S_OK) return rc;
#include "filestream.h"

FileStream::FileStream(std::string fileName, bool isBinary)
	: m_strFileName(fileName),
	m_bIsBinaryStream(isBinary),
	m_streamMode(NoMode)
{
	m_pStream = new std::fstream();
}

FileStream::~FileStream()
{
	delete m_pStream;
}

ResultCode FileStream::Read(char * pObject, size_t objectSize)
{
	if (pObject == nullptr)
		return ERR_INVALIDPARAMS;
	
	ResultCode rs = S_OK;

	// ��������� �����
	rs = Open(ReadMode);
	ReturnIfFailed(rs);

	// ����������
	m_pStream->read(pObject, objectSize);
	
	int error;
	if ((error = m_pStream->rdstate()) != 0) {
		if (error == std::ios_base::eofbit) // ���� ����� �� eof, ����� �� ������
			return S_FALSE;
		return ERR_READFILE; // �����...
	}

	return S_OK;

}

ResultCode FileStream::Write(char * pObject, size_t objectSize)
{
	if (pObject == nullptr)
		return ERR_INVALIDPARAMS;

	ResultCode rs = S_OK;

	// ��������� �����
	rs = Open(WriteMode);
	ReturnIfFailed(rs);

	// �����
	m_pStream->write(pObject, objectSize);

	// �� �������� - ����������� ������
	if (m_pStream->rdstate())
		return ERR_WRITEFILE;

	return S_OK;
}

void FileStream::Close()
{
	m_streamMode = NoMode;
	m_pStream->close();
}

void FileStream::Seek(size_t position)
{
	m_pStream->seekg(position);
}

ResultCode FileStream::Open(StreamMode streamMode)
{
	// ���� �� ����� ������������� �����, �� ����� ������
	if (m_streamMode == streamMode)
		return S_OK;

	// ������ ����������� ��������� ��� fstream->open
	std::ios_base::openmode mode = streamMode == ReadMode ? std::ios::in : std::ios::out | std::ios::ate;
	if (m_bIsBinaryStream)
		mode |= std::ios::binary;

	if (!IsOpen()) {
		// ����� �� ��� ������. ������ ������� ���
		m_pStream->open(m_strFileName, mode);
		if (!IsOpen())
			return ERR_FILENOTFOUND;

		m_streamMode = streamMode;
		return S_OK;
	}

	// ���� �� �����, ������ ���������� ������� ����� ������
	// ��������� ����� � ���������
	m_pStream->close();
	m_pStream->open(m_strFileName, mode);

	if (!IsOpen())
		return ERR_FAILURE;

	m_streamMode = streamMode;
	return S_OK;
}

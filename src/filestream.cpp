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

	// Открываем поток
	rs = Open(ReadMode);
	ReturnIfFailed(rs);

	// Вычитываем
	m_pStream->read(pObject, objectSize);
	
	int error;
	if ((error = m_pStream->rdstate()) != 0) {
		if (error == std::ios_base::eofbit) // Если дошли до eof, тогда не ошибка
			return S_FALSE;
		return ERR_READFILE; // иначе...
	}

	return S_OK;

}

ResultCode FileStream::Write(char * pObject, size_t objectSize)
{
	if (pObject == nullptr)
		return ERR_INVALIDPARAMS;

	ResultCode rs = S_OK;

	// Открываем поток
	rs = Open(WriteMode);
	ReturnIfFailed(rs);

	// Пишем
	m_pStream->write(pObject, objectSize);

	// Не записали - обязательно ошибка
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
	// Если мы имеем запрашиваемый режим, то поток открыт
	if (m_streamMode == streamMode)
		return S_OK;

	// Найдем необходимые параметры для fstream->open
	std::ios_base::openmode mode = streamMode == ReadMode ? std::ios::in : std::ios::out | std::ios::ate;
	if (m_bIsBinaryStream)
		mode |= std::ios::binary;

	if (!IsOpen()) {
		// Поток не был открыт. Просто откроем его
		m_pStream->open(m_strFileName, mode);
		if (!IsOpen())
			return ERR_FILENOTFOUND;

		m_streamMode = streamMode;
		return S_OK;
	}

	// Если мы здесь, значит необходимо сменить режим записи
	// Закрываем поток и открываем
	m_pStream->close();
	m_pStream->open(m_strFileName, mode);

	if (!IsOpen())
		return ERR_FAILURE;

	m_streamMode = streamMode;
	return S_OK;
}

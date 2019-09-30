#pragma once
#include <fstream>
#include <string>
#include "rescode.h"


// Класс-обертка для потока ввода-вывода файлов.
// Может переключаться между вводом и выводом, но с 
// определенными условиями - при изменении типа потока
// позиция в файле сбрасывается. Для того, чтобы попасть 
// в необхлдимое место нежно использовать Seek
class FileStream {
public:

	// Конструктор
	// fileName - имя файла
	// isBinary - флаг бинарного файла
	FileStream(std::string fileName, bool isBinary = true);

	// Деструктор
	virtual ~FileStream();

	// Чтение из потока. Если флаг режима не ReadMode,
	// то фактически пересоздает поток, сбрасывая его 
	// позицию в файле
	// pObject - указатель на переменную, куда поток будет вычитывать (!= NULL)
	// objectSize - количество байтов для вычитывания
	ResultCode Read(char* pObject, size_t objectSize);

	// Запись в поток. Если флаг режима не WriteMode,
	// то фактически пересоздает поток, сбрасывая его 
	// позицию в файле
	// pObject - указатель на переменную, которая будет записана (!= NULL)
	// objectSize - количество байтов для записи
	ResultCode Write(char* pObject, size_t objectSize);

	// Закрыть поток
	void Close();

	// Переместиться на позицию в файле
	// position - позиция в файле
	void Seek(size_t position);

	// Проверка. Возвращает true если поток открыт
	inline bool IsOpen() { return m_pStream->is_open(); };

	inline std::string GetFilename() { return m_strFileName; };

private:

	// Режим потока.
	enum StreamMode {
		NoMode,		// Поток не читает и не пишет
		ReadMode,	// Поток читает
		WriteMode   // Поток пишет
	};

	// Поток чтения/записи
	std::fstream* m_pStream;

	// Имя файла
	std::string m_strFileName;

	// Флаг. Если true, значит файл бинарный
	bool		m_bIsBinaryStream;

	// Режим потока
	StreamMode  m_streamMode;

	// Открыть поток с определенным режимом
	ResultCode Open(StreamMode mode);

};
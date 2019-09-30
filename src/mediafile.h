#pragma once
#include <string>
#include "memory.h"
#include "filestream.h"
#include "image.h"

// Класс исключения для файлов медиа
// Выбрасывается в исключительных случаях
class MediaFileException {};

// Класс файла BITMAP
class BMPFile {
public:

	// Сигнатура для сравнения
	static const unsigned BITMAP_SIGNATURE = 0x4d42;

	// Сжатие. В данный момент не используется, но при
	// увеличении количества обрабатываемых форматов 
	// необходимо расширять список
	enum BmpCompression {
		NoCompression = 0x00
	};

	//Структура должна иметь отступы в один байт
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

	// Конструктор
	// fileName - имя файла
	BMPFile(std::string fileName);

	// Деструктор
	~BMPFile();

	// Прочитать картинку из файла. Аналог инициализации
	// Делать ОБЯЗАТЕЛЬНО до того, как будем брать картинку
	// через GetImage
	ResultCode ReadImageFromFile();

	// Записать картинку в файл
	// img - фрейм для записи
	ResultCode SaveImageToFile(Frame* img);

	// Получить пиксели картинки. Здесь может 
	// вылететь MediaFileException, если предварительно
	// эту картинку не сохранить или загрузить
	Frame GetImage();

private:

	// Прочитать заголовок
	ResultCode ReadHeader();

	// Записать заголовок в файл
	ResultCode WriteHeader();

	// Поток файла
	FileStream*  m_pFileStream;

	// Заголовок файла
	BmpRequiredHeader    m_header;

	// Фрейм
	Frame*			m_pImageObject;

};

// Класс YCbCr файла
class YUVFile {
public:
	
	// Тип хранения данных о пикселях Y'UV. 
	// В данный момент не используется, но при
	// увеличении количества обрабатываемых форматов 
	// необходимо расширять список
	enum YUVType {
		YUV420
	};

	// Конструктор
	// У Y'UV файла нет заголовка, поэтому необходимо заранее знать размер кадра
	// fileName - имя файла
	// size - размер кадра
	YUVFile(std::string fileName, Size2 frameSize);
	
	// Деструктор
	~YUVFile();

	// Вычитать фрейм из файла.
	// img - указатель на фрейм
	// Возвращает:
	// S_OK -фрейм был вычитан
	// S_FALSE -файл подошел к концу
	// Failed(x) -ошибка
	ResultCode ReadFrame(Frame* img);

	// Записать фрейм в файл.
	// img - указатель на фрейм
	// Возвращает:
	// S_OK -фрейм был записан
	// Failed(x) -ошибка
	ResultCode SaveFrame(Frame* img);

private:

	// Поток файла
	FileStream*     m_pFileStream;

	// Размер фрейма
	Size2			m_frameSize;

};
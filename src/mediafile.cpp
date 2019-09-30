#include "mediafile.h"

BMPFile::BMPFile(std::string fileName)
{
	m_pFileStream = new FileStream(fileName);
}

BMPFile::~BMPFile()
{
	delete m_pFileStream;
}

ResultCode BMPFile::ReadImageFromFile()
{
	ResultCode rs = S_OK;
	
	// Вычитываем заголовок
	rs = ReadHeader();
	ReturnIfNotOK(rs);

	// Сразу вычисляем размеры
	size_t width = m_header.InfoHeader.width;
	size_t height = m_header.InfoHeader.height;

	// Оу, важная деталь. Файл BMP хранится построчно,
	// при этом необходимо, чтобы каждая строка начиналась с
	// адреса, кратного четырем (очевидно почему). Поэтому
	// вычитывать файл мы тоже должны построчно, иначе наткнемся
	// на сдвиг rgb, что попортит нам выходной результат
	size_t hStep = (width*3 + 3) & (~3);

	// Временная защита от файлов с обратной последовательностью записи
	// TODO: дописать
	if (height < 0 )
		return ERR_FILECORRUPTED;

	// Создаем фрейм
	m_pImageObject =
		new Frame({ width , height });

	// Переносимся на позицию после заголовка
	m_pFileStream->Seek(m_header.FileHeader.fileOffset);

	// Вычитываем все скопом
	uint8_t* rgb = new uint8_t[hStep * height];
	rs = m_pFileStream->Read((char*)rgb, hStep * height * sizeof(uint8_t));
	ReturnIfNotOK(rs);

	// Внимание! Вычитываем файл строками(шагами) в обратном порядке
	// Дело в том, что пиксели в файлах BMP с положительной высотой в заголовке
	// находятся в обратном порядке относительно вертикали.
	int index = 0;
	for (int h = height - 1; h >= 0; h--) {
		uint8_t* currentPage = rgb + (hStep * h);

		// Мало того, сами пиксели расположены не в порядке rgb, а gbr
		for (int w = 0; w < width; w++) {
			m_pImageObject->GetPixels()[index].z = currentPage[w * 3]; // b
			m_pImageObject->GetPixels()[index].y = currentPage[w * 3 + 1]; // g 
			m_pImageObject->GetPixels()[index].x = currentPage[w * 3 + 2]; // r
			index++;
		}
	}

	// Закрываем поток, так как больше мы из него ничего вычитывать не должны
	m_pFileStream->Close();

	return S_OK;
}

// TODO: Протестить, не использовалась
ResultCode BMPFile::SaveImageToFile(Frame* img)
{
	ResultCode rs = S_OK;

	// Заменяем кадр
	m_pImageObject = img;

	WriteHeader();

	m_pFileStream->Seek(m_header.FileHeader.fileOffset);

	// Сразу вычисляем размеры
	size_t width = m_header.InfoHeader.width;
	size_t height = m_header.InfoHeader.height;

	// Оу, важная деталь. Файл BMP хранится построчно,
	// при этом необходимо, чтобы каждая строка начиналась с
	// адреса, кратного четырем (очевидно почему). Поэтому
	// писать файл мы тоже должны построчно, иначе наткнемся
	// на сдвиг rgb, что попортит нам выходной результат
	size_t hStep = (width * 3 + 3) & (~3);


	uint8_t* rgb = new uint8_t[hStep * height];

	// Внимание! Пишем файл строками(шагами) в обратном порядке
	// Дело в том, что пиксели в файлах BMP с положительной высотой в заголовке
	// находятся в обратном порядке относительно вертикали.
	int index = 0;
	for (int h = height - 1; h >= 0; h--) {
		uint8_t* currentPage = rgb + (hStep * h);

		// Мало того, сами пиксели расположены не в порядке rgb, а gbr
		for (int w = 0; w < width; w++) {
			currentPage[w * 3] = m_pImageObject->GetPixels()[index].z;  // b
			currentPage[w * 3 + 1] = m_pImageObject->GetPixels()[index].y;  // g 
			currentPage[w * 3 + 2] = m_pImageObject->GetPixels()[index].x; // r
			index++;
		}
	}

	rs = m_pFileStream->Write((char*)rgb, hStep * height * sizeof(uint8_t));
	ReturnIfFailed(rs);

	m_pFileStream->Close();

	return S_OK;
}

// Получить изображение
Frame BMPFile::GetImage()
{
	// Если нет изображения - валим все
	if (m_pImageObject == NULL)
		throw MediaFileException();

	// Либо возвращаем
 	return *m_pImageObject;

}

ResultCode BMPFile::ReadHeader()
{
	// Вычитываем заголовок
	ResultCode rs;
	rs = m_pFileStream->Read((char*)&m_header, sizeof(m_header));
	ReturnIfNotOK(rs);

	// И проверяем
	if ((m_header.FileHeader.signature != BITMAP_SIGNATURE) ||
		(m_header.InfoHeader.compression != NoCompression))
		return ERR_FILECORRUPTED;
		
	return S_OK;
}

ResultCode BMPFile::WriteHeader()
{
	// Собираем заголовок BMP24 без альфа канала и палитры
	m_header.FileHeader.fileOffset = sizeof(BmpRequiredHeader);
	m_header.FileHeader.fileSize = sizeof(BmpRequiredHeader) +
		m_pImageObject->GetPixelsCount() * 3;
	m_header.FileHeader.signature = BITMAP_SIGNATURE;

	m_header.InfoHeader.bitCount = 24;
	m_header.InfoHeader.colorsUsed = 0;
	m_header.InfoHeader.compression = NoCompression;
	m_header.InfoHeader.headerSize = 40;
	m_header.InfoHeader.height = m_pImageObject->GetSize().height;
	m_header.InfoHeader.importantColors = 0;
	m_header.InfoHeader.pixelsPerMeterX = 0;
	m_header.InfoHeader.pixelsPerMeterY = 0;
	m_header.InfoHeader.planes = 1;
	m_header.InfoHeader.sizeImage = m_pImageObject->GetPixelsCount() * 3;
	m_header.InfoHeader.width = m_pImageObject->GetSize().width;

	// И пишем его
	return m_pFileStream->Write((char*)&m_header, sizeof(BmpRequiredHeader));
}

YUVFile::YUVFile(std::string fileName,Size2 size)
	: m_frameSize(size)
{
	m_pFileStream = new FileStream(fileName);
}

YUVFile::~YUVFile()
{
	delete m_pFileStream;
}

ResultCode YUVFile::ReadFrame(Frame* pFrameOut)
{
	ResultCode rs = S_OK;

	// Сразу узнаем высоту и ширину
	uint32_t height = m_frameSize.height;
	uint32_t width = m_frameSize.width;

	// И количество байт в кадре
	uint32_t sizeBytes = (width * height + 2 * ((width + 1) / 2) * ((height + 1) / 2));

	// Вычитываем
	uint8_t* yuv = new uint8_t[sizeBytes];
	rs = m_pFileStream->Read((char*)yuv, sizeBytes);
	ReturnIfNotOK(rs);

	// Распределяем по плоскостям
	uint8_t* y = yuv;
	uint8_t* u = yuv + width * height;
	uint8_t* v = yuv + width * height + width * height / 4;

	Frame* pFrame = new Frame(m_frameSize);
	Vector3* pPixels = pFrame->GetPixels();

	for (uint32_t i = 0; i < height; i++) {
		for (uint32_t j = 0; j < width; j++) {
			int pixelIndex = i * width + j;
			pPixels[pixelIndex].x = y[pixelIndex]; // y
			pPixels[pixelIndex].y = u[((i / 2) * (width / 2)) + (j / 2)]; // u
			pPixels[pixelIndex].z = v[((i / 2) * (width / 2)) + (j / 2)]; //v
		}
	}

	*pFrameOut = *pFrame;

	return S_OK;
}

ResultCode YUVFile::SaveFrame(Frame * img)
{
	// Получили нужные величины
	uint32_t height = m_frameSize.height;
	uint32_t width = m_frameSize.width;

	uint32_t size = (width*height + 2 * ((width + 1) / 2)*((height + 1) / 2));

	uint8_t* yuv = new uint8_t[size];

	// Расбросали пиксели на плоскости
	uint8_t* y = yuv;
	uint8_t* u = yuv + width * height;
	uint8_t* v = yuv + width * height + width * height / 4;

 	for (uint32_t i = 0; i < height; i++) {
		for (uint32_t j = 0; j < width; j++) {
			int pixelIndex = i * width + j;
			y[pixelIndex] = img->GetPixels()[pixelIndex].x;
			u[((i / 2) * (width / 2)) + (j / 2)] = img->GetPixels()[pixelIndex].y;  // u
			v[((i / 2) * (width / 2)) + (j / 2)] = img->GetPixels()[pixelIndex].z;  //v
		}
	}
	
	// Записываем
	return m_pFileStream->Write((char*)yuv, size);
}

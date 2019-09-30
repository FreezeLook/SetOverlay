// TestConvert.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

#include  "mediafile.h"
#include "conversion.h"


int main()
{
	// На самом деле можно было бы распарсить параметры из argv,
	// но есть шанс, что пользователю неудобно будет вводить 
	// размеры файла и позицию оверлея. Ну, если надобность появится
	// то прикрутить можно быстро

	std::string YUVFileName;
	Size2 YUVFileSize{ 0,0 };

	std::cout << "Please, enter YUV420 filename (with .yuv): ";
	std::cin >> YUVFileName;
	std::cout << "Okay, lets choose this file resolution:" << std::endl
		<< "1. SMALL (192x256)" << std::endl
		<< "2. QCIF  (176x144)" << std::endl
		<< "3. CIF   (352x288)" << std::endl
		<< "4. WQVGA (416x240)" << std::endl
		<< "5. WVGA  (832x480)" << std::endl
		<< "6. VGA   (640x480)" << std::endl
		<< "7. 720p  (1280x720)" << std::endl
		<< "8. 1080p (1920x1080)" << std::endl
		<< "9. Custom" << std::endl;

	unsigned short userChoice;
	while (std::cin >> userChoice && (!std::cin.good() || userChoice > 9 || userChoice < 1)) {
		std::cout << "Please enter correct number!" << std::endl;
		std::cin.clear();
	}

	switch (userChoice)
	{
	case 1:
		YUVFileSize.width = 192;
		YUVFileSize.height = 256;
		break;
	case 2:
		YUVFileSize.width = 176;
		YUVFileSize.height = 144;
		break;
	case 3:
		YUVFileSize.width = 352;
		YUVFileSize.height = 288;
		break;
	case 4:
		YUVFileSize.width = 416;
		YUVFileSize.height = 240;
		break;
	case 5:
		YUVFileSize.width = 832;
		YUVFileSize.height = 480;
		break;
	case 6:
		YUVFileSize.width = 640;
		YUVFileSize.height = 480;
		break;
	case 7:
		YUVFileSize.width = 1280;
		YUVFileSize.height = 720;
		break;
	case 8:
		YUVFileSize.width = 1920;
		YUVFileSize.height = 1080;
		break;
	case 9:
		std::cout << "Width: ";
		while (std::cin >> YUVFileSize.width && !std::cin.good()) {
			std::cout << "Please enter correct number!" << std::endl;
			std::cin.clear();
		}
		std::cout << "Height: ";
		while (std::cin >> YUVFileSize.height && !std::cin.good()) {
			std::cout << "Please enter correct number!" << std::endl;
			std::cin.clear();
		}
	}

	ResultCode rs = S_OK;

	YUVFile in(YUVFileName, YUVFileSize);
	Frame YUVFrame(YUVFileSize);
	rs = in.ReadFrame(&YUVFrame);
	if (Failed(rs))
	{
		std::cout << "File wasn't opened. Error code: " << rs << std::endl;
		std::cout << "Press Enter to exit." << std::endl;
		std::getchar();
		std::getchar();
		return 0;
	}

	std::string BMPFileName;
	std::cout << "Please, enter BMP24 filename (with .bmp): ";
	std::cin >> BMPFileName;

	BMPFile BMPOverlay(BMPFileName);
	rs = BMPOverlay.ReadImageFromFile();

	if (Failed(rs))
	{
		std::cout << "File wasn't opened. Error code: " << rs << std::endl;
		std::cout << "Press Enter to exit." << std::endl;
		std::getchar();
		std::getchar();
		return 0;
	}


	Frame overlayFrame = BMPOverlay.GetImage();
	
	std::cout << "Now you need to select a position for overlay." << std::endl
		<< " _____________________" << std::endl
		<< "| 1        2        3 |" << std::endl
		<< "|                     |" << std::endl
		<< "| 4        5        6 |" << std::endl
		<< "|                     |" << std::endl
		<< "| 7        8        9 |" << std::endl
		<< "|_____________________|" << std::endl;
	Frame::OverlayPosition overlayPos = Frame::OverlayPosition::LeftUp;

	userChoice = 0;
	while (std::cin >> userChoice && (!std::cin.good() || userChoice > 9 || userChoice < 1)) {
		std::cout << "Please enter correct number!" << std::endl;
		std::cin.clear();
	}

	switch (userChoice)
	{
	case 1:
		overlayPos = Frame::OverlayPosition::LeftUp;
		break;
	case 2:
		overlayPos = Frame::OverlayPosition::MiddleUp;
		break;
	case 3:
		overlayPos = Frame::OverlayPosition::RightUp;
		break;
	case 4:
		overlayPos = Frame::OverlayPosition::LeftMiddle;
		break;
	case 5:
		overlayPos = Frame::OverlayPosition::Middle;
		break;
	case 6:
		overlayPos = Frame::OverlayPosition::RightMiddle;
		break;
	case 7:
		overlayPos = Frame::OverlayPosition::LeftDown;
		break;
	case 8:
		overlayPos = Frame::OverlayPosition::MiddleDown;
		break;
	case 9:
		overlayPos = Frame::OverlayPosition::RightDown;
	}

	// Сразу создаем выходной файл
	std::string outYUVFileName = YUVFileName.erase(YUVFileName.find_last_of('.'));
	outYUVFileName += "_out.yuv";
	YUVFile out(outYUVFileName, YUVFileSize);

	// Отлично. Пользователя опросили, начинаем конвертирование
	Converter conv(6);
	conv.Initialize(overlayFrame);
	Frame convertedOverlayFrame(overlayFrame.GetSize());
	conv.Convert(&convertedOverlayFrame);

	// Итак, фрейм у нас уже загружен - мы проверяли наличие файла
	for (;;) {
		// Накладываем ...
		YUVFrame.OverlayFrame(convertedOverlayFrame, overlayPos);
		out.SaveFrame(&YUVFrame);

		// ... и пытаемся загрузить новый
		if (in.ReadFrame(&YUVFrame) != S_OK)
			break;
	}

	std::cout << "Done! Press Enter to continue..." << std::endl;
	std::getchar();
	std::getchar();
	return 0;
}

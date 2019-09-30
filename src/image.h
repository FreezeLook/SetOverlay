#pragma once
#include <stdint.h>
#include "rescode.h"
#include <string>

// Объявления вперед
class Frame;

// Основной тип пикселя для изображений
typedef struct Vector3 {
	uint8_t x;
	uint8_t y;
	uint8_t z;

	Vector3() { x = 0; y = 0; z = 0; }

	Vector3& operator=(const Vector3& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		return *this;
	};
} Color3;

// Вспомогательные типы для конвертирования
typedef struct Vector4 {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t a;

	Vector4() { x = 0; y = 0; z = 0; a = 0; }

	Vector4& operator=(const Vector4& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		a = Vec.a;
		return *this;
	};

	Vector4& operator=(const Vector3& Vec) {
		x = Vec.x;
		y = Vec.y;
		z = Vec.z;
		a = 0;
		return *this;
	};
} Color4;


// Сюда конечно можно было бы вставить Size2...
// но так как эти типы вспомогательные, то 
// обойдемся без этого. Если будем расширять плагин,
// то это обязательно нужно сделать
struct CommonFrame {
	Color3* pixels;
	size_t  size;
};


// Структура для удобной работы с размерами изображения
struct Size2 {
	size_t width;
	size_t height;

	inline unsigned int GetElementsCount() { return width * height; }
};

// Класс кадра
class Frame {
public:

	// Тип кадра .
	// TODO: пока не используется, необходимо добавить проверки
	enum FrameType {
		RGB,
		YUV
	};

	enum OverlayPosition {
		LeftUp,
		MiddleUp,
		RightUp,
		LeftMiddle,
		Middle,
		RightMiddle,
		LeftDown,
		MiddleDown,
		RightDown
	};

	// Конструктор
	// frameSize - размер кадра
	Frame(Size2 frameSize);

	// Конструктор
	// frameSize - размер кадра
	// framePixels - массив пикселей кадра
	Frame(Size2 frameSize, Vector3* framePixels);

	// Деструктор
	virtual ~Frame();

	// Получить размер изображения
	inline Size2 GetSize() { return m_ImageSize; };

	// Получить количество пикселей
	inline unsigned int GetPixelsCount() { return m_ImageSize.GetElementsCount(); };

	// Получить указатель на массив пикселей кадра
	inline Vector3* GetPixels() { return m_pPixels; };

	// Наложить один кадр на другой
	// frame - накладываемый кадр
	// pos - позиция наложения
	ResultCode OverlayFrame(Frame &frame, Frame::OverlayPosition pos);

private:
	// Размер кадра
	Size2 m_ImageSize;

	// Пиксели кадра
	Vector3* m_pPixels;
};

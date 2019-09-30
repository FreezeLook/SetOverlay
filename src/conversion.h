#pragma once
#include <condition_variable>
#include <vector>
#include <mutex>
#include "image.h"

// Данные для потоков обработки
class ConversionThreadData
{
public:

	// Функции доступа к данным

	// Установить источник
	inline void SetSource(CommonFrame source) { m_source = source; }

	// Получить источник
	inline CommonFrame& GetSource() { return m_source; }

	// Установить результат
	inline void SetDestination(const CommonFrame& destination) { m_destination = destination; }

	// Получить результат
	inline CommonFrame& GetDestination() { return m_destination; }

private:
	// Часть кадра для конвертирования
	CommonFrame m_source;

	// Место, куда поток будет сохранять конвертированные данные
	CommonFrame m_destination;
};


class Converter
{
public:
	// Конструктор
	// threadsCount - количество обрабатывающих потоков.
	// Если threadsCount == 0, то количество потоков все равно 1.
	Converter(const unsigned int threadsCount);

	// Деструктор
	virtual ~Converter();

	// Инициализация
	void Initialize(Frame& sourceFrame);

	// Конвертирование
	ResultCode Convert(Frame* outputFrame);
	

private:

	// Переменные для управления пула потоков

	// Значения для лока\анлока потоков
	const int cLockThreads = -1;
	const int cUnlockThreads = -2;

	// Переменная для синхронизации потоков.
	// Для работы потоки ожидают определенные значения, заданные выше
	int m_syncVariableValue = 0;

	// Флаг завершения работы потоков
	bool m_bJobDone = false;

	// Размер кадра
	Size2	m_pictureSize;

	// Конвертируемый кадр
	CommonFrame m_source;

	// Результат конвертирования
	CommonFrame m_destination;

	// Пул потоков
	std::vector<ConversionThreadData> m_threadPool;

	// Количество потоков, выполняющих задачу в данный момент
	unsigned int m_runningThreads = 0;

	// Количество потоков, завершивших работу
	unsigned int m_completedThreads = 0;

	// Мютекс для доступа к обрабатываемым данным
	std::mutex m_mutex;

	// Переменная синхронизации потоков
	std::condition_variable m_sync;

	// Функция, передаваемая в потоки для конвертирования
	void ConvertionTask(size_t threadIndex);

	// Функция преобразования RGB to YCbCr. Отдается рабочим потокам
	void ConvertRgbToYuv(CommonFrame& source, CommonFrame& destination);

};

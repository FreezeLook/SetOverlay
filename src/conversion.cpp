#include <algorithm>
#include <chrono>
#include <fstream>
#include <thread>
#include <cmath>

#include "conversion.h"

Converter::Converter(const unsigned int threadsCount)
	:m_completedThreads(threadsCount > 0 ? threadsCount : 1)
{
	m_threadPool.resize(threadsCount);
}

Converter::~Converter()
{
	if (m_source.pixels)
		free(m_source.pixels);
	if (m_destination.pixels)
		free(m_destination.pixels);
}

#pragma warning(push)
#pragma warning(disable : 4316)
void Converter::Initialize(Frame& sourceFrame)
{
	// Инициализируем потоки
	size_t threadsCount = m_threadPool.size();
	for (size_t threadIndex = 0; threadIndex < threadsCount; ++threadIndex)
	{
		auto newThread = std::thread
		(
			&Converter::ConvertionTask,
			this,
			threadIndex
		);

		newThread.detach();
	}
	
	// Заполняем m_source и создаем m_destination, на будущее

	// Копируем(!) кадр для обработки
	m_pictureSize = sourceFrame.GetSize();
	size_t pixelsCount = sourceFrame.GetPixelsCount();
	m_source.pixels = new Vector3[pixelsCount];
	for (size_t i = 0; i < pixelsCount; i++) {
		m_source.pixels[i] = sourceFrame.GetPixels()[i];
	}
	m_source.size = pixelsCount;

	// Создаем кадр, куда будем складывать сконвертированное
	m_destination.pixels = new Vector3[pixelsCount];
	m_destination.size = pixelsCount;
}
#pragma warning(pop)

ResultCode Converter::Convert(Frame* outputFrame)
{
	// Если у нас конечный фрейм отличается по размеру, то это ошибка
	if ((outputFrame->GetSize().height != m_pictureSize.height) ||
		(outputFrame->GetSize().width != m_pictureSize.width))
		return ERR_INVALIDPARAMS;

	size_t threadCount = m_threadPool.size();

	// Для каждого потока предназначена своя часть картинки
	size_t sizeParts = m_source.size / threadCount;

	for (size_t threadIndex = 0; threadIndex < threadCount; ++threadIndex)
	{
		// Задаем отступ и размер для создания 
		// CommonFrame(потом эта переменная идет в рабочий поток)
		size_t subpartOffset = threadIndex * sizeParts;
		size_t subpartSize = std::min(sizeParts, m_source.size - subpartOffset);

		CommonFrame& sourceSubpart = m_threadPool[threadIndex].GetSource();
		sourceSubpart.pixels = m_source.pixels + subpartOffset;
		sourceSubpart.size = subpartSize;
			
		CommonFrame& destSubpart = m_threadPool[threadIndex].GetDestination();
		destSubpart.pixels = m_destination.pixels + subpartOffset;
		destSubpart.size = subpartSize;
	}

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		// Потоки готовы к работе. Разблокируем их ( и уведомим )
		m_syncVariableValue = cUnlockThreads;
		m_sync.notify_all();
	}

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		// Ждем пока все потоки заработают...
		m_sync.wait(lock, [&]() { return m_runningThreads == threadCount; });
		// ... и когда закончат работу
		m_sync.wait(lock, [&]() { return m_completedThreads == threadCount; });
	}
		
	// Работа выполнена, собираем конечное изображение
	m_bJobDone = true;

	// Точнее просто копируем, потому что оно у нас лежим в m_destination
	for (uint32_t i = 0; i < m_pictureSize.GetElementsCount(); i++) {
		outputFrame->GetPixels()[i] = m_destination.pixels[i];
	}

	return S_OK;
}



void Converter::ConvertionTask(size_t threadIndex)
{
	while (!m_bJobDone)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			//С помощью m_sync синхронизируем начало работы потоков
			m_sync.wait(lock, [&]() { return m_syncVariableValue == cUnlockThreads; });

			m_runningThreads++;
			if (m_runningThreads == m_threadPool.size())
			{
				m_completedThreads = 0;
			}
			m_sync.notify_all();
		}

		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// Дожидаемся пока последний поток будет готов к выполнению задачи
			m_sync.wait(lock, [&]() { return m_runningThreads == m_threadPool.size(); });
			m_sync.notify_all();
		}

		// Конвертируем
		ConvertRgbToYuv
		(
			m_threadPool[threadIndex].GetSource(),
			m_threadPool[threadIndex].GetDestination()
		);

		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// Ждем пока все закончат
			m_completedThreads++;
			if (m_completedThreads == m_threadPool.size()) {
				m_syncVariableValue = cLockThreads;
				m_runningThreads = 0;
			}

			// Когда дождемся - известим
			m_sync.wait(lock, [&]() { return m_completedThreads == m_threadPool.size(); });
			m_sync.notify_all();
		}
	}
}

void Converter::ConvertRgbToYuv(CommonFrame& source, CommonFrame & destination)
{
	// Ох, как много я здесь бился с реализацией в SIMD.
	// К сожалению, вменяемого кода предложить не могу.
	// Но SIMD-реализация предполагает хорошую оптимизацию,
	// крайне заинтересован в изучении.

	size_t size = source.size;
	Vector3* rgb = source.pixels;
	Vector3* yuv = destination.pixels;
	for (size_t i = 0; i < size; i++) {
		int r = rgb[i].x;
		int g = rgb[i].y;
		int b = rgb[i].z;

		yuv[i].x = (uint8_t)std::round(0.257 * r + 0.504 * g + 0.098 * b + 16);
		yuv[i].y = (uint8_t)std::round(-0.148 * r - 0.291 * g + 0.439  * b + 128);
		yuv[i].z = (uint8_t)std::round(0.439 * r - 0.3678 * g - 0.071 * b + 128);
	}
}

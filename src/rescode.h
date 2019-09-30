#pragma once

enum  ResultCode {
	///////////////////////////
	// Коды возврата - успех
	///////////////////////////

	S_OK,					// Стандартный код успешного выполнения операции
	S_FALSE,				// Операция завершилась не удачно, но это не ошибка




	///////////////////////////
	// Коды возврата - провал
	///////////////////////////
	ERR_FAILURE,
	ERR_INVALIDPARAMS, // Переданы неправильные параметры
	ERR_OUTOFMEMORY,
	ERR_NOTIMPLEMENTED,

	// Коды возврата для файла
	ERR_FILENOTFOUND,		// Файл не найден
	ERR_READFILE,			// Ошибка при чтении файла
	ERR_WRITEFILE,			// Ошибка при записи в файл
	ERR_FILECORRUPTED,


};

// Проверка на успешность операции
inline bool Failed(ResultCode& code) {
	return (code >= ERR_FAILURE);
}

// Всякие приятные мелочи
#define ReturnIfFailed(rc)								\
	if (Failed(rc)) return rc;


#define ReturnIfNotOK(rc)								\
	if (rc != S_OK) return rc;
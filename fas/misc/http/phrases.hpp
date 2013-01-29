#ifndef FAS_MISC_HTTP_PHRASE_HPP
#define FAS_MISC_HTTP_PHRASE_HPP

namespace fas{ namespace http{

#define FAS_HTTP_PHRASE(name, text) struct name { const char* value() const { return text;} };

FAS_HTTP_PHRASE(Continue, "Continue")
FAS_HTTP_PHRASE(Switching_Protocols, "Switching Protocols")
FAS_HTTP_PHRASE(Processing, "Processing Protocols")


/// Reason Phrase ( HTTP )
FAS_HTTP_PHRASE(OK, "OK")

/**
4xx: Client Error (Ошибка клиента).
400 Bad Request (Плохой запрос).
401 Unauthorized (Неавторизован).
402 Payment Required (Необходима оплата).
403 Forbidden (Запрещено).
404 Not Found (Не найдено).
405 Method Not Allowed (Метод не поддерживается).
406 Not Acceptable (Не приемлемо).
407 Proxy Authentication Required (Необходима аутентификация прокси).
408 Request Timeout (Время ожидания истекло).
409 Conflict (Конфликт).
410 Gone (Удалён).
411 Length Required (Необходима длина).
412 Precondition Failed (Условие «ложно»).
413 Request Entity Too Large (Запрашиваемые данные слишком большие).
414 Request-URI Too Long (Запрашиваемый URI слишком длинный).
415 Unsupported Media Type (Неподдерживаемый тип данных).
416 Requested Range Not Satisfiable (Запрашиваемый диапазон не достижим).
417 Expectation Failed (Ожидаемое не приемлемо).
422 Unprocessable Entity (Необрабатываемый экзмепляр).
423 Locked (Заблокировано).
424 Failed Dependency (Невыполненная зависимость).
425 Unordered Collection (Неупорядоченный набор).
426 Upgrade Required (Необходимо обновление).
449 Retry With (Повторить с...).
*/

FAS_HTTP_PHRASE(Bad_Request, "Bad Request")
FAS_HTTP_PHRASE(Forbidden, "Forbidden")
FAS_HTTP_PHRASE(Not_Found, "Not Found")
FAS_HTTP_PHRASE(Method_Not_Allowed, "Method Not Allowed")

/*
5xx: Server Error (Ошибка сервера).
500 Internal Server Error (Внутренняя ошибка сервера).
501 Not Implemented (Не реализовано).
502 Bad Gateway (Плохой шлюз).
503 Service Unavailable (Сервис недоступен).
504 Gateway Timeout (Шлюз не отвечает).
505 HTTP Version Not Supported (Версия HTTP не поддерживается).
506 Variant Also Negotiates (Вариант тоже согласован).
507 Insufficient Storage (Переполнение хранилища).
509 Bandwidth Limit Exceeded (Исчерпана пропускная ширина канала).
510 Not Extended (Не расширено).
*/

FAS_HTTP_PHRASE(Internal_Server_Error, "Internal Server Error")
FAS_HTTP_PHRASE(Not_Implemented, "Not Implemented")

}}

#endif

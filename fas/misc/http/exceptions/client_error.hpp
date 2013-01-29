#ifndef FAS_MISC_HTTP_EXCEPTIONS_CLIENT_ERROR_HPP
#define FAS_MISC_HTTP_EXCEPTIONS_CLIENT_ERROR_HPP

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

#include "macros_defined.hpp"
#include "../phrases.hpp"

FAS_HTTP_CLIENT_ERROR(Bad_Request, 400);
FAS_HTTP_CLIENT_ERROR(Forbidden, 403);
FAS_HTTP_CLIENT_ERROR(Not_Found, 404);
FAS_HTTP_CLIENT_ERROR(Method_Not_Allowed, 405);

/*
FAS_HTTP_CLIENT_ERROR(Bad_Request, 400, "Bad Request");
FAS_HTTP_CLIENT_ERROR(Forbidden, 403, "Forbidden");
FAS_HTTP_CLIENT_ERROR(Not_Found, 404, "Not Found");
FAS_HTTP_CLIENT_ERROR(Method_Not_Allowed, 405, "Method Not Allowed");
*/

#endif

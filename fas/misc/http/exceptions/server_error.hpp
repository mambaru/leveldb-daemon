#ifndef FAS_MISC_HTTP_EXCEPTIONS_SERVER_ERROR_HPP
#define FAS_MISC_HTTP_EXCEPTIONS_SERVER_ERROR_HPP

#include "macros_defined.hpp"
#include "../phrases.hpp"

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

FAS_HTTP_SERVER_ERROR(Internal_Server_Error, 500)
FAS_HTTP_SERVER_ERROR(Not_Implemented, 501)

#endif

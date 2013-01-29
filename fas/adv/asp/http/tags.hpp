//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_TAGS_HPP
#define FAS_ADV_ASP_HTTP_TAGS_HPP

namespace fas{ namespace adv{ namespace asp{ namespace http{

/** общая цепочка вызовов:
  *      [внешний аспект] -> _input_[чтение http] -> _incoming_[ ... внутренний аспект ... ] -> _outgoing_[формирование http] -> _output_[внешний аспект]
  */

/** Вход аспекта: (сырые http данные, запрос на сервер или ответ на клиенте) */
struct _input_{};
/** Выход аспекта: (сырые http данные, ответ сервера или запрос клиент) */
struct _output_{};
/** Сюда попадают прочитанные данные (const char* d, size_t s)*/
struct _incoming_{};
/** Сюда передаются данные для формирования http пакета */
struct _outgoing_{};

/** Сюда попадают обработанные http заголовки и прочитанные данные (T, H& head, B& body)
    которые перенаправляется в _incoming_ */
struct _pre_incoming_{};
struct _pre_output_{};


/** Общие для клиента и для сервера*/
struct _reader_{};
struct _writer_{};

// Обработчик http исключений
struct _exception_{};
struct _informational_exception_{};
struct _success_exception_{};
struct _redirection_exception_{};
struct _client_error_{};
struct _server_error_{};

/** может использоваться как общий обработчик _redirection_exception_, _client_error_, _server_error_ и прочее*/
struct _http_error_{};

}}}}

#endif

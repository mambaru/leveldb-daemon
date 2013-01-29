#ifndef FAS_MISC_HTTP_TAGS_HPP
#define FAS_MISC_HTTP_TAGS_HPP

namespace fas{ namespace http{

struct _header_{};
struct _initial_{};

/** Заголовок прочитан*/
struct _head_readed_ {};
/** Тело прочитан*/
struct _body_readed_ {};

/** Готово для записи */
struct _head_ready_{};
struct _body_ready_{};

/** Тег для остальных заголовков (если используется, снижается быстродействие) */
struct _common_headers_{};

struct _content_reader_{};
struct _multipart_reader_{};
struct _chunked_reader_{};

// чистильщик 
struct _make_clean_ {};
struct _cleaner_ {};

// Груповой тег конкретного писателя тела сообщения
struct _body_writer_{};
struct _content_writer_{ };
struct _chunked_writer_{ };
struct _multipart_writer_{};


/**  URI */

// сработал обработчик по умолчанию (любой URI)
struct _default_uri_ready_ {};
struct _rule_ {};

}}

#endif

//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_TAGS_HPP
#define FAS_ADV_HTTP_TAGS_HPP

namespace fas{ namespace adv{ namespace http{

struct _guri_{};

struct _input_{};
struct _output_{};
struct _outgoing_{};
struct _incoming_{};

// Состояния чтения
struct _reader_state_{};
struct _greader_{};
struct _state_read_header_{};
struct _state_read_body_{};
struct _state_read_nobody_{};
struct _state_read_chunk_{};

// состояния записи
struct _writer_state_{};
struct _gwriter_{};
struct _state_write_initial_{};
struct _state_write_header_{};
struct _state_write_body_{};
struct _state_write_nobody_{};
struct _state_write_chunk_{};



struct _request_header_{};
struct _response_header_{};
struct _header_reader_{};
struct _body_reader_{};
struct _body_chunk_reader_{};
struct _body_buf_reader_{};
struct _nobody_reader_{};

struct _initial_writer_{};
struct _header_writer_{};
struct _body_writer_{};
struct _body_chunk_writer_{};
struct _body_buf_writer_{};
struct _nobody_writer_{};

struct _process_{};
struct _process_uri_{};
struct _writer_{};
// убрать
//  struct _after_receiver_{};

// Уведомления
struct _head_readed_{}; // заголовок прочитан
struct _body_readed_{}; // тело прочитано
struct _body_writed_{}; // тело отправлено (для chunked отправлен 0)
}}}

#endif

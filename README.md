# Общая информация

Levedb-daemon - сетевой интерфейс для key-value хранилища leveldb, работающий по протоколу JSON-RPC.

## Особенности

 - Параллельный доступ
 - Асинхронные операции
 - Поддержка репликации
 - Пакетные изменения

## Методы работы с хранилищем

 - <b>get(keyList)</b> - получение данных по списку ключей
 - <b>set(key2valueList)</b> - запись данных по списку ключей
 - <b>del(keyList)</b> - удаление по списку ключей
 - <b>inc_add(incrementList, defaultList)</b> - инкремент со значением по умолчанию
 - <b>inc(incrementList)</b> - инкремент
 - <b>update_packed({key: string, inc: key2intList, set: key2valueList, def: key2valueList})</b> - обновление json-упакованной структуры
 - <b>get_range(fromKey, toKey, limit, skipCount)</b> - получение диапазона значений
 - <b>rep_status()</b> - статус репликации. Запрос к мастеру.


# Установка и настройка

## Требования

 - [LevelDb 1.7+][1]
 - [Intel Threading Building Blocks][2] 
 - GCC 4.5+
 - Boost
 - CMake

В Gentoo нужно установить dev-libs/leveldb, dev-cpp/tbb, dev-libs/boost, dev-util/cmake

В Debian: libleveldb-dev, libtbb2, libtbb-dev, libboost-dev, libboost-thread-dev, libboost-program-options-dev, cmake

## Установка

    git clone git://github.com/mambaru/leveldb-daemon.git
    cd leveldb-daemon
    make all
    ./release/leveldb.json --help

# API


# Репликация

Чтобы реплицировать данные необходимо:

## Мастер

 - Указать порт для подключения слейвов (параметр запуска <b>--rport</b>)

## Слейв

 - Указать хост мастера (параметр запуска <b>--replication.host</b>)
 - Указать порт мастера для репликации (параметр запуска <b>--replication.port</b>). Равен <b>--rport</b> у мастера.
 - Указать уникальный идентификатор этого потока репликации (параметр запуска <b>--replication.id</b>). Текст [a-zA-Z_0-9]+. Один мастер может быть источником данных для нескольких слейвов.

# Логирование ошибок

При запуске можно указать параметр <b>--replication.host={filename}</b>, чтобы получить больше информации, почему что-то не работает.

# Лицензия

Программа-демон разработана в [компании "Мамба"][3] и распространяется по лицензии GNU GPL ver. 2.
Разработчик - Илья Шаповалов. В демоне используются следующие библиотеки:

 - [levelDb][1] - New BSD License. (c) Google
 - [faslib][4] - GPL2. (c) Владимир Мигашко, компания "Мамба"
 - [intel tbb][5] - GPL2. (c) Intel
 - [boost][6] - Boost License. 

Библиотеки, используемые в web-интерфейсе, см. в подпапках веб-интерфейса.

  [1]: http://code.google.com/p/leveldb/
  [2]: http://threadingbuildingblocks.org/ver.php?fid=182
  [3]: http://corp.mamba.ru/
  [4]: http://code.google.com/p/faslib/
  [5]: http://threadingbuildingblocks.org/
  [6]: http://www.boost.org/


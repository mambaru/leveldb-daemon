<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
//request: {"jsonrpc":"2.0","method":"set","id":0,"params":{"foo":"Hello world","bar":"{\"k1\":1,\"k2\":\"This is xopowo\"}"}}
$connect->request('set', array(
    'foo' => "Hello world",
    'bar' => json_encode(array(
        'k1' => 1,
        'k2' => 'This is xopowo',
    )),
));
//response: {"jsonrpc":"2.0","id":0,"result":true}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get","id":1,"params":["foo","bar"]}
$future = $connect->request('get', array('foo', 'bar'));
//response: {"jsonrpc":"2.0","id":1,"result":{"foo":"Hello world","bar":"{\"k1\":1,\"k2\":\"This is xopowo\"}"}}
$connect->process();
var_dump($future->get());
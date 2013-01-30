<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);

//request: {"jsonrpc":"2.0","method":"set","id":0,"params":{"structure":"{\"k1\":1,\"k2\":\"This is xopowo\",\"k3\":\"Hello world\"}"}}
$connect->request('set', array(
    'structure' => json_encode(array(
        'k1' => 1,
        'k2' => 'This is xopowo',
        'k3' => 'Hello world'
    )),
));
//response: {"jsonrpc":"2.0","id":0,"result":true}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get","id":1,"params":["structure"]}
$future = $connect->request('get', array('structure'));
//response: {"jsonrpc":"2.0","id":1,"result":{"structure":"{\"k1\":1,\"k2\":\"This is xopowo\",\"k3\":\"Hello world\"}"}}
$connect->process();
var_dump($future->get());

//request: {"jsonrpc":"2.0","method":"update_packed","id":2,"params":{"key":"structure","inc":{"k1":2,"k4":-1},"set":{"k3":"Hello world 1359555147"}}}
$connect->request('update_packed', array(
    'key' => 'structure',
    'inc' => array('k1' => 2, 'k4' => -1),
    'set' => array('k3' => 'Hello world '.time()),
));
//response: {"jsonrpc":"2.0","id":2,"result":{"k2":"This is xopowo","k4":"-1","k1":"3","k3":"Hello world 1359555147"}}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get","id":3,"params":["structure"]}
$future = $connect->request('get', array('structure'));
//response: {"jsonrpc":"2.0","id":3,"result":{"structure":"{\"k2\":\"This is xopowo\",\"k4\":\"-1\",\"k1\":\"3\",\"k3\":\"Hello world 1359555147\"}"}}
$connect->process();
var_dump($future->get());
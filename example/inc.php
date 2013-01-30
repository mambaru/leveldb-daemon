<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);

$connect->request('del', array('k1', 'k2', 'k3', 'k4'));
$connect->process();

//request: {"jsonrpc":"2.0","method":"set","id":0,"params":{"k1":1,"k2":8}}
$connect->request('set', array( 'k1' => 1, 'k2' => 8 ));
//response: {"jsonrpc":"2.0","id":0,"result":true}
$connect->process();

//request: {"jsonrpc":"2.0","method":"inc","id":1,"params":{"k1":2,"k2":-1}}
$connect->request('inc', array( 'k1' => 2, 'k2' => -1 ));
//response: {"jsonrpc":"2.0","id":1,"result":{"k1":3,"k2":7}}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get","id":2,"params":["k1","k2","k3","k4"]}
$future = $connect->request('get', array('k1', 'k2', 'k3', 'k4'));
//response: {"jsonrpc":"2.0","id":3,"result":{"k1":"3","k2":"7"}}
$connect->process();
var_dump($future->get());

//request: {"jsonrpc":"2.0","method":"inc_add","id":3,"params":{"inc":{"k1":2,"k2":-1,"k3":3,"k4":5},"def":{"k3":-8}}}
$connect->request('inc_add', array(
    'inc' => array( 'k1' => 2, 'k2' => -1, 'k3' => 3, 'k4' => 5 ),
    'def' => array( 'k3' => -8 ),
));
//response: {"jsonrpc":"2.0","id":4,"result":{"k1":5,"k2":6,"k3":-5,"k4":5}}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get","id":5,"params":["k1","k2","k3","k4"]}
$future = $connect->request('get', array('k1', 'k2', 'k3', 'k4'));
//response: {"jsonrpc":"2.0","id":5,"result":{"k1":"5","k2":"6","k3":"-5","k4":"5"}}
$connect->process();
var_dump($future->get());
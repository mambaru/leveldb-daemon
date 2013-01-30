<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
$range = array();
for( $i = 0; $i < 100; $i++ ) {
    $range["range{$i}"] = rand(0, 1000);
}

//request: {"jsonrpc":"2.0","method":"set","id":0,"params":{"range0":202,"range1":354,...."range98":683,"range99":397}}
$connect->request('set', $range);
//response: {"jsonrpc":"2.0","id":0,"result":true}
$connect->process();

//request: {"jsonrpc":"2.0","method":"get_range","id":1,"params":{"from":"range","to":"","limit":50}}
$future = $connect->request('get_range',array(
    'from' => 'range',
    'to' => '',
    'limit' => 50,
));
//response: {"jsonrpc":"2.0","id":1,"result":{"range0":"202","range1":"354",..."range52":"794","range53":"643"}}
$connect->process();
var_dump($future->get());

//request: {"jsonrpc":"2.0","method":"get_range","id":2,"params":{"from":"range","to":"range23","limit":5,"skip":2}}
$future = $connect->request('get_range',array(
    'from' => 'range',
    'to' => 'range23',
    'limit' => 5,
    'skip' => 2,
));
//response: {"jsonrpc":"2.0","id":2,"result":{"range10":"902","range11":"948","range12":"349","range13":"360","range14":"890"}}
$connect->process();
var_dump($future->get());
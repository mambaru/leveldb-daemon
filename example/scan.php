<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
$range = array();
for( $i = 0; $i < 100; $i++ ) {
    $range["range{$i}"] = rand(0, 1000);
}

$connect->request('set', $range);
$connect->process();

$feature = $connect->request('get_range',array(
    'from' => 'range',
    'to' => '',
    'limit' => 50,
));
$connect->process();
var_dump($feature->get());

$feature = $connect->request('get_range',array(
    'from' => 'range',
    'to' => 'range23',
    'limit' => 5,
    'skip' => 2,
));
$connect->process();
var_dump($feature->get());
<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
$connect->request('set', array(
    'scalar' => "Hello world",
    'array' => json_encode(array(
        'k1' => 1,
        'k2' => 'This is xopowo',
    )),
));
$connect->process();
$feature = $connect->request('get', array('scalar', 'array'));
$connect->process();
var_dump($feature->get());
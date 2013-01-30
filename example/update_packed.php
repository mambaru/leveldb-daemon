<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);

$connect->request('set', array(
    'structure' => json_encode(array(
        'k1' => 1,
        'k2' => 'This is xopowo',
        'k3' => 'Hello world'
    )),
));
$connect->process();

$feature = $connect->request('get', array('structure'));
$connect->process();
var_dump($feature->get());

$connect->request('update_packed', array(
    'key' => 'structure',
    'inc' => array('k1' => 2, 'k4' => -1),
    'set' => array('k3' => 'Hello world '.time()),
));
$connect->process();

$feature = $connect->request('get', array('structure'));
$connect->process();
var_dump($feature->get());
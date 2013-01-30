<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
$connect->request('set', array(
    'k1' => 1,
    'k2' => 8,
));
$connect->process();

$connect->request('inc', array(
    'k1' => 2,
    'k2' => -1,
));
$connect->process();

$feature = $connect->request('get', array('k1', 'k2', 'k3', 'k4'));
$connect->process();
var_dump($feature->get());

$connect->request('inc_add', array(
    'inc' => array(
        'k1' => 2,
        'k2' => -1,
        'k3' => 3,
        'k4' => 5
    ),
    'def' => array(
        'k3' => -8,
    ),
));
$connect->process();

$feature = $connect->request('get', array('k1', 'k2', 'k3', 'k4'));
$connect->process();
var_dump($feature->get());
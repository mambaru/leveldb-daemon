<?php
 
set_include_path(get_include_path().PATH_SEPARATOR.dirname(__FILE__));
include_once 'JsonRpc.php';
$config = include 'config.php';

$connect = new JsonRpc_Connection($config['host'], $config['port']);
//request: {"jsonrpc":"2.0","method":"rep_status","id":0,"params":[]}
$future = $connect->request('rep_status', array());
//response: {"jsonrpc":"2.0","id":0,"result":{"replication.test":{"id":"replication.test","state":"running","queue":0,"current":""}}}
$connect->process();
var_dump($future->get());
<?php
//ssh root@10.100.1.55 -L3307:10.100.1.51:3306
//	$F = @fsockopen('tcp://127.0.0.1',10000);

$host = '127.0.0.1';
$port = 20100;

if (!isset($argv[2])) die('usage: public_config channel [host port]');

$channel = $argv[1];
if (isset($argv[2])) $host = $argv[2];
if (isset($argv[3])) $port = $argv[3];

include "utils.php";
$conn = new JsonRpc_Connection(array('host'=>$host,'port'=>$port));

$data = CometHelper::load_from_channel($conn,$channel);
$file = tempnam('/tmp','cometcfg');
if (false===file_put_contents($file, $data)) die('cannot write to '.$file);
system("vim ".$file." > `tty`");
$filedata = file_get_contents($file);
unlink($file);
if ($filedata==$data) exit;
echo $filedata;
echo "\n";
$ret = readline('U SURE? [y/n] ');
if (strtolower(substr(trim($ret),0,1))!='y') exit;



class CometHelper {
	public static function load_from_channel($conn, $channel) {
		$ret = $conn->request('load',array('channel'=>$channel,'cursor'=>0,'limit'=>1))->get();
		if ($ret['messages']) {
			return $ret['messages'][0]['content'];
		} else return '';
	}
	public static function publish_configuration_file($conn, $channel) {
		$cfgchannel = 'comet.daemon.configuration.comet-configuration';
		$data = self::load_from_channel($conn, $cfgchannel);
		if (!$data) die('No configuration');
		
		list($config,$strconfig) = self::get_config($data);
		$new_config = self::insert_channel($config, $channel);
		if ($new_config!=$config) {
			$new_config = json_encode($new_config);
			$new_config = substr($new_config,1,strlen($new_config)-2);
			$filedata = str_replace('<<<>>>', "\n\n".$new_config.",\n\n", $strconfig);
			$params = array(
				'action' => 'publish',
				'content' => $filedata,
				'lifetime' => 86400*365*10,
				'limit' => 1,
				'identity' => '',
				'cursor'=> 1,
			);
			send('publish',array('channel'=>$cfgchannel)+$params, $conn);
			send('publish',array('channel'=>$cfgchannel.'.update')+$params, $conn);
			if (self::load_from_channel($conn, $cfgchannel)!=$filedata) die('Save failed: reason unknown'."\n");
			
		}
	}

	private static function get_config($data) {
		$mark_begin = '/*php_publish_begin<<<*/';
		$mark_end = '/*>>>php_publish_end*/';
		$pos_begin = strpos($data, $mark_begin);
		$pos_end = strpos($data,$mark_end);

		$config = substr($data,$pos_begin+strlen($mark_begin),$pos_end-$pos_begin-strlen($mark_begin) );
		$str = substr($data,0,$pos_begin+strlen($mark_begin)).'<<<>>>'.substr($data,$pos_end);

		$config = json_decode('['.trim($config).'"dummy"]',true);
		if (!$config) die('Failed while parsing config');
		unset($config[count($config)-1]);
		return array($config,$str);
	}

	private static function insert_channel($config, $channel) {
		foreach ($config as $item) {
			if ($item['name'] == $channel) {
				return $config;
			}
		}
		if (substr($channel,0,14)!='configuration.') die('invalid channel name');
		$name = str_replace('configuration.','',$channel);
		$name = str_replace('.','_',$name);
		$config[] = array(
		  "name"=>$name,
		  "enabled"=>true,
		  "filepath"=>"./configurations/daemons/".$name.".conf",
		  "publish_channel"=>$channel,
		  "publish_broker"=>"broker1",
		  "subscribe_channel"=>$channel.'.update',
		  "subscribe_broker"=>"broker1",
		  "publish_startup"=>true,
		  "load_startup"=>false,
		  "suffix"=>"./configurations/daemons/backup/".$name."-",
		  "postfix"=>".tmp",
		  "mode"=>"multiply",
		);
		return $config;
	}
}


$params = array(
	'action' => 'publish',
	'content' => $filedata,
	'lifetime' => 86400*365*10,
	'limit' => 1,
	'identity' => '',
	'cursor'=> 1,
);
send('publish',array('channel'=>$channel)+$params);
send('publish',array('channel'=>$channel.'.update')+$params);
send('load',array('channel'=>$channel,'cursor'=>0,'limit'=>1));



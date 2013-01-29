<?php
/* usage:
1) tcpdump -s 0 -w - port 21020 | gzip > dump.gz - на сервере, с которого нужно снимать нагрузку
2) zcat dump.gz | ngrep -I - -W none -q | gzip > replay.gz - на любом сервере
3) zcat replay.gz | php replay.php cdaemon22 22000 - на ctest1 или на сервере, на который нужно отправлять нагрузку

php replay.php cdaemon22 22000 10 10 - 10 соединений, при этом отправлять по 10 одновременных запросов на соединение
php replay.php cdaemon22 22000 5 1 - 5 соединений, при этом отправлять по 1 запросу на соединение (безопасно на продашне)
php replay.php cdaemon22 22000 30 100 - максимальная нагрузка

*/

$C = array();
$host = $argv[1];
$port = $argv[2];
if (!$host || !$port) die('no host&port');

$connections = 10; if (isset($argv[3])) $connections = $argv[3];
$queue = 1; if (isset($argv[4])) $queue = $argv[4];

for ($i=0;$i<$connections;$i++) {
	$C[$i] = fsockopen($host, $port);
	$S[$i] = 0;
	stream_set_timeout($C[$i],1);
}

$cnt = 0;
$f = fopen('php://stdin','r');
$last = 0;
while (!feof($f)) {
	$s = fgets($f);
	if (strlen($s)<10) continue;
	if ($s{0}!='{') continue;
	if (substr($s,0,17)!='{"jsonrpc":"2.0",') continue;
	
	$conn = $C[$cnt%count($C)];
	
	$s = trim(trim($s),'.');
	$wait = 0;
	if (substr($s,17,5)=='"id":') {
		$s = substr($s,0,22).$cnt.ltrim(substr($s,23),'0123456789');
		$wait = 1;
	} elseif (strpos(substr($s,0,60),',"id":')>0) {
		$s = preg_replace('~"method":"(.+)","id":(\d+),~','"method":"\1","id":'.$cnt.',',substr($s,0,60)).substr($s,60);
		$wait = 1;
	}
	//$s = str_replace('"calculate_total":"no"','"calculate_total":"accurate"',$s);
//	if (!($cnt%10)) usleep(1);

	if (fwrite($conn,$s."\r\n")) {
		$S[$cnt%count($C)]+=$wait;
		if ($queue && $S[$cnt%count($C)]>$queue) {
			fgets($conn);
		}
	} else {
		@fclose($conn);
		$C[$cnt%count($C)] = fsockopen($host, $port);
		$S[$cnt%count($C)] = 0;
		stream_set_timeout($C[$cnt%count($C)],1);
	}
	
	if (!($cnt%5000)) {
		echo round(5000/(microtime(true)-$last)).' rps      '."\r";
		$last = microtime(true);
	}

	$cnt++;
	
}
echo "\nDone\n";
exit;

for ($i=0;$i<$connections;$i++) {
	for ($j=0;$j<$S[$i];$j++) fgets($C[$i]);
}

<?php
include __DIR__."/Json.php";

function send($method,$data,$conn = null) {
		if (!$conn) $conn = $GLOBALS['conn'];
        echo $method."\n\t\t".json_encode($data)."\n";
        try {
                $r = $conn->request($method,$data)->get();
                echo "\t\t".var_export($r,true)."\n";
                return $r;
        } catch (Exception $e) {
                echo "FAIL: ";
                print_r($e);
        }
}
function massert($v,$v2 = true) {
        if ($v!=$v2) {
                echo "\t\tassertion failed\n";
                echo "\t\t###\t".var_export($v,true)."\n";
                echo "\t\t###\t".var_export($v2,true)."\n";
        }
        echo "\n";
}
function sorted($arr) {
        sort($arr);
        return $arr;
}
function get_key($key,$arr) {
        if (!is_array($arr)) throw new Exception($arr);
        if (!array_key_exists($key,$arr)) throw new Exception(array($key,$arr));
        return $arr[$key];
}
function get_values($key, $arr) {
	if (!is_array($arr)) throw new Exception($arr);
	$R = array();
	foreach ($arr as $item) $R[] = $item[$key];
	return $R;
}


class randomConnection {
    public $conns = array();
    public $count;
    public function __construct($count, $arr) {
        $this->count = $count;
        for ($i=0;$i<$count;$i++) $this->conns[] = new JsonRpc_Connection($arr);
    }
    public function request($a,$b) {
        return $this->conns[mt_rand(0,$this->count-1)]->request($a,$b);
    }
    public function process() {
        foreach ($this->conns as $conn) $conn->process();
    }
}

function measure($label,$cb) {
    global $conn,$conns;
    $ts = microtime(true);
    $wait = $cb($conns);
    $conns->process();
    $delta = microtime(true)-$ts;
    echo $label.sprintf(": %3.3f sec, %d req, %5.5f rps, %3.3f ms/req",$delta, count($wait), count($wait)/$delta, $delta*1000/count($wait))."\n";
    return $delta;
}

function do_measure($count,$param,$func) {
    $s = 0;
    for ($i=0;$i<$count;$i++) $s += measure($param,$func);
    return $s;
}


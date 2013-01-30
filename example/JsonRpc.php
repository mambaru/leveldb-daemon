<?php

class JsonRpc_Future
{
	private $data;
	private $failed = false;

	public function get() {
		return $this->data;
	}

	public function onReady($data) {
		$this->data = $data;
		$this->failed = false;
	}
}

/**
 * Соедирение с JSON-RPC демоном.
 */
class JsonRpc_Connection
{
	private $host;
    private $port;

    private $socket = null;
    private $failed = false;
    private $id = 0;
    private $onRecv = array();

	public function __construct($host, $port) {
		$this->host = $host;
        $this->port = (int)$port;
	}

	public function __destruct() {
		if ( $this->socket ) {
            fclose($this->socket);
        }
	}

	private function connect() {
		if ( $this->socket ) {
            return $this;
        }
		if ($this->failed) {
            return $this;
        }
		$this->socket = fsockopen($this->host, $this->port);
		$this->id = 0;
		$this->onRecv = array();
		if ( !$this->socket ) {
			$this->failed = true;
		}
		return $this;
	}

	private function send($data) {
        //echo "//request: ".json_encode($data).PHP_EOL;
		if (!fwrite($this->socket, json_encode($data)."\r\n")) {
            $this->throwEx('fwrite failed');
        }
	}

    /**
     * Вызов метода без обработки ответа.
     *
     * @param string $method
     * @param array $params
     *
     * @return JsonRpc_Connection
     */
    public function notify($method, $params) {
		if ( $this->connect()->failed ) {
            return $this;
        }
		$data = array(
            'jsonrpc' => '2.0',
            'method' => $method,
            'params' => $params,
        );
		$this->send($data);
		return $this;
	}

    public function failed() {
		return $this->failed;
	}

	private function requestCallback($method, $params, $onRecv) {
		if ( $this->connect()->failed ) {
            return $this;
        }
		$id = $this->id++;
		$this->onRecv[$id] = $onRecv;
		$data = array(
            'jsonrpc' => '2.0',
            'method' => $method,
            'id' => $id,
            'params' => $params,
        );
		$this->send($data);
		return $this;
	}

    /**
     * Асинхронный запрос.
     *
     * @param array $method
     * @param array $params
     *
     * @return JsonRpc_Future
     */
    public function request($method, $params) {
		$F = new JsonRpc_Future();
		$F->func = array($this, 'process');
		$this->requestCallback($method, $params, array($F, 'onReady'));
		return $F;
	}

	private function throwEx($message) {
		@fclose($this->socket);
		$this->socket = null;
		throw new Exception($message);
	}

    /**
     * Получить результаты асинхронных запросов.
     *
     * @return JsonRpc_Connection
     */
    public function process() {
		if ( $this->connect()->failed ) {
            foreach($this->onRecv as $future) {
                $future->failed = true;
            }
            return $this;
        }
		while ( count($this->onRecv) > 0 ) {
			$data = trim(fgets($this->socket));
			if ( !$data ) {
                $this->throwEx('no data recieved');
            }
            //echo "//response: ".$data.PHP_EOL;
			$data = json_decode($data, true);
			if ( isset($data['error']) ) {
                $this->throwEx(var_export($data['error'],1));
            }
			if ( !isset($data['id']) ) {
				$this->throwEx('id not found');
			}
			if ( !isset($this->onRecv[$data['id']]) ) {
                $this->throwEx('invalid data recieved');
            }
			call_user_func($this->onRecv[$data['id']], $data['result'], $data['id']);
			unset($this->onRecv[$data['id']]);
		}

		return $this;
	}
}


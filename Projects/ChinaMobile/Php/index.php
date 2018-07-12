<?php
require_once 'gen-php/CRSBrainServing.php';
require_once 'gen-php/Types.php';
require_once 'Thrift.php';
use App\Thrift;

// 设置中国时区
date_default_timezone_set('PRC');

class TrioBrain extends Thrift{

    protected $_port = array('10967','10967','10967');
#protected $_port = array('10967'); 

#protected $_ip = array('192.168.159.76');
    protected $_ip = array('192.168.159.76','192.168.159.77','192.168.159.78');
    protected $_balance_mode = -2; //均衡模式
    protected $_request_empty = "request";
    protected $_error_request_empty = array("ret"=>-1,"outaction"=>"10");
    protected $_error_response_empty = array("ret"=>-1,"outaction"=>"10");
    protected $_response_empty = "response_empty";
    protected $_request = "request";

    public function begin() {
        //前端必选参数判断
        $postStr = file_get_contents("php://input");
        $this->log("input+++++++++++++++++++++",$this->_request);
        $this->log($postStr, $this->_request);
        $postObj = "";


        //判断接收是否为空
        if (empty($postStr)) {
            $reply = json_encode($this->_error_request_empty, JSON_UNESCAPED_UNICODE);
            echo $reply;
            //$post_log_str =json_encode(json_decode($postStr, true));
            //$this->log($post_log_str, $this->_request_empty);
            exit;
        }
        else {
            $postObj = json_decode($postStr, true);
        }

        /*
         //判断是否userid、inparams inaction 是否为null，inter_idx
         if (is_null($postObj['userid']) || is_null($postObj['inparams'])|| is_null($postObj['inaction'])) {
           $reply = json_encode($this->_error_request_empty, JSON_UNESCAPED_UNICODE);
           //$post_log_str =json_encode(json_decode($postStr, true));
           //$this->log($post_log_str, $this->_request_empty);
           echo $reply;
           exit;
            }
         */

        if ($postObj['inaction'] == 8||$postObj['inaction'] == 9||$postObj['inaction'] == 11)  {

            return $postObj;
        }
        else {
            //待确认具体的
            $reply = json_encode(array("ret"=>-1,"outaction"=>"10"), JSON_UNESCAPED_UNICODE);
            echo $reply;
            exit;
        }
    }


    public function handle(){

        //进入PHP时间，为true返回浮点数，微秒
        $php_start = microtime(true);
        $front_params = $this->begin();
        $this->init("TrioBrain");
        if ($front_params['inaction'] == 8) {

            $InParams = new \InParams(
                array(
                    "call_id" => $front_params['inparams']['call_id'],
                    "inter_idx" => $front_params['inparams']['inter_idx'],
                    "call_dst_id" => $front_params['inparams']['call_dst_id'],
                    "call_sor_id" => $front_params['inparams']['call_sor_id'],
                    "flow" => $front_params['inparams']['flow'],
                    "auth_token" => $front_params['inparams']['auth_token'],
                    "entrance_id" => $front_params['inparams']['entrance_id'],
                    "start_time" => $front_params['inparams']['start_time'],
                    "ses_time" => $front_params['inparams']['ses_time'],
                    "suilu_region_id" => $front_params['inparams']['suilu_region_id'],
                    "cc_id" => $front_params['inparams']['cc_id'],
                    "role_label" => $front_params['inparams']['role_label']));

            $request = new \ChinaMobileBundle(
                array(
                    'userid' => (string)$front_params['userid'],
                    'inaction' => $front_params['inaction'],
                    'inparams' => $InParams));
        }
        else if ($front_params['inaction'] == 9)  {
            //begin_play;end_play;end_play;result_time;flow_result_type; input;org;newsess
            $InParams = new \InParams(
                array(
                    "call_id" => $front_params['inparams']['call_id'],
                    "inter_idx" => $front_params['inparams']['inter_idx'],
                    "begin_play" => $front_params['inparams']['begin_play'],
                    "end_play" => $front_params['inparams']['end_play'],
                    "result_time" => $front_params['inparams']['result_time'],
                    "flow_result_type" => $front_params['inparams']['flow_result_type'],
                    "input" => $front_params['inparams']['input'],
                    "inter_no" => $front_params['inparams']['inter_no'],
                    "org" => $front_params['inparams']['org'],
                    "newsess" => $front_params['inparams']['newsess']));

            $request = new \ChinaMobileBundle(
                array(
                    'userid' => (string)$front_params['userid'],
                    'inaction' => $front_params['inaction'],
                    'inparams' => $InParams));
        }

        else if ($front_params['inaction'] == 11)  {
            //trans_idx;begin_trans;end_trans;trans_result
            $InParams = new \InParams(
                array(
                    "call_id" => $front_params['inparams']['call_id'],
                    "inter_idx" => $front_params['inparams']['inter_idx'],
                    "trans_idx" => $front_params['inparams']['trans_idx'],
                    "begin_trans" => $front_params['inparams']['begin_trans'],
                    "end_trans" => $front_params['inparams']['end_trans'],
                    "trans_result" => $front_params['inparams']['trans_result']));

            $request = new \ChinaMobileBundle(
                array(
                    'userid' => (string)$front_params['userid'],
                    'inaction' => (int)$front_params['inaction'],
                    'inparams' => $InParams));
        }

        // 选择后端server
        //$this->log("选择client", $this->_request_empty);
        $selected_server = $this->selectServer($this->_ip,$this->_port,$this->_balance_mode,$front_params['userid']);
        //后端请求开始时间
        $backend_start = microtime(true);

        $result = null;
        try{
            $client = $this->getClient($selected_server);
            //$this->log("开始调用brain1", $this->_request);
            $result = $client->CRSBrainProcess($request);
            // $this->log("调用brain结束1", $this->_request);
            //$this->log($post_log_str, $this->_request_empty);
        }catch (Exception $e){

            $result = null;
        }finally{
            if(!isset($result) && $this->_balance_mode < 0){
                // 把选择的IP和port再List中去除
                $tmp_ip = $this->_ip;
                $tmp_port = $this->_port;
                $key = array_search($selected_server['ip'],$tmp_ip);
                array_splice($tmp_ip,$key,1);
                array_splice($tmp_port,$key,1);
                // 递归调用
                if(!empty($tmp_ip) && !empty($tmp_port)){
                    $selected_server = $this->selectServer($tmp_ip,$tmp_port,$this->_balance_mode,$front_params['userid']);
                    try{
                        $client = $this->getClient($selected_server);
                        // $this->log("开始调用brain2", $this->_request_empty);
                        $result = $client->CRSBrainProcess($request);
                        // $this->log("调用结束brain2", $this->_request_empty);
                    }
                    catch (Exception $e){
                        $this->log($selected_server['ip'],"noselectserver" );
                        $result = null;
                    }
                }else{
                    //$this->log($selected_server['ip'],"noselectserver3" );
                    echo json_encode(array("ret"=>-1,"outaction"=>"10"), JSON_UNESCAPED_UNICODE);
                    return;
                }
            }
        }

        //计算后端请求时间
        $backend_timecost = round(microtime(true) - $backend_start,5);
        $replys = $this->object_array($result->outparams);
        $outaction = $result->outaction;

        /*if(is_null($outaction)||is_null($replys)||is_null($replys->call_id)||is_null($result->ret)){
          $reply = json_encode($this->_error_response_empty, JSON_UNESCAPED_UNICODE);
          echo $reply;
          //$this->log("OUTPUT===================", $this->_request);
          //$this->log($reply, $this->_request);
          return;
        }*/

        if ($outaction <>9 && $outaction <>11 && $outaction<>10) {
            $ret = (int)$result->ret;
            $outaction = (int)($result->outaction);
            $ret_arr = array(
                "ret" => $ret,
                "outaction" => $outaction);
            $reply_json = json_encode($ret_arr, JSON_UNESCAPED_UNICODE);
            $this->log("output================", $this->_request);
            $this->log($reply_json, $this->_request);
            echo $reply_json;
            return;
        }


        if ($outaction == 9)  {

            $ret = (int)$result->ret;
            $outaction = (int)($result->outaction);
            $call_id = (string)($replys['call_id']);
            $inter_idx = (string)($replys['inter_idx']);
            $flow_type = (string)$replys['flow_type'];
            $flow_node_name = (string)$replys['flow_node_name'];
            $model_type = (string)$replys['model_type'];
            $prompt_type = (string)$replys['prompt_type'];
            $prompt_wav = (string)$replys['prompt_wav'];
            $prompt_text = $this->dealPromptText((string)$replys['prompt_text']);
            $timeout = (string)$replys['timeout'];
            $grammar = (string)$replys['grammar'];
            $org = (string)$replys['org'];
            $newsess = (string)$replys['newsess'];
            $delay = (string)$replys['delay'];
            $act_params = (string)$replys['act_params'];

            echo $this->replyfactory_9($front_params,$result,$ret,$outaction,$call_id,$inter_idx,$flow_type,$flow_node_name,$model_type,$prompt_type,$prompt_wav,$prompt_text,$timeout,$grammar,$org,$newsess,$delay,$act_params);

        }
        else if ($outaction == 10) {
            $ret = (int)$result->ret;
            $outaction = (int)($result->outaction);
            $call_id = (string)($replys['call_id']);
            $inter_idx = (string)($replys['inter_idx']);
            $trans_idx = (string)$replys['trans_idx'];
            $call_sor_id = (string)$replys['call_sor_id'];
            $call_dst_id = (string)$replys['call_dst_id'];
            $start_time = (string)$replys['start_time'];
            $end_time = (string)$replys['end_time'];
            $region_id = (string)$replys['region_id'];
            $entrance_id = (string)$replys['entrance_id'];
            $exit_id = (string)$replys['exit_id'];
            $user_type_id = (string)$replys['user_type_id'];
            $suilu_region_id = (string)$replys['suilu_region_id'];
            echo $this->replyfactory_10($front_params, $result, $ret, $outaction, $call_id, $inter_idx, $trans_idx, $call_sor_id, $call_dst_id, $start_time, $end_time, $region_id, $entrance_id, $exit_id, $user_type_id, $suilu_region_id);

        }



        else if ($outaction == 11)  {
            $ret = (int)$result->ret;
            $outaction = (int)($result->outaction);
            $call_id = (string)($replys['call_id']);
            $inter_idx = (string)($replys['inter_idx']);
            $trans_idx = (string)$replys['trans_idx'];
            $call_sor_id = (string)$replys['call_sor_id'];
            $trans_type = (string)$replys['trans_type'];
            $route_value = (string)$replys['route_value'];

            echo $this->replyfactory_11($front_params,$result,$ret,$outaction,$call_id,$inter_idx,$trans_idx,$call_sor_id,$trans_type,$route_value);

        }


        //php运行总时间
        global $run_start;
        $timecost = round(microtime(true) - $php_start,5);
        //$log_str = "[".$front_params['userid']."\t".$timecost."\t".$backend_timecost."\t".microtime(true)."]";
        //$this->log($log_str, "time");
    }

    protected function getClient($params) {
        /*
       总共设置10s延迟
        */
        $params['sendtimeout'] = 5000;
        $params['recvtimeout'] = 15000;
        $protocol = $this->getProtocol($params);
        $client = new \CRSBrainServingClient($protocol);

        return $client;
    }

    protected function dealPromptText($prompt_text) {
        #$prompt_text = strtolower($prompt_text);
        $prompt_text = str_replace("/MB", "每兆", $prompt_text);
        $prompt_text = str_replace("/Mb", "每兆", $prompt_text);
        $prompt_text = str_replace("/M", "每兆", $prompt_text);
        $prompt_text = str_replace("/mb", "每兆", $prompt_text);
        $prompt_text = str_replace("/m", "每兆", $prompt_text);
        $prompt_text = str_replace("MGPRS", "兆 GPRS", $prompt_text);
        return preg_replace('/(?<=\d)\.0+(?!\d)/', '', $prompt_text);
    }

    protected function log($info, $classed = 'ChinaMobile') {

        $file_name = 'log/'.$classed .'.'. date('Y-m-d-H', time()) ;
        $content = date('Y-m-d H:i:s', time()) . "\t" . $info . "\n";
        file_put_contents($file_name, $content, FILE_APPEND);
    }


    public function replyfactory_8($front_params,$result,$ret,$outaction,$call_id,$inter_idx) {
        $ret_arr = array(
            "ret" => $ret,
            "outaction" => $outaction,
            "outparams" => array(
                "call_id" => $call_id,
                "inter_idx" => $inter_idx));

        $reply_json = json_encode($ret_arr, JSON_UNESCAPED_UNICODE);
        $outparams = json_encode($result, JSON_UNESCAPED_UNICODE);
        $this->log("OUTPUT=================", $this->_request);
        $this->log($reply_json, $this->_request);
        return $reply_json;
    }

    public function replyfactory_10($front_params, $result, $ret, $outaction, $call_id, $inter_idx, $trans_idx, $call_sor_id, $call_dst_id, $start_time, $end_time, $region_id, $entrance_id, $exit_id, $user_type_id, $suilu_region_id) {
        $ret_arr = array(
            "ret" => $ret,
            "outaction" => $outaction,
            "outparams" => array(
                "call_id" => $call_id,
                "inter_idx" => $inter_idx,
                "call_sor_id" => $call_sor_id,
                "call_dst_id" => $call_dst_id,
                "start_time" => $start_time,
                "end_time" => $end_time,
                "region_id" => $region_id,
                "entrance_id" => $entrance_id,
                "exit_id" => $exit_id,
                "user_type_id" => $user_type_id,
                "suilu_region_id" => $suilu_region_id
            )
        );
        $reply_json = json_encode($ret_arr, JSON_UNESCAPED_UNICODE);
        $outparams = json_encode($result, JSON_UNESCAPED_UNICODE);
        $this->log("OUTPUT===================", $this->_request);
        $this->log($reply_json, $this->_request);
        //$this->log($outparams, $this->_request);
        return $reply_json;
    }




    public function replyfactory_9($front_params,$result,$ret,$outaction,$call_id,$inter_idx,$flow_type,$flow_node_name,$model_type,$prompt_type,$prompt_wav,$prompt_text,$timeout,$grammar,$org,$newsess,$delay,$act_params) {

        $ret_arr = array(
            "ret" => $ret,
            "outaction" => $outaction,
            "outparams" => array(
                "call_id" => $call_id,
                "inter_idx" => $inter_idx,
                "flow_type" => $flow_type,
                "flow_node_name" => $flow_node_name,
                "model_type" => $model_type,
                "prompt_type" => $prompt_type,
                "prompt_wav" => $prompt_wav,
                "prompt_text" => $prompt_text,
                "timeout" => $timeout,
                "grammar" => $grammar,
                "org" => $org,
                "newsess" => $newsess,
                "delay" => $delay,
                "act_params" => $act_params));

        $reply_json = json_encode($ret_arr, JSON_UNESCAPED_UNICODE);
        $outparams = json_encode($result, JSON_UNESCAPED_UNICODE);
        //$this->log($outparams, $this->_request);
        $this->log("OUTPUT====================", $this->_request);
        $this->log($reply_json, $this->_request);
        return $reply_json;
    }

    public function replyfactory_11($front_params,$result,$ret,$outaction,$call_id,$inter_idx,$trans_idx,$call_sor_id,$trans_type,$route_value) {

        $ret_arr = array(
            "ret" => $ret,
            "outaction" => $outaction,
            "outparams" => array(
                "call_id" =>  $call_id,
                "inter_idx" => $inter_idx,
                "trans_idx" =>  $trans_idx,
                "call_sor_id" => $call_sor_id,
                "trans_type" =>  $trans_type,
                "route_value" => $route_value));

        $reply_json = json_encode($ret_arr, JSON_UNESCAPED_UNICODE);
        $outparams = json_encode($result, JSON_UNESCAPED_UNICODE);
        $this->log("OUTPUT===================", $this->_request);
        $this->log($reply_json, $this->_request);
        //$this->log($outparams, $this->_request);
        return $reply_json;
    }

    public function object_array($array) {
        if(is_object($array)) {
            $array = (array)$array;
        } if(is_array($array)) {
            foreach($array as $key => $value) {
                $array[$key] = $this->object_array($value);
            }
        }

        return $array;
    }



    protected function selectServer($ip_list,$port_list,$mode = 0,$id = "") {
        $avalid_legth = min(count($ip_list),count($port_list));
        $adjust_mode = $avalid_legth > $mode ? $mode : $avalid_legth -1  ;

        /* adjust_mode 选择后端服务方式：
       -2  id取mod,保证上下文落在同一服务器上
           -1  均衡模式，random后端服务
         >=0 取对应元素值
          */
        switch ($adjust_mode) {
            case '-2':
                $ascii_total = 0;
                for ($i = 0; $i < mb_strlen($id); $i++) {
                    $ascii_total += ord(mb_substr($id, $i, $i + 1, 'utf-8'));
                }
                $select_index = $ascii_total % $avalid_legth;
                return array("ip" => $ip_list[$select_index],"port" => $port_list[$select_index]) ;
                break;
            case '-1':
                $select_index = mt_rand(0,$avalid_legth-1);
                return array("ip" => $ip_list[$select_index],"port" => $port_list[$select_index]) ;
                break;
            default:
                return array("ip" => $ip_list[$adjust_mode],"port" => $port_list[$adjust_mode]) ;
                break;
        }
    }

}
header("Content-type: application/json; charset=utf-8");
header("Access-Control-Allow-Origin: *");
$brain = new TrioBrain();
$brain->handle();
?>



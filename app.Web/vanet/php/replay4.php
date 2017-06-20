<?php
$num = 8;
$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
mysql_select_db("buptant",$conn);
mysql_query("set names utf8");

function readLog($filename){ 
	if(file_exists($filename)){ 
		$content = file_get_contents($filename); 
		$json = json_decode('['.$content.']',true);
		echo $filename;
	}else{ 
		$json = 'null';  
	} 
	return $json; 
} 
$data = array();
$len = array();

$data[0] = readLog("11.json");
$data[1] = readLog("22.json");
$data[2] = readLog("33.json");
$data[3] = readLog("44.json");
$data[4] = readLog("77.json");
$data[5] = readLog("88.json");
$data[6] = readLog("99.json");
$data[7] = readLog("1010.json");
$data[8] = readLog("obd.json");
$data[9] = readLog("status.json");
for($i=0; $i<$num ; $i++){
	$data[$i] = $data[$i][0];
	$len[$i] = sizeof($data[$i]);
}

function lat_shift($latitude)
{
	$lat = $latitude;
	//$lat = floor($lat)+($lat - floor($lat))*100 / 60.0;
	return $lat;
}
	
function time_shift($utc)
{	//$utc:hhmmss.sss  $date:ddmmyy
	$hour = substr($utc,0,2);
	$minute = substr($utc,2,2);
	$second = substr($utc,4,2);
	//simplified scheme
	$hour += 8;
	if ($hour >= 24)
	{
		$hour -= 24;
		$day += 1;
	}
	
	$time = sprintf("%s:%s:%s",$hour,$minute,$second);
	return $time;
}

$utc = "";
for ($i=300; $i<700; $i++){
	for($j=0; $j<$num; $j++){
		if ($j != 8){
			$t = 1000*time();
			$carID = $data[$j][$i]['carID']; 
			$utc = $data[$j][$i]['utc']; 
			$latitude = $data[$j][$i]['latitude']; 
			$longitude = $data[$j][$i]['longitude']; 
			$heading = $data[$j][$i]['heading']; 
			$speed = $data[$j][$i]['speed'];
			$mode = $data[$j][$i]['mode'];	
			$status = -1;
			$date = -1; 
			$magAngle = -1; 
			$magDir = -1; 
			$lat = lat_shift($latitude);
			$lon = lat_shift($longitude);
			$spd = $speed;
			$time = time_shift($utc);	
			if($lat != null)
				mysql_query("INSERT INTO car_info (`carID`,`status`,`longitude`,`latitude`,`speed`,`heading`,`magAngle`,`magDir`,`mode`,`timestamp`,`utc`) 
							VALUES ($carID,'$status','$lon', '$lat', '$spd', '$heading', '$magAngle', '$magDir', '$mode', '$time','$t')"); 
		}
		if ($j == 8){
			for ($k=0; $k<1000; $k++){
				if ($data[$j][$k]['obdTime'] >= $utc){
					$din = $data[$j][$k]['din'];
					$rpm = $data[$j][$k]['rpm'];
					$pedalPosition = $data[$j][$k]['pedalPosition'];
					mysql_query("INSERT INTO obd_realTimeData (`din`,`rpm`,`pedalPosition`) 
								VALUES ('$din','$rpm','$pedalPosition')");
					break;
				}
			}
			
		} 
		
	}

	usleep(200000);
} 


?>
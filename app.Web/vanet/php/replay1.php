<?php
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

$data[0] = readLog("1.json");
$data[1] = readLog("2.json");
$data[2] = readLog("3.json");
$data[3] = readLog("4.json");
$data[4] = readLog("5.json");
$data[5] = readLog("7.json");
$data[6] = readLog("8.json");
//$data[7] = readLog("10.json");
for($i=0; $i<7; $i++){
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


	for ($i=0; $i<70; $i++){
		for($j=0; $j<7; $j++){
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
			usleep(200000);
	} 
?>
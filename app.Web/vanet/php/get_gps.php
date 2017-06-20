<?php
$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
mysql_select_db("buptant",$conn);
mysql_query("set names utf8");

$data = file_get_contents("syc.log");
$buf = explode(',',$data);
echo(sizeof($buf));

$num = (sizeof($buf) - 1) / 23; 

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

for ($i=0; $i<$num; $i++){
	$t = 1000*time();
	//if ($buf[0+23*$i] == '$GPYBM'){
		//printf($buf[1]); 
		$serialNo = $buf[1+23*$i]; 
		$utc = $buf[2+23*$i]; 
		$latitude = $buf[3+23*$i]; 
		$longitude = $buf[4+23*$i]; 
		$heading = $buf[6+23*$i]; 
		$speed = $buf[11+23*$i];
		$mode = $buf[16+23*$i];	
		
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
										VALUES (10,'$status','$lon', '$lat', '$spd', '$heading', '$magAngle', '$magDir', '$mode', '$time','$t')"); 
	//}
	usleep(200000);
}
?>
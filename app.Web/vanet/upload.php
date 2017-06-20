<?php
function lat_shift($latitude)
{
	$lat = $latitude / 8 / 1000000;
	return $lat;
}
function spd_shift($speed)
{
	return ($speed / 100) * 3.6;
}
function head_shift($heading)
{
	return ($heding / 100);
}
function time_shift($time)
{
	return $time / 100;
}

$con = mysql_connect("localhost6","root","root");
if (!$con)
{
  die('Could not connect: ' . mysql_error());
}
mysql_select_db("buptant",$con);
mysql_query("set names utf8");
  
header("Content-type:text/html;charset=utf-8");   
    
echo 'php://input:<br/>';   
$data = file_get_contents('php://input');   
#print_r(urldecode($data));   

$buf = explode('&',urldecode($data));
print_r("carId=");
print_r($buf[0]);
$carId = $buf[0];
$lat = lat_shift($buf[1]);
$lon = lat_shift($buf[2]);
$spd = spd_shift($buf[3]);
$head = head_shift($buf[4]);
$time = time_shift($buf[5]);
$localTime = time();
print_r("latitude=");
print_r($lat);
print_r("longitude=");
print_r($lon);
print_r("speed=");
print_r($spd);
print_r("heading=");
print_r($head);
print_r("time=");
print_r($time);

$sql = "INSERT INTO car_gps_info (`car_id`,`longitude`,`latitude`,`speed`,`heading`,`mag_angle`,`mag_dir`,`mode`,`timestamp`,`utc`) VALUES ('$carId','$lon', '$lat', '$spd', '$head', '-1', '-1', '-1', '$time','$localTime')"; 
if (!mysql_query($sql,$con))
{
   die('Error: ' . mysql_error());
}
		
?>  
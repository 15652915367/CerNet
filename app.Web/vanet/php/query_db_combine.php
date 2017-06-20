<?php
	//$carNum = 3;
	$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
	mysql_select_db("buptant",$conn);
	mysql_query("set names utf8");
	date_default_timezone_set("PRC");

	$date = date_create();
	$local_utc = 1000*date_timestamp_get($date);
	$local_utc -= 5000;
	
	$sql_1="select * from obd_realTimeData where dynamicCarId= 'c10f43bb7e0aabc9855f' AND obdTime<='$local_utc' order by id desc limit 1";
	// $sql_2="select * from obd_realTimeData where dynamicCarId= '416941a7324d984d6c4d2ff3c3e3daac' order by id desc limit 1";
	// $sql_3="select * from obd_realTimeData where dynamicCarId= 'fa1da0605a53740445af6679b8a9fbde' order by id desc limit 1";
	
	$sql_1_="select * from car_info where carID= '1' AND timestamp<='$local_utc' order by id desc limit 1";
	
	$query=mysql_query($sql_1);
	$row =mysql_fetch_array($query);  
	$query=mysql_query($sql_1);
	$row_=mysql_fetch_array($query);
	//echo ($row['obdTime']);echo ($row['dynamicCarId']);
	
	if($row_['carID'] != null){
			$row_['timestamp'] = date("Y-m-d H:i:s",$row_['timestamp']/1000);
			 echo ( "car=".$row_['carID']
					."lon=".$row_['longitude']
					."lat=".$row_['latitude']
					."spd=".$row_['speed']
					."time=".$row_['timestamp']
					."E");
		}
		
	if($row['dynamicCarId'] != null){
			$row['obdTime'] = date("Y-m-d H:i:s",$row['obdTime']/1000);
			echo ( "car=".$row['dynamicCarId']
					."dataType=".$row['dataType']
					."spd=".$row['speed']
					."rpm=".$row['rpm']
					."mileage=".$row['mileage']
					."fuel=".$row['fuel']
					."BV=".$row['batteryVoltage']
					."malfunctionNum=".$row['malfunctionNum']
					."tim=".$row['obdTime']
					."E");
		} 
	
	
	


?>
<?php
	$carNum = $_POST["num"];
	$carNum = 10;
	$count = 0;
	$delay = 3;
	$isFilter = true;
	
	//$id = $_POST["id"];
	$conn=mysql_connect("localhost6","root","root") or die("can not connect to server");
	mysql_select_db("buptant",$conn);
	mysql_query("set names utf8");
	
	$sql_1="select * from car_gps_info where car_id= '1' order by id desc limit 1";
	$sql_2="select * from car_gps_info where car_id= '2' order by id desc limit 1";
	$sql_3="select * from car_gps_info where car_id= '3' order by id desc limit 1";
	$sql_4="select * from car_gps_info where car_id= '4' order by id desc limit 1";
	$sql_5="select * from car_gps_info where car_id= '5' order by id desc limit 1";
	$sql_6="select * from car_gps_info where car_id= '6' order by id desc limit 1";
	$sql_7="select * from car_gps_info where car_id= '7' order by id desc limit 1";
	$sql_8="select * from car_gps_info where car_id= '8' order by id desc limit 1";
	$sql_9="select * from car_gps_info where car_id= '9' order by id desc limit 1";
	$sql_10="select * from car_gps_info where car_id= '10' order by id desc limit 1";
	$sql_11="select * from car_gps_info where car_id= '11' order by id desc limit 1";

	$sql=array($sql_1,$sql_2,$sql_3,$sql_4,$sql_7,$sql_8,$sql_9,$sql_10);
	//$sql=array($sql_2,$sql_4,$sql_7,$sql_8);
	//$sql=array($sql_2);

	for ($x=0; $x<$carNum; $x++) {
	   $query=mysql_query($sql[$x]);
	   $row[$x]=mysql_fetch_array($query);  
	}
	
	for ($x=0; $x<$carNum; $x++) {
		if($row[$x][car_id] == null){
			unset($row[$x]);
		} 
	} 
	
	if($isFilter){
		$t = time();
		for ($x=0; $x<$carNum; $x++) {
			if($row[$x][car_id] != null){
				if((abs($t-$row[$x][utc]) > $delay)){
					unset($row[$x]);
				} 
			}
		} 
	}
/* 	unset($row[0]);
	unset($row[1]);
	unset($row[2]);
	unset($row[3]);
	unset($row[4]);
	unset($row[5]);
	unset($row[6]);
	unset($row[7]);
	unset($row[8]); */
	
	$count = sizeof($row);
	echo ("num=".$count."N");
	
	for ($x=0; $x<$carNum; $x++) {
		if($row[$x][car_id] != null){	
			 echo ( "car=".$row[$x][car_id]
					."lon=".$row[$x][longitude]
					."lat=".$row[$x][latitude]
					."spd=".$row[$x][speed]
					."hea=".$row[$x][heading]
					."tim=".$row[$x][timestamp]
					."E");	
		}
	}
?>

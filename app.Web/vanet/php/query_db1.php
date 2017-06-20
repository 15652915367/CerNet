<?php
	$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
	mysql_select_db("buptant",$conn);
	mysql_query("set names utf8");

	$start_id = 19571;
	$end_id = 19598;
	
	$sql_1="select * from car_info where carID= '1' order by id desc limit 1";
	$sql_2="select * from car_info where carID= '2' order by id desc limit 1";
	$sql_3="select * from car_info where carID= '3' order by id desc limit 1";
	$sql_4="select * from car_info where carID= '4' order by id desc limit 1";
	$sql_5="select * from car_info where carID= '5' order by id desc limit 1";

	$sql=array($sql_1,$sql_2,$sql_3,$sql_4,$sql_5);

	for ($x=0; $x<5; $x++) {
	   $query=mysql_query($sql[$x]);
	   $row[$x]=mysql_fetch_array($query);  
	}
	
	for ($x=0; $x<5; $x++) {
		if($row[$x][carID] != null)
			 echo ( "car=".$row[$x][carID]
					."lon=".$row[$x][longitude]
					."lat=".$row[$x][latitude]
					."spd=".$row[$x][speed]
					."tim=".$row[$x][timestamp]
					."E");
	}

?>

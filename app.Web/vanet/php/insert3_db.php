<?php

    $conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
	mysql_select_db("buptant",$conn);
	mysql_query("set names utf8");
/*
	mysql_query("INSERT INTO amap_car (`carID`,`longitude`,`latitude`,`timestamp`) VALUES ('6', '134', '134', '2016006')");
	mysql_close($conn);
	print_r("test");
*/	

	$num_of_cars = 12;
	
	$tstamp = 2016007;
	
	$longitude1_1 = 106.473443 ;$latitude1_1 = 29.677934;$heading1_1 = 90;
	$longitude1_2 = 106.473644 ;$latitude1_2 = 29.677906;$heading1_2 = 120;
	$longitude1_3 = 106.473787 ;$latitude1_3 = 29.677762;$heading1_3 = 150;
	$longitude1_4 = 106.473816 ;$latitude1_4 = 29.677620;$heading1_4 = 180;
	$longitude1_5 = 106.473797 ;$latitude1_5 = 29.677471;$heading1_5 = 210;
	$longitude1_6 = 106.473671 ;$latitude1_6 = 29.677380;$heading1_6 = 240;
	$longitude1_7 = 106.473454 ;$latitude1_7 = 29.677357;$heading1_7 = 270;
	$longitude1_8 = 106.473234 ;$latitude1_8 = 29.677396;$heading1_8 = 300;
	$longitude1_9 = 106.473116 ;$latitude1_9 = 29.677536;$heading1_9 = 330;
	$longitude1_0 = 106.473127 ;$latitude1_0 = 29.677664;$heading1_0 = 360;
	$longitude1_11 = 106.473143 ;$latitude1_11 = 29.677720;$heading1_11 = 30;
	$longitude1_12 = 106.473312 ;$latitude1_12 = 29.677860;$heading1_12 = 60;
	
	$longitude2_1 = 106.473535 ;$latitude2_1 = 29.677951;$heading2_1 = 120;
	$longitude2_2 = 106.473730 ;$latitude2_2 = 29.677836;$heading2_2 = 150;
	$longitude2_3 = 106.473813 ;$latitude2_3 = 29.677692;$heading2_3 = 180;
	$longitude2_4 = 106.473821 ;$latitude2_4 = 29.677545;$heading2_4 = 210;
	$longitude2_5 = 106.473762 ;$latitude2_5 = 29.677415;$heading2_5 = 240;
	$longitude2_6 = 106.473585 ;$latitude2_6 = 29.677359;$heading2_6 = 270;
	$longitude2_7 = 106.473325 ;$latitude2_7 = 29.677387;$heading2_7 = 300;
	$longitude2_8 = 106.473234 ;$latitude2_8 = 29.677461;$heading2_8 = 330;
	$longitude2_9 = 106.473119 ;$latitude2_9 = 29.677580;$heading2_9 = 350;
	$longitude2_0 = 106.473138 ;$latitude2_0 = 29.677729;$heading2_0 = 10;
	$longitude2_11 = 106.473213 ;$latitude2_11 = 29.677804;$heading2_11 = 30;
	$longitude2_12 = 106.473392 ;$latitude2_12 = 29.677895;$heading2_12 = 60;
	
	
	
	$longitude1 = array($longitude1_1,$longitude1_2,$longitude1_3,$longitude1_4,$longitude1_5,$longitude1_6,$longitude1_7,$longitude1_8,$longitude1_9,$longitude1_0,$longitude1_11,$longitude1_12);
	$longitude2 = array($longitude2_1,$longitude2_2,$longitude2_3,$longitude2_4,$longitude2_5,$longitude2_6,$longitude2_7,$longitude2_8,$longitude2_9,$longitude2_0,$longitude2_11,$longitude2_12);
	
	$latitude1 = array($latitude1_1,$latitude1_2,$latitude1_3,$latitude1_4,$latitude1_5,$latitude1_6,$latitude1_7,$latitude1_8,$latitude1_9,$latitude1_0,$latitude1_11,$latitude1_12);
	$latitude2 = array($latitude2_1,$latitude2_2,$latitude2_3,$latitude2_4,$latitude2_5,$latitude2_6,$latitude2_7,$latitude2_8,$latitude2_9,$latitude2_0,$latitude2_11,$latitude2_12);
	
	$heading1 = array($heading1_1,$heading1_2,$heading1_3,$heading1_4,$heading1_5,$heading1_6,$heading1_7,$heading1_8,$heading1_9,$heading1_0,$heading1_11,$heading1_12);
	$heading2 = array($heading2_1,$heading2_2,$heading2_3,$heading2_4,$heading2_5,$heading2_6,$heading2_7,$heading2_8,$heading2_9,$heading2_0,$heading2_11,$heading2_12);
	
	for ($i =0; $i<1; $i++){
		for($x=0; $x < $num_of_cars ;$x++)
		{
			$showtime=date("Y-m-d H:i:s");
			print_r($showtime);
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`) VALUES (1,'30','$longitude1[$x]', '$latitude1[$x]', '$heading1[$x]', '$showtime')");
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`) VALUES (2,'30','$longitude2[$x]', '$latitude2[$x]', '$heading2[$x]','$showtime')");
			
			sleep(1);
		}
	}
	/*while(1){
	for($x=0; $x < $num_of_cars ;$x++)
	{
		mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (1,'1000','$longitude1[$x]', '$latitude1[$x]', '$tstamp')");print_r("$longitude1[$x]");
		$tstamp += 1;
		mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (2,'1000','$longitude2[$x]', '$latitude2[$x]', '$tstamp')");print_r("$longitude2[$x]");
		$tstamp += 1;
		mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (3,'1000','$longitude3[$x]', '$latitude3[$x]', '$tstamp')");print_r("$longitude3[$x]");
		$tstamp += 1;
		mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (4,'1000','$longitude4[$x]', '$latitude4[$x]', '$tstamp')");print_r("$longitude4[$x]");
		$tstamp += 1;
		mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (5,'1000','$longitude5[$x]', '$latitude5[$x]', '$tstamp')");print_r("$longitude5[$x]");
		$tstamp += 1;
		sleep(1);
	}
	}*/
	
	
/* 	$longitude = array(
	array($longitude1_1,$longitude1_2,$longitude1_3,$longitude1_4,$longitude1_5),
	array($longitude2_1,$longitude2_2,$longitude2_3,$longitude2_4,$longitude2_5),
	array($longitude3_1,$longitude3_2,$longitude3_3,$longitude3_4,$longitude3_5),
	array($longitude4_1,$longitude4_2,$longitude4_3,$longitude4_4,$longitude4_5),
	array($longitude5_1,$longitude5_2,$longitude5_3,$longitude5_4,$longitude5_5),
	);
	$latitude = array(
	array($latitude1_1,$latitude1_2,$latitude1_3,$latitude1_4,$latitude1_5),
	array($latitude2_1,$latitude2_2,$latitude2_3,$latitude2_4,$latitude2_5),
	array($latitude3_1,$latitude3_2,$latitude3_3,$latitude3_4,$latitude3_5),
	array($latitude4_1,$latitude4_2,$latitude4_3,$latitude4_4,$latitude4_5),
	array($latitude5_1,$latitude5_2,$latitude5_3,$latitude5_4,$latitude5_5),
	); */
	
	
	
	/* print_r("$longitude[0][0].");
	print_r("$longitude1_1");
	
	for($x=0;$x<$num_of_cars;$x++)
	{
		for($y=0;$y<$num_of_cars;$y++)
		{
			mysql_query("INSERT INTO amap_car (`carID`,`speed`,`longitude`,`latitude`,`timestamp`) VALUES (6+$x,'1000','$longitude[$x][$y]', '$latitude[$x][$y]', '$tstamp')");
			$tstamp += 1;
		}
	}
	*/
	mysql_close($conn); 
	print_r("done");
	

?>
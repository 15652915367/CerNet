<?php

    $conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
	mysql_select_db("buptant",$conn);
	mysql_query("set names utf8");
/*
	mysql_query("INSERT INTO amap_car (`carID`,`longitude`,`latitude`,`timestamp`) VALUES ('6', '134', '134', '2016006')");
	mysql_close($conn);
	print_r("test");
*/	

	$num_of_cars = 5;
	
	$tstamp = 2016007;
	
	$longitude1_1 = 106.4755 ;$latitude1_1 = 29.6729;	
	$longitude1_2 = 106.4768 ;$latitude1_2 = 29.6718;
	$longitude1_3 = 106.4774 ;$latitude1_3 = 29.6724;
	$longitude1_4 = 106.4761 ;$latitude1_4 = 29.6734;
	$longitude1_5 = 106.4743 ;$latitude1_5 = 29.6739;
	
	$longitude2_1 = 106.4759 ;$latitude2_1 = 29.6734;
	$longitude2_2 = 106.4766 ;$latitude2_2 = 29.6740;
	$longitude2_3 = 106.4754 ;$latitude2_3 = 29.6749;
	$longitude2_4 = 106.4748 ;$latitude2_4 = 29.6743;
	$longitude2_5 = 106.4748 ;$latitude2_5 = 29.6743;
	
	$longitude3_1 = 106.4766 ;$latitude3_1 = 29.6740;
	$longitude3_2 = 106.4771 ;$latitude3_2 = 29.6744;
	$longitude3_3 = 106.4760 ;$latitude3_3 = 29.6754;
	$longitude3_4 = 106.4754 ;$latitude3_4 = 29.6750;
	$longitude3_5 = 106.4755 ;$latitude3_5 = 29.6748;
	
	$longitude4_1 = 106.4771 ;$latitude4_1 = 29.6744;
	$longitude4_2 = 106.4775 ;$latitude4_2 = 29.6748;
	$longitude4_3 = 106.4763 ;$latitude4_3 = 29.6758;
	$longitude4_4 = 106.4759 ;$latitude4_4 = 29.6752;
	$longitude4_5 = 106.4760 ;$latitude4_5 = 29.6753;
	
	$longitude5_1 = 106.4775 ;$latitude5_1 = 29.6748;
	$longitude5_2 = 106.4771 ;$latitude5_2 = 29.6744;
	$longitude5_3 = 106.4759 ;$latitude5_3 = 29.6752;
	$longitude5_4 = 106.4764 ;$latitude5_4 = 29.6756;
	$longitude5_5 = 106.4764 ;$latitude5_5 = 29.6757;
	
	
	$longitude1 = array($longitude1_1,$longitude1_2,$longitude1_3,$longitude1_4,$longitude1_5);
	$longitude2 = array($longitude2_1,$longitude2_2,$longitude2_3,$longitude2_4,$longitude2_5);
	$longitude3 = array($longitude3_1,$longitude3_2,$longitude3_3,$longitude3_4,$longitude3_5);
	$longitude4 = array($longitude4_1,$longitude4_2,$longitude4_3,$longitude4_4,$longitude4_5);
	$longitude5 = array($longitude5_1,$longitude5_2,$longitude5_3,$longitude5_4,$longitude5_5);
	$latitude1 = array($latitude1_1,$latitude1_2,$latitude1_3,$latitude1_4,$latitude1_5);
	$latitude2 = array($latitude2_1,$latitude2_2,$latitude2_3,$latitude2_4,$latitude2_5);
	$latitude3 = array($latitude3_1,$latitude3_2,$latitude3_3,$latitude3_4,$latitude3_5);
	$latitude4 = array($latitude4_1,$latitude4_2,$latitude4_3,$latitude4_4,$latitude4_5);
	$latitude5 = array($latitude5_1,$latitude5_2,$latitude5_3,$latitude5_4,$latitude5_5);
	
	for ($i =0; $i<5; $i++){
		for($x=0; $x < $num_of_cars ;$x++)
		{
			$showtime=date("Y-m-d H:i:s");
			print_r($showtime);
			$t = 1000*time();
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`,`utc`) VALUES (1,'1000','$longitude1[$x]', '$latitude1[$x]', '30', 001, '$t')");
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`,`utc`) VALUES (2,'2000','$longitude2[$x]', '$latitude2[$x]', '60', 002, '$t')");
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`,`utc`) VALUES (3,'3000','$longitude3[$x]', '$latitude3[$x]', '90', 003, '$t')");
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`,`utc`) VALUES (4,'4000','$longitude4[$x]', '$latitude4[$x]', '120', 004, '$t')");
			mysql_query("INSERT INTO car_info (`carID`,`speed`,`longitude`,`latitude`,`heading`,`timestamp`,`utc`) VALUES (5,'5000','$longitude5[$x]', '$latitude5[$x]', '150', 005, '$t')");
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
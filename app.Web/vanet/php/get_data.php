<?php
/* $backstr = $_POST["backstr"];
echo $backstr;
$filename = "receive.txt";
$myfile = fopen($filename, "w");
fwrite($myfile, $backstr);
fclose($myfile);  */

$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
mysql_select_db("buptant",$conn);
mysql_query("set names utf8");

$content = file_get_contents("php://input");
$uncompressed = gzinflate($content);
$filename = "receive.txt";
$myfile = fopen($filename, "w");
fwrite($myfile, $content);
fclose($myfile);
echo "success";


$headers = array(); 
foreach ($_SERVER as $key => $value) { 
    if ('HTTP_' == substr($key, 0, 5)) { 
        $headers[str_replace('_', '-', substr($key, 5))] = $value; 
    } 
}
$res = print_r($headers, true);
file_put_contents('arr.txt', $res);


$ugzipFile = fopen("ungzip.txt", "w");
$compressed   = gzdecode($content);
fwrite ($ugzipFile, $compressed );
fclose($ugzipFile); 

$obd_data   = gzdecode($content);
//$obd_data = '[{"accStatus":1,"accelerator":0,"airCondition":0,"autoLight":-1,"behindFogLights":0,"dataType":"TMCSTDP0004","dippedHeadLight":1,"doorStatus":0,"dynamicCarId":"c39c90cc9264421b5309f61fc9d48638","footbrake":1,"frontFogLights":0,"fuelAlarm":-1,"gear":3,"guardStatus":0,"handbrake":0,"hazardLight":0,"highHeadLight":0,"hood":-1,"horn":-1,"lbDoor":0,"lbLock":-1,"lbWindow":-1,"leftTurnSignal":0,"lfDoor":-1,"lfLock":-1,"lfWindow":-1,"lightStatus":-127,"lockStatus":239,"mainSafetybelt":-1,"mirrorStatus":-1,"obdTime":1478262909374,"oilAlarm":-1,"rbDoor":0,"rbLock":-1,"rbWindow":-1,"readLight":-1,"reversingLight":-1,"rfDoor":0,"rfLock":0,"rfWindow":-1,"rightTurnSignal":0,"roofLight":-1,"trunk":0,"viceSafetybelt":-1,"widthLamp":1,"winWiper":-1,"windowStatus":0},{"accStatus":1,"accelerator":0,"airCondition":0,"autoLight":-1,"behindFogLights":0,"dataType":"TMCSTDP0004","dippedHeadLight":1,"doorStatus":0,"dynamicCarId":"c39c90cc9264421b5309f61fc9d48638","footbrake":0,"frontFogLights":0,"fuelAlarm":-1,"gear":3,"guardStatus":0,"handbrake":0,"hazardLight":0,"highHeadLight":0,"hood":-1,"horn":-1,"lbDoor":0,"lbLock":-1,"lbWindow":-1,"leftTurnSignal":0,"lfDoor":-1,"lfLock":-1,"lfWindow":-1,"lightStatus":-127,"lockStatus":239,"mainSafetybelt":-1,"mirrorStatus":-1,"obdTime":1478262910375,"oilAlarm":-1,"rbDoor":0,"rbLock":-1,"rbWindow":-1,"readLight":-1,"reversingLight":-1,"rfDoor":0,"rfLock":0,"rfWindow":-1,"rightTurnSignal":0,"roofLight":-1,"trunk":0,"viceSafetybelt":-1,"widthLamp":1,"winWiper":-1,"windowStatus":0},{"accStatus":1,"accelerator":0,"airCondition":0,"autoLight":-1,"behindFogLights":0,"dataType":"TMCSTDP0004","dippedHeadLight":1,"doorStatus":0,"dynamicCarId":"c39c90cc9264421b5309f61fc9d48638","footbrake":1,"frontFogLights":0,"fuelAlarm":-1,"gear":3,"guardStatus":0,"handbrake":0,"hazardLight":0,"highHeadLight":0,"hood":-1,"horn":-1,"lbDoor":0,"lbLock":-1,"lbWindow":-1,"leftTurnSignal":0,"lfDoor":-1,"lfLock":-1,"lfWindow":-1,"lightStatus":-127,"lockStatus":239,"mainSafetybelt":-1,"mirrorStatus":-1,"obdTime":1478262909374,"oilAlarm":-1,"rbDoor":0,"rbLock":-1,"rbWindow":-1,"readLight":-1,"reversingLight":-1,"rfDoor":0,"rfLock":0,"rfWindow":-1,"rightTurnSignal":0,"roofLight":-1,"trunk":0,"viceSafetybelt":-1,"widthLamp":1,"winWiper":-1,"windowStatus":0},{"accStatus":1,"accelerator":0,"airCondition":0,"autoLight":-1,"behindFogLights":0,"dataType":"TMCSTDP0004","dippedHeadLight":1,"doorStatus":0,"dynamicCarId":"c39c90cc9264421b5309f61fc9d48638","footbrake":0,"frontFogLights":0,"fuelAlarm":-1,"gear":3,"guardStatus":0,"handbrake":0,"hazardLight":0,"highHeadLight":0,"hood":-1,"horn":-1,"lbDoor":0,"lbLock":-1,"lbWindow":-1,"leftTurnSignal":0,"lfDoor":-1,"lfLock":-1,"lfWindow":-1,"lightStatus":-127,"lockStatus":239,"mainSafetybelt":-1,"mirrorStatus":-1,"obdTime":1478262910375,"oilAlarm":-1,"rbDoor":0,"rbLock":-1,"rbWindow":-1,"readLight":-1,"reversingLight":-1,"rfDoor":0,"rfLock":0,"rfWindow":-1,"rightTurnSignal":0,"roofLight":-1,"trunk":0,"viceSafetybelt":-1,"widthLamp":1,"winWiper":-1,"windowStatus":0}]';
$obd = json_decode($obd_data,true);

//for($i=0; $i<sizeof($obd); $i++){
for($i=0; $i<1; $i++){
	
	if ($obd[$i]['dataType'] == "TMCSTDP0010"){
		$din = $obd[$i]['din'];
		$dynamicCarId = $obd[$i]['dynamicCarId'];
		$dataType = $obd[$i]['dataType'];
		$obdTime =  $obd[$i]['obdTime'];
		$stateType = $obd[$i]['stateType'];
		
		mysql_query("INSERT INTO obd_aliveStatus (`dynamicCarId`,`dataType`,`obdTime`,`stateType`) 
										VALUES ('$dynamicCarId','$dataType','$obdTime','$stateType')");

	}
	
	if ($obd[$i]['dataType'] == "TMCSTDP0008"){
		$din = $obd[$i]['din'];
		$dynamicCarId = $obd[$i]['dynamicCarId'];
		$dataType = $obd[$i]['dataType'];
		$obdTime =  $obd[$i]['obdTime'];
		$speed = $obd[$i]['speed'];
		$rpm = $obd[$i]['rpm'];
		$mileage = $obd[$i]['mileage'];
		$fuel = $obd[$i]['fuel'];
		$residualFuel = $obd[$i]['residualFuel'];
		$perResidualFuel = $obd[$i]['perResidualFuel'];
		$batteryVoltage = $obd[$i]['batteryVoltage'];
		$malfunctionNum = $obd[$i]['malfunctionNum'];
		
		mysql_query("INSERT INTO obd_realTimeData (`dynamicCarId`,`dataType`,`obdTime`,`speed`,`rpm`,`mileage`,`fuel`,`residualFuel`,`perResidualFuel`,`batteryVoltage`,`malfunctionNum`) 
										VALUES ('$dynamicCarId','$dataType','$obdTime','$speed','$rpm','$mileage','$fuel','$residualFuel','$perResidualFuel','$batteryVoltage','malfunctionNum')");
	}
	
	if ($obd[$i]['dataType'] == "TMCSTDP0004"){
		$din = $obd[$i]['din'];
		$dynamicCarId = $obd[$i]['dynamicCarId'];
		$dataType = $obd[$i]['dataType'];
		$obdTime =  $obd[$i]['obdTime'];
		$footbrake = $obd[$i]['footbrake'];
		$accelerator = $obd[$i]['accelerator'];
		$handbrake = $obd[$i]['handbrake'];
		$leftTurnSignal = $obd[$i]['leftTurnSignal'];
		$rightTurnSignal = $obd[$i]['rightTurnSignal'];
		
		mysql_query("INSERT INTO obd_safetyStatus (`dynamicCarId`,`dataType`,`obdTime`,`footbrake`,`accelerator`,`handbrake`,`leftTurnSignal`,`rightTurnSignal`) 
										VALUES ('$dynamicCarId','$dataType','$obdTime','$footbrake','$accelerator','$handbrake','$leftTurnSignal','$rightTurnSignal')");

	}
	
	if ($obd[$i]['dataType'] == "TMCSTDP0012"){
		$din = $obd[$i]['din'];
		$dynamicCarId = $obd[$i]['dynamicCarId'];
		$dataType = $obd[$i]['dataType'];
		$obdTime =  $obd[$i]['obdTime'];
		$eventType = $obd[$i]['eventType'];
		
		mysql_query("INSERT INTO obd_drivingEvent (`dynamicCarId`,`dataType`,`obdTime`,`eventType`) 
										VALUES ('$dynamicCarId','$dataType','$obdTime','$eventType')");

	}
	
	if ($obd[$i]['dataType'] == "TMCSTDP0007"){
		$din = $obd[$i]['din'];
		$dynamicCarId = $obd[$i]['dynamicCarId'];
		$dataType = $obd[$i]['dataType'];
		$obdTime =  $obd[$i]['obdTime'];
		$longitude = $obd[$i]['longitude'];
		$latitude = $obd[$i]['latitude'];
		$altitude = $obd[$i]['altitude'];
		$speed = $obd[$i]['speed'];
		$heading = $obd[$i]['heading'];
		$satellites = $obd[$i]['satellites'];
		$dataValid = $obd[$i]['dataValid'];
		
		mysql_query("INSERT INTO obd_gpsData (`din`,`dynamicCarId`,`dataType`,`obdTime`,`longitude`,`latitude`,`altitude`,`speed`,`heading`,`satellites`,`valid`) 
										VALUES ('$din','$dynamicCarId','$dataType','$obdTime','$longitude','$latitude','$altitude','$speed','$heading','$satellites','$dataValid')");
	}
}


mysql_close($conn); 



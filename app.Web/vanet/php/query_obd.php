<?php
	$id = $_POST["index"];
	
	if($id > -1){
		$conn=mysql_connect("120.55.170.85","buptant","buptant1014") or die("can not connect to server");
		mysql_select_db("buptant",$conn);
		mysql_query("set names utf8");
		

		$obd_real1="select * from obd_realTimeData where din= 'S534100000796' order by id desc limit 1";
		$obd_real2="select * from obd_realTimeData where din= 'S534100001623' order by id desc limit 1";
		$obd_real3="select * from obd_realTimeData where din= 'S534100001621' order by id desc limit 1";
		$obd_real4="select * from obd_realTimeData where din= 'S534100001628' order by id desc limit 1";
		$obd_real5="select * from obd_realTimeData where din= 'S534100001667' order by id desc limit 1";
		$obd_real6="select * from obd_realTimeData where din= 'S534100001662' order by id desc limit 1";
		
		$obd_real7="select * from obd_realTimeData where din= 'S534100001666' order by id desc limit 1";
		$obd_real8="select * from obd_realTimeData where din= 'S534100001665' order by id desc limit 1";
		$obd_real9="select * from obd_realTimeData where din= 'S534100001664' order by id desc limit 1";
		$obd_real10="select * from obd_realTimeData where din= 'S534100001626' order by id desc limit 1";
		$obd_real11="select * from obd_realTimeData where din= 'S534100001626' order by id desc limit 1";

		$obd_safety1="select * from obd_safetyStatus where din= 'S534100000796' order by id desc limit 1";
		$obd_safety2="select * from obd_safetyStatus where din= 'S534100001623' order by id desc limit 1";
		$obd_safety3="select * from obd_safetyStatus where din= 'S534100001621' order by id desc limit 1";
		$obd_safety4="select * from obd_safetyStatus where din= 'S534100001628' order by id desc limit 1";
		$obd_safety5="select * from obd_safetyStatus where din= 'S534100001667' order by id desc limit 1";
		$obd_safety6="select * from obd_safetyStatus where din= 'S534100001662' order by id desc limit 1";
		
		$obd_safety7="select * from obd_safetyStatus where din= 'S534100001666' order by id desc limit 1";
		$obd_safety8="select * from obd_safetyStatus where din= 'S534100001665' order by id desc limit 1";
		$obd_safety9="select * from obd_safetyStatus where din= 'S534100001664' order by id desc limit 1";
		$obd_safety10="select * from obd_safetyStatus where din= 'S534100001626' order by id desc limit 1";
		$obd_safety11="select * from obd_safetyStatus where din= 'S534100001626' order by id desc limit 1";

		$obdReal=array($obd_real1,$obd_real2,$obd_real3,$obd_real4,$obd_real5,$obd_real6,$obd_real7,$obd_real8,$obd_real9,$obd_real10,$obd_real11);
		$obdSafety=array($obd_safety1,$obd_safety2,$obd_safety3,$obd_safety4,$obd_safety5,$obd_safety6,$obd_safety7,$obd_safety8,$obd_safety9,$obd_safety10,$obd_safety11);

		$query_obdReal = mysql_query($obdReal[$id]);
		$row_obdReal =mysql_fetch_array($query_obdReal);
		$query_obdSafety = mysql_query($obdSafety[$id]);
		$row_obdSafety = mysql_fetch_array($query_obdSafety);
		
		echo("dyn=".$row_obdReal['din']
			 ."rpm=".$row_obdReal['rpm']
			 ."acc=".$row_obdReal['pedalPosition']
			 ."foo=".$row_obdSafety['footbrake']
			 ."han=".$row_obdSafety['handbrake']
			 ."lef=".$row_obdSafety['leftTurnSignal']
			 ."rig=".$row_obdSafety['rightTurnSignal']
			 .'E');
	}
	else
		echo "Na";

?>

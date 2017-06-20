<?php

function distance($lat1, $lng1, $lat2, $lng2){
        return (2*ATAN2(SQRT(SIN(($lat1-$lat2)*PI()/180/2)
        *SIN(($lat1-$lat2)*PI()/180/2)+
        COS($lat2*PI()/180)*COS($lat1*PI()/180)
        *SIN(($lng1-$lng2)*PI()/180/2)
        *SIN(($lng1-$lng2)*PI()/180/2)),
        SQRT(1-SIN(($lat1-$lat2)*PI()/180/2)
        *SIN(($lat1-$lat2)*PI()/180/2)
        +COS($lat2*PI()/180)*COS($lat1*PI()/180)
        *SIN(($lng1-$lng2)*PI()/180/2)
        *SIN(($lng1-$lng2)*PI()/180/2))))*6378140;
    }

$lat = array(29.6713,29.6713 ,29.67315871,29.67315897,29.67315991,29.67316123,29.67316298);
//$lat = array(29.6713,29.6713);
$lon = array(106.48812,106.48811,106.4757408,106.475741,106.4757417,106.4757426,106.4757439);
//$lng = array(106.4881,106.4822);

for($i=0;$i<6;$i++)
{
	printf("%s\n",distance($lat[$i],$lon[$i],$lat[$i+1],$lon[$i+1]));
}	



?>
<?php
/* function getMillisecond() {
list($t1, $t2) = explode(' ', microtime());
return (float)sprintf('%.0f',(floatval($t1)+floatval($t2))*1000);
} */
$time = 1478222387000;
echo date("Y-m-d h:i:s",$time);
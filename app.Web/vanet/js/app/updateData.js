var newSpd;
var newHea;
var newTime;

var viewHea;
var viewSpd;
var viewAcc = 0;
var preSpd = -1;

var xmlhttp;
var url = "php/query_db.php";

function updateData()
{
	xmlhttp=null;
	if (window.XMLHttpRequest)
	{// all modern browsers
		xmlhttp=new XMLHttpRequest();
	}
	else if (window.ActiveXObject)
	{// for IE5, IE6
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	if (xmlhttp!=null)
	{
		xmlhttp.onreadystatechange=state_Change;
		xmlhttp.open("POST",url,true);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("num="+carNum);
	}
	else
	{
		alert("Your browser does not support XMLHTTP.");
	}
	
}

//数据返回后
function state_Change()
{
	//var rep=xmlhttp.responseText;
	//alert( xmlhttp.readyState + "::" +rep);
	if (xmlhttp.readyState==4)
	{// 4 = "loaded"
		if (xmlhttp.status==200)
		{// 200 = "OK"
			var rep=xmlhttp.responseText;
			dataDump(rep);
			//alert(rep);
			if(rep != null)
				//更新地图信息
				updateMapInfo();
			
			setTimeout("updateData()",updateIntval); 
    }
	else
    {
		//alert("Problem retrieving data:" + xmlhttp.statusText);
    }
  }
}

function delUnaliveControl(i){
	for(var j=0; j<aliveNum; j++){
		if(i == (parseInt(carIDs[j]) - 1))
			return;
	}
	map.removeControl(myZoomCtrls[i]);
}

function updateMapInfo(){
	for(var i=0; i<carNum; i++){
		delUnaliveControl(i);
	} 
	if(aliveNum >0){
		for(var i=0; i<aliveNum; i++){
			var index = parseInt(carIDs[i]) - 1;	
			map.addControl(myZoomCtrls[index]);				
		}
		alivePoss.splice(0,alivePoss.length);
		for(var i=0; i<aliveNum; i++){
			
			/* alivePoss[i] = new BMap.Point(longitudes.shift().toString().substring(0,10)
										, latitudes.shift().toString().substring(0,9)); */
			alivePoss[i] = new BMap.Point(longitudes.shift()
										, latitudes.shift());
		}
		var convertor = new BMap.Convertor();
		convertor.translate(alivePoss, 1, 5, translateCallback);
	}
	
	var arrayIndex = carIDs.indexOf((viewIndex+1).toString());
    console.log(arrayIndex);
	if(viewIndex == -1){
			document.getElementById("longitude").value = "";
			document.getElementById("latitude").value = "";
			document.getElementById("heading").value = "";
		}
	else if(arrayIndex != -1){
			console.log(arrayIndex);		
			document.getElementById("longitude").value = alivePoss[arrayIndex].lng.toFixed(6) + "° E";
			document.getElementById("latitude").value = alivePoss[arrayIndex].lat.toFixed(6) + "°  N";
			//alert(arrayIndex);
			if(viewIndex<=5){
				document.getElementById("carImage").src = "img/ca.png";
				document.getElementById("carID").innerHTML = "北邮 "+(viewIndex+1) + "号";
			}
			else{ 
				document.getElementById("carImage").src = "img/ca.png";
				document.getElementById("carID").innerHTML = "北邮 "+(viewIndex) + "号";
			}
			/* else 
				document.getElementById("carID").innerHTML = "悦翔v7 7号"; */
			if(viewSpd > 2){
				var direction = translateHeading(viewHea);
				document.getElementById("heading").value = direction;
			}
			else{
				document.getElementById("heading").value = "--";
			}
	}	
	//document.getElementById("heading").value = "东南";
}

function monitor()
{
	for(var i=0; i<carNum; i++){
		if(clearFlag[i] == true){
			animations[i].clearCar();
			animations[i].hideInfoWindow();
			animations[i].setAutoView(false);
			
			if(viewIndex == i){
				viewIndex = -1;
				viewSpd = 0;
				viewAcc = 0;
				preSpd = -1;
			}
		}
		else
			clearFlag[i] = true;
	}
	setTimeout("monitor()",3000);
}

translateCallback = function (data){
    if(data.status === 0) {	
        for (var i = 0; i < data.points.length; i++) {
			data.points[i].lng = parseFloat(data.points[i].lng.toString().substring(0,9));
			data.points[i].lat = parseFloat(data.points[i].lat.toString().substring(0,8)); 
			//data.points[i].lng = parseFloat(data.points[i].lng).toFixed(5);
			//data.points[i].lat = parseFloat(data.points[i].lat).toFixed(5); 
			var index = parseInt(carIDs[i]) - 1;	
			newSpd = speeds.shift();
			newHea = headings.shift();
			newTime = timestamps.shift();
			if(index == viewIndex){
				viewHea = newHea;
				viewSpd = newSpd;
				
				if(preSpd != -1)
					viewAcc = (viewSpd - preSpd)*5;
				preSpd = newSpd;
			}
			
			animations[index].start(data.points[i], newSpd, newHea);
			
			if(index <=5)
				animations[index].setContent(
										"北邮" + (index+1) +"号<br>");	
										//+ newTime);	
			if(index > 5 && index <= 9)
				animations[index].setContent(
									    "北邮 " + (index+1) + "号" + "<br>");	
										//+ newTime);
			if(index == 10)
				animations[index].setContent(
										 "北邮");
			
			clearFlag[index] = false;
        }
    } 	
}

function translateHeading(str) {
	var num = parseFloat(str).toFixed(1);
	var direction;
	// 0-180   -180~0

	if(num>180&&num<=360){
		num = num - 360;
	}
	NUM = Math.abs(num);
	// -10~10
	if(num>-10&&num<=10){
		direction = "正北";
	}else if(num>10&&num<=80){
		direction = "东北";
	}else if(num>80&&num<=100){
		direction = "正东";
	}else if(num>100&&num<=170){
		direction = "东南";
	}else if(NUM>170&&NUM<=180){
		direction = "正南";
	}else if(num>-170&&num<=-110){
		direction = "西南";
	}else if(num>-110&&num<=-80){
		direction = "正西";
	}else {
		direction = "西北";
	}

	return direction;
}

function changeView()
{
	if(aliveNum > 0){	
		viewIndex ++;	
 		while(carIDs.indexOf((viewIndex+1).toString()) == -1){
			viewIndex ++;
			if(viewIndex > carNum)
				viewIndex = 0;
		} 
		
 		for (var i=0; i<aliveNum; i++){
			var index = parseInt(carIDs[i]) - 1;	
			if(index != viewIndex){
				document.getElementById("btn_"+index).style.backgroundColor = "";
				animations[parseInt(index)].setAutoView(false);
				animations[parseInt(index)].setBackgroundColor(false);
				animations[parseInt(index)].hideInfoWindow();
				document.getElementById("btn_"+index).style.color = "gray";
				document.getElementById("btn_"+index).style.border = "1px solid gray";
			 }
		} 
				  
	 	animations[parseInt(viewIndex)].setAutoView(false);
		//animations[parseInt(viewIndex)].panToMe();
		animations[parseInt(viewIndex)].setZindex(true);
		animations[parseInt(viewIndex)].setBackgroundColor(true);
		animations[parseInt(viewIndex)].hideInfoWindow();
		document.getElementById("btn_"+viewIndex).style.backgroundColor = "";
		document.getElementById("btn_"+viewIndex).style.color = "yellow";
		document.getElementById("btn_"+viewIndex).style.border = "1px solid yellow";

		viewAcc = 0;
	}
	setTimeout("changeView()",5000);
}


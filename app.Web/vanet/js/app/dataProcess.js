var strNum = "num=";
var strCarID = "car=";
var strLon = "lon=";
var strLat = "lat=";
var strSpd = "spd=";
var strHea = "hea=";
var strTim = "tim=";
var strE   = "E"
var strN   = "N"

var strDyn = "dyn=";
var strRpm = "rpm=";
var strAcc = "acc=";
var strFoo = "foo=";
var strHan = "han=";
var strLef = "lef=";
var strRig = "rig=";

var strLen = 4;

function initData(){

}

function dataDump(str){
	getAliveNum(str);
	carIDConvert(str);
	lonConvert(str);
	latConvert(str);
	spdConvert(str);
	heaConvert(str);
	timConvert(str);	
}

function obdDataDump(str) {
	// dynamicCarConvert(str);
	rpmConvert(str);
	accConvert(str);
	fooConvert(str);
	hanConvert(str);
	lefConvert(str);	
	rigConvert(str);
}

function getAliveNum(str){
	var st;
	var end;
	st = str.indexOf(strNum) + strLen;
	end = str.indexOf(strN);
	aliveNum = str.substring(st, end);
	str = str.substring(strLen + 1);
	//alert(aliveNum);
}
function carIDConvert(str){
	var st;
	var end;
	carIDs.splice(0,carIDs.length);
	for(var i=0; i<aliveNum; i++){
		st 	= str.indexOf(strCarID) + strLen;
		end = str.indexOf(strLon);
		carIDs[i] = str.substring(st, end);
		str = str.substring(str.indexOf(strE) + 1);
	}
}
function lonConvert(str){
	var st;
	var end;
	for(var i=0; i<aliveNum; i++){		
		st = str.indexOf(strLon) + strLen;
		end = str.indexOf(strLat);
		longitudes.push(str.substring(st, end));
		str = str.substring(str.indexOf(strE) + 1);
	}
}
function latConvert(str){
	var st;
	var end;
	for(var i=0; i<aliveNum; i++){	
		st = str.indexOf(strLat) + strLen;
		end = str.indexOf(strSpd);
		latitudes.push(str.substring(st, end));
		str = str.substring(str.indexOf(strE) + 1);
	}
}
function spdConvert(str){
	var st;
	var end;
	for(var i=0; i<aliveNum; i++){		
		st = str.indexOf(strSpd) + strLen;
		end = str.indexOf(strHea);
		speeds.push(str.substring(st, end));
		str = str.substring(str.indexOf(strE) + 1);
	}
}
function heaConvert(str){
	var st;
	var end;
	for(var i=0; i<aliveNum; i++){		
		st = str.indexOf(strHea) + strLen;
		end = str.indexOf(strTim);
		headings.push(str.substring(st, end));
		str = str.substring(str.indexOf(strE) + 1);	
	}
	
}
function timConvert(str){
	var st;
	var end;
	for(var i=0; i<aliveNum; i++){	
		st = str.indexOf(strTim) + strLen;
		end = str.indexOf(strE);
		timestamps.push(str.substring(st, end));
		str = str.substring(str.indexOf(strE) + 1);
	}
}
function rpmConvert(str){
	var st;
	var end;
	st = str.indexOf(strRpm) + strLen;
	end = str.indexOf(strAcc);
	rpm = str.substring(st, end);
}
function accConvert(str){
	var st;
	var end;
	st = str.indexOf(strAcc) + strLen;
	end = str.indexOf(strFoo);
	accelerator = (str.substring(st, end));
}
function fooConvert(str){
	var st;
	var end;
		st = str.indexOf(strFoo) + strLen;
		end = str.indexOf(strHan);
		footbrake = (str.substring(st, end));
}
function hanConvert(str){
	var st;
	var end;
		st = str.indexOf(strHan) + strLen;
		end = str.indexOf(strLef);
		handbrake = (str.substring(st, end));

}
function lefConvert(str){
	var st;
	var end;
		st = str.indexOf(strLef) + strLen;
		end = str.indexOf(strRig);
		leftsignal = (str.substring(st, end));
}
function rigConvert(str){
	var st;
	var end;
		st = str.indexOf(strRig) + strLen;
		end = str.indexOf(strE);
		rightsignal = (str.substring(st, end));
}
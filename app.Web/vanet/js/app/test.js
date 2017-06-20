define(function (){

	function checkTime(i){
		if(i<10){
			i="0"+i;
		}
		return i;
	};	
return {
	startTime: function(){
	var today =  new Date();
	var y = today.getFullYear();
	var mon = today.getMonth() + 1;
	var d = today.getDate();
	var h = today.getHours();
	var min = today.getMinutes();
	var s = today.getSeconds();
	min = checkTime(min);
	s = checkTime(s);
	document.getElementById('time').innerHTML = y + '/' + mon + '/'+d + ' ' +h +':'+ min +':'+ s;
	setTimeout('startTime()',500);
	}	
}
});
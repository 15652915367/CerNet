var obdIndex;
function obdDataUpdate() {
	obdIndex = viewIndex;
	var url = "php/query_obd.php"
	$.ajax ({
		url: url,
		type: "POST",
	    dataType: "text",
		data: {
			index:obdIndex
		},
		success: function(data){
			obdDataDump(data);
			//alert(data);
			dataDisplay();
			setTimeout(function(){
				obdDataUpdate()
			},200)
			console.log(accelerator);
		},
		error: function(){
			//alert ("异步请求obd数据出错！");
		}
	})
}

function dataDisplay() {
		switch(footbrake){
		 	/* case '1': footbrake = "踩下"; break;
			case '0': footbrake = "松开"; break;
			case '-1': footbrake = "--"; break;  */
			default : footbrake = "--";
		}

		switch(handbrake){
	 		/* case '1': handbrake = "拉起"; break;
			case '0': handbrake = "放下"; break;
			case '-1': handbrake = "--"; break; */
			default : handbrake = "--";
		}

		turnsignal = leftsignal+rightsignal;
		
		switch(turnsignal) {
			/* case '-1-1': turnsignal = "--";break;
			case '11': turnsignal = "双闪";break;
			case '10': turnsignal = "左转";break;
			case '01': turnsignal = "右转";break;
			case '00': turnsignal = "关闭";break; */
			default : turnsignal = "--";
		}
		if(rpm == "")
			document.getElementById("rpm").value = "--";
		else
			document.getElementById("rpm").value = "--";
		if((accelerator == "")||(accelerator == -1))
			document.getElementById("accelerator").value = "--";
		else
			document.getElementById("accelerator").value = "--";
		//document.getElementById("accelerator").value = "34%";
		//document.getElementById("accelerator").value = accelerator;
		document.getElementById("footBrake").value = footbrake;
		document.getElementById("handBrake").value = handbrake;
		document.getElementById("turnSignal").value = turnsignal;
		
		/*switch (viewIndex){
			case 0: document.getElementById("accelerator").value = parseFloat(21.3)+ "%";break;
			case 1: document.getElementById("accelerator").value = parseFloat(24.6)+ "%";break;
			case 2: document.getElementById("accelerator").value = parseFloat(42.5)+ "%";break;
			case 3: document.getElementById("accelerator").value = parseFloat(27.6)+ "%";break;
			case 6: document.getElementById("accelerator").value = parseFloat(12.3)+ "%";
					document.getElementById("rpm").value = parseFloat(2421)+ " 转/分";break;
			case 7: document.getElementById("accelerator").value = parseFloat(32.5)+ "%";break;
			case 8: document.getElementById("accelerator").value = parseFloat(23.2)+ "%";break;
			case 9: document.getElementById("accelerator").value = parseFloat(34.5)+ "%";
					document.getElementById("rpm").value = parseFloat(1426)+ " 转/分";break;
			default: break;
		}*/
}

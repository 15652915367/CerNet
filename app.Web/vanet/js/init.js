	var gauge1;

	var points = [new BMap.Point(116.3786889372559,39.90762965106183),
                  new BMap.Point(116.38632786853032,39.90795884517671),
                  new BMap.Point(116.39534009082035,39.907432133833574),
                  new BMap.Point(116.40624058825688,39.90789300648029),
                  new BMap.Point(116.41413701159672,39.90795884517671)
    ];

	
	var carNum;			
	var aliveNum = -1;
	var viewIndex = 0;
	var initAngle;
    var cars = new Array();
	var animations = new Array();
	var myZoomCtrls = new Array();
	
	var carIDs 				= new Array();
	var longitudes 			= new Array();
	var latitudes 			= new Array();
	var speeds 				= new Array();
	var headings 			= new Array();
	var timestamps 			= new Array();
	
	var	rpm;
	var	accelerator;
	var	footbrake;
	var	handbrake;
	var	leftsignal;
	var	rightsignal;
	var turnsignal;
	
	var aliveCars = new Array();
	var alivePoss = new Array();
	var aliveSpds = new Array();
	var aliveTims = new Array();
	
	var clearFlag = new Array();
	
	var xmlhttp;
	var url = "php/query_db.php";

	//初始化地图车辆数目
	carNum = 10;
	//初始化数据更新频率
	updateIntval = 1000;
	//初始化数据
	initData();
	//startTime();
	
	// 百度地图API功能
	var map = new BMap.Map("allmap", {mapType: BMAP_NORMAL_MAP});    // 创建Map实例 
	var mapWforGPS = new BMapLib.MapWrapper(map, BMapLib.COORD_TYPE_GPS); 
	var cr = new BMap.CopyrightControl({anchor: BMAP_ANCHOR_BOTTOM_RIGHT});   //设置版权控件位置
	var carIcon = new BMap.Icon("img/car-fushi2.png", new BMap.Size(30, 15),{imageSize: new BMap.Size(30, 15)}); 
	var truckIcon = new BMap.Icon("img/car-fushi2.png", new BMap.Size(30, 15),{imageSize: new BMap.Size(30, 15)}); 
	map.centerAndZoom(new BMap.Point(116.3645, 39.967987), 15);  // 初始化地图,设置中心点坐标和地图级别
	map.enableKeyboard();
	var top_left_control = new BMap.ScaleControl({anchor: BMAP_ANCHOR_TOP_LEFT});// 左上角，添加比例尺
	map.addControl(new BMap.MapTypeControl());   //添加地图类型控件
	
	map.setCurrentCity("重庆");          // 设置地图显示的城市 此项是必须设置的
	map.enableScrollWheelZoom(true);     //开启鼠标滚轮缩放
	map.addControl(cr); //添加版权控件
	map.addControl(top_left_control);    
	var bs = map.getBounds();   //返回地图可视区域
	cr.addCopyright({id: 1, content: "<a href='#' style='position:absolute;font-size:20px;font-weight:bolder;color:#ffffff;bottom:5px;right:130px'>北京邮电大学</a> <img style='width:100px;height:100px;' src='img/bupt.png'>"}); 
	
	cars[0] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[1] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[2] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[3] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[4] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[5] = new BMap.Marker(new BMap.Point(0, 0), {icon: carIcon});
	cars[6] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});
	cars[7] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});
	cars[8] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});
	cars[9] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});
	cars[10] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});

	completeMapInit();
	function completeMapInit() {
		for(var i=0; i<carNum; i++){
			//cars[i] = new BMap.Marker(new BMap.Point(0, 0), {icon: truckIcon});
			myZoomCtrls[i] = new ZoomControl(i, (i+1)*30);
			//map.addControl(myZoomCtrls[i]);	
			map.addOverlay(cars[i]);
			//map.addOverlay(cars[10]);			
			animations[i] = new BMapLib.LuShu(map, i, mapWforGPS,cars[i],{
                autoView:true,//是否开启自动视野调整，如果开启那么路书在运动过程中会根据视野自动调整
                enableRotation:true,//是否设置marker随着道路的走向进行旋转
            });  			
		}
		updateData();
		obdDataUpdate();
		monitor();
		changeView();
    }
		

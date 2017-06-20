// 定义一个控件类,即function
	function ZoomControl(index, yOffset){
	  // 默认停靠位置和偏移量
	  this.carID = index;
	  this.defaultAnchor = BMAP_ANCHOR_TOP_RIGHT;
	  this.defaultOffset = new BMap.Size(10, yOffset);
	  this.isPush = false;
	}

	// 通过JavaScript的prototype属性继承于BMap.Control
	ZoomControl.prototype = new BMap.Control();

	// 自定义控件必须实现自己的initialize方法,并且将控件的DOM元素返回
	// 在本方法中创建个div元素作为控件的容器,并将其添加到地图容器中
	ZoomControl.prototype.initialize = function(map){
	  // 创建一个DOM元素
	  var div = document.createElement("div");
	  // 添加文字说明
	  div.index = this.carID;
	  div.id = "btn_"+this.carID;
	  
	  if(this.carID >= 6)
			div.appendChild(document.createTextNode( "北邮" + (this.carID + 1) + "号车"));
	  else
	  div.appendChild(document.createTextNode( "北邮" + (this.carID+1) + "号车"));
	  // 设置样式
	  //pl
	  div.style.cursor = "pointer";
	  div.style.border = "1px solid gray";
	  div.style.backgroundColor = "00FF00";
	  div.style.color = "gray";
	  // 绑定事件,点击一次放大两级
	  div.onclick = function(e){
		  if(this.isPush){
			  animations[parseInt(this.index)].setAutoView(false);
			  animations[parseInt(this.index)].setBackgroundColor(false);
			  animations[parseInt(this.index)].hideInfoWindow();
			  
			  div.style.backgroundColor = "";
			  div.style.color = "gray";
			  div.style.border = "1px solid gray";
			  viewIndex = -1;
			  this.isPush = false;
		  }
		  else{			  
			  for (var i=0; i<aliveNum; i++){
				  var index = parseInt(carIDs[i]) - 1;	
				  if(index != div.index){
					document.getElementById("btn_"+index).style.backgroundColor = "";
					animations[parseInt(index)].setAutoView(false);
					animations[parseInt(index)].setBackgroundColor(false);
					animations[parseInt(index)].hideInfoWindow();
					document.getElementById("btn_"+index).style.color = "gray";
					document.getElementById("btn_"+index).style.border = "1px solid gray";
				  }
			  }
			  
			  animations[parseInt(this.index)].setAutoView(true);
			  animations[parseInt(this.index)].setCentor();
			  animations[parseInt(this.index)].setZindex(true);
			  animations[parseInt(this.index)].setBackgroundColor(true);
			  animations[parseInt(this.index)].showInfoWindow();
			  div.style.backgroundColor = "";
			  div.style.color = "yellow";
			  div.style.border = "1px solid yellow";
			  
			  viewIndex = this.index;			  
			  this.isPush = true;
		  }	  
	  }
	  
	  
	  // 添加DOM元素到地图中
	  map.getContainer().appendChild(div);
	  
	  // 将DOM元素返回
	  return div;
	}
	
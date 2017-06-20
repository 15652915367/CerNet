    $(function() {
        $('#container1').highcharts({
          chart: {
            type: 'gauge',
            backgroundColor: 'rgba(0,0,0,0)',
            plotBackgroundColor: null,
            plotBackgroundImage: null,
            plotBorderWidth: 0,
            plotShadow: false
          },
          pane: {
            startAngle: -150,
            endAngle: 150,
            background: [{
              backgroundColor: {
                linearGradient: {
                  x1: 0,
                  y1: 0,
                  x2: 0,
                  y2: 1
                },
                stops: [[0, '#272727'], [1, '#333']]
              },
              borderWidth: 0,
              outerRadius: '109%'
            },
            {
              backgroundColor: {
                linearGradient: {
                  x1: 0,
                  y1: 0,
                  x2: 0,
                  y2: 1
                },
                stops: [[0, '#333'], [1, '#272727']]
              },
              borderWidth: 1,
              outerRadius: '107%'
            },
            {
              backgroundColor: '#272727'
            },
            {
              backgroundColor: '#303030',
              borderWidth: 0,
              outerRadius: '105%',
              innerRadius: '103%'
            }]
          },
          // the value axis
          yAxis: {
            min: 0,
            max: 200,
            minorTickInterval: 'auto',
            minorTickWidth: 1,
            minorTickLength: 8,
            minorTickPosition: 'inside',
            minorTickColor: 'white',
            tickPixelInterval: 30,
            tickWidth: 2,
            tickPosition: 'inside',
            tickLength: 10,
            tickColor: 'white',
            labels: {
              step: 2,
              rotation: 'auto',
			  style: {
                  color: 'white'
              }
            },
            title: {
				y: 60,
              style: {
                  color: 'white',
                  fontSize: '15px'
              },
              text: 'km/h'
            },
            plotBands: [{
              from: 0,
              to: 120,
              color: '#55BF3B' // green
            },
            {
              from: 120,
              to: 160,
              color: '#DDDF0D' // yellow
            },
            {
              from: 160,
              to: 200,
              color: '#DF5353' // red
            }]
          },
		  plotOptions: {  
            gauge: {  
              dataLabels: {  
                enabled: true,  
                 y : 40,
                 color : 'white',
                 borderColor : '',
                 style: {
                  fontSize: '16px'
                 } 
              },  
              dial: {//仪表盘指针  
                radius: '80%',  
                rearLength: '20%',  
                backgroundColor: 'white',  
                borderColor: 'white',  
                borderWidth: 1,  
                baseWidth: 1.5,  
                topWidth: 1,  
                baseLength: '40%'  
              }  
            }  
          },
          series: [{
            name: '速度',
            data: [0],
            tooltip: {
              valueSuffix: ' km/h'
            }
          }]
        },
        // Add some life
        function(chart) {
          if (!chart.renderer.forExport) {
            setInterval(function() {
              var point = chart.series[0].points[0];
			  if(viewSpd != null)
				point.update( (parseInt(viewSpd)) );
            },
            200);
          }
        });
      });


    var ms = 'm/s'+'2'.sup();
    $(function() {
        $('#container2').highcharts({
          chart: {
            type: 'gauge',
            backgroundColor: 'rgba(0,0,0,0)',
            plotBackgroundColor: null,
            plotBackgroundImage: null,
            plotBorderWidth: 0,
            plotShadow: false
          },
          pane: {
            startAngle: -90,
            endAngle: 90,
            background: [{
              backgroundColor: {
                linearGradient: {
                  x1: 0,
                  y1: 0,
                  x2: 0,
                  y2: 1
                },
                stops: [[0, '#272727'], [1, '#333']]
              },
              borderWidth: 0,
              outerRadius: '109%'
            },
            {
              backgroundColor: {
                linearGradient: {
                  x1: 0,
                  y1: 0,
                  x2: 0,
                  y2: 1
                },
                stops: [[0, '#333'], [1, '#272727']]
              },
              borderWidth: 1,
              outerRadius: '107%'
            },
            {
              backgroundColor: '#272727'
            },
            {
              backgroundColor: '#303030',
              borderWidth: 0,
              outerRadius: '105%',
              innerRadius: '103%'
            }]
          },
          // the value axis
          yAxis: {
            min: -50,
            max: 50,
            minorTickInterval: 'auto',
            minorTickWidth: 1,
            minorTickLength: 8,
            minorTickPosition: 'inside',
            minorTickColor: 'white',
            tickPixelInterval: 60,
            tickWidth: 3,
            tickPosition: 'inside',
            tickLength: 10,
            tickColor: 'white',
            labels: {
              step: 1,
              rotation: 'auto',
			  style: {
                  color: 'white'
              }
            },
            title: {
			  y: 70,
              style: {
                  color: 'white',
                  fontSize: '15px'
              },
              text: ms
            },
            plotBands: [{
              from: -50,
              to: 0,
              color: '#55BF3B' // green
            },
            {
              from: 0,
              to: 25,
              color: '#DDDF0D' // yellow
            },
            {
              from: 25,
              to: 50,
              color: '#DF5353' // red
            }]
          },
		  plotOptions: {  
            gauge: {  
              dataLabels: {  
                enabled: true,  
                 y : 40,
                 color : 'white',
                 borderColor : '',
                 style: {
                  fontSize: '16px'
                 } 
              },  
              dial: {//仪表盘指针  
                radius: '80%',  
                rearLength: '20%',  
                backgroundColor: 'white',  
                borderColor: 'white',  
                borderWidth: 1,  
                baseWidth: 1.5,  
                topWidth: 1,  
                baseLength: '40%'  
              }  
            }  
          }, 
          series: [{
            name: '加速度',
            data: [0],
            tooltip: {
              valueSuffix: ms
            }
          }]
        },
        // Add some life
        function(chart) {
          if (!chart.renderer.forExport) {
            setInterval(function() {
              var point = chart.series[0].points[0];
			  if(viewAcc < 25 && viewAcc > -25)
				point.update(parseInt(viewAcc));
            },
            200);
          }
        });
      });

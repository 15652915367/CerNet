requirejs.config({
	baseUrl: 'js/lib',
	paths: {
		app: '../app',
		jquery: 'jquery-3.1.1.min',
		highchartsmore: 'highcharts-more',
		highchart: 'highcharts'
		mapwrapper: 'MapWrapper',
	}
});
requirejs(['jquery'],
function ($) {
	// body...
});
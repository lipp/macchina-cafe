var React = require('react');
var smoothie = require('smoothie');

var jet = require('node-jet');
var peer = new jet.Peer({
	url: 'ws://bunsen-2.local:11123'
});

var ainLines = [];
for(var i = 0; i < 6; ++i) {
	ainLines[i] = new smoothie.TimeSeries();
}

peer.fetch('ains', function(path, event, ainVals) {
	var now = new Date().getTime();

	ainVals.forEach(function(val, index) {
		ainLines[index].append(now, val);
	});
});

var Plot = React.createClass({
	initSmoothie() {
		this.chart = new smoothie.SmoothieChart({millisPerPixel: 5});
		var wrap = React.findDOMNode(this.refs.wrap);
		var canvas = React.findDOMNode(this.refs.smoothieChart);
		canvas.width = wrap.clientWidth;
		canvas.height = wrap.clientHeight;
		this.chart.streamTo(canvas);
		var chart = this.chart;
		this.ains.forEach(function(ain) {
			console.log(ain);
			chart.addTimeSeries(ainLines[ain]);
		});
	},

	componentDidMount() {
		this.ains = JSON.parse(this.props.ains);
		window.addEventListener('resize', this.handleResize);
		this.initSmoothie();
	},

	componentWillUnmount: function() {
    		window.removeEventListener('resize', this.handleResize);
  	},

	handleResize() {
		this.initSmoothie();
	},

	render() {
		return (
			<div className="Plot" ref="wrap">
				<canvas ref="smoothieChart"></canvas>
			</div>
		);
	}
});

module.exports = Plot;

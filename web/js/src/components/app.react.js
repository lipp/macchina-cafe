var React = require('react');

var Plot = require('./plot.react');

var App = React.createClass({
	render() {
		return (
			<div>
				<Plot ains="[0]"></Plot>
			</div>
		);
	}
});

module.exports = App;

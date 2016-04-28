const webpack = require('webpack'),
	path = require('path');

var config = {
	entry: {
		javascript: __dirname + '/src.js',
	},
	output: {
		filename: __dirname + '/bundle.js',
	},
	module: {
		loaders: [
			{
				test: /\.js$/,
				exclude: /node_modules/,
				loaders: ["babel-loader"],
			},
		],
	},
};

module.exports = config;

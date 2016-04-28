/*
 * jmlib-js - Portable JuggleMaster Library (JavaScript Version)
 * Version 2.0
 * (C) Per Johan Groland 2006-2016
 *
 * Based on JMLib 2.0, (C) Per Johan Groland and Gary Briggs
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib_js under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */


const gulp = require('gulp'),
	babel = require('gulp-babel'),
	eslint = require('gulp-eslint');

gulp.task('default', ['lint', 'build']);

gulp.task('lint', () => {
	gulp.src(['src/**/*.js', 'demo/src.js'])
		.pipe(eslint({
			extends: 'eslint:recommended',
			rules: {
				'no-var': 'error',
				'no-unused-vars': 'error',
				eqeqeq: ['error', 'allow-null'],
				semi: ['error', 'always'],
				//quotes: ['error', 'single'],
				'comma-dangle': ['error', 'always'],
			},
			envs: [
				'es6',
				'node',
			],
		}))
		.pipe(eslint.formatEach('stylish', process.stderr));
});

gulp.task('build', () => {
	return gulp.src('src/**/*.js')
		.pipe(babel({
			presets: ['es2015']
		}))
		.pipe(gulp.dest('dist'));
});

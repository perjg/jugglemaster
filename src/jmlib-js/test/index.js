'use strict';

var should = require('chai').should(),
	jmlib = require('../index'),
	validator = require('../dist/validator');

function test_true(site) {
	it(`${site} is a valid sitewap`, () => {
		validator.validateSite(site).should.equal(true);
	});
}

function test_false(site) {
	it(`${site} is an invalid sitewap`, () => {
		validator.validateSite(site).should.equal(false);
	});
}

describe('JMValidator Vanilla Siteswap tests', () => {
	test_true('3');
	test_true('633');
	test_true('534');
	test_false('644');
	test_false('6102');
	test_true('44453');
	test_false('6337');
	test_true('b');
	test_false('a1');
	test_true('b1');
	test_false('c1');
});

describe('JMValidator Multiplex Siteswap tests', () => {
	test_true('[34]1');
	test_true('[22]5[22]0[54]020');
	test_true('[234]57');
	test_true('[54]24522');
	test_false('4[45]3');
	test_true('[54]24');
});

describe('JMValidator Sync Siteswap tests', () => {
	test_true('(2,4x)(4x,2)');
	test_true('([66x],2)(2,[66x])');
	test_true('(6x,4)(4,6x)');
	test_false('(2x,4)(4x,2)');
	test_false('([2x4],4)([24x],2)');
	test_false('([2x4]4,4)([24x],2)');
	test_false('([2x4,4)([24x],2)');
	test_true('(4,4)(0,4x)(4,4)(4x,0)');
	test_true('([yxy],2)(2,[22])(2,[yxy])([22],2)');
	test_true('(2,[62])([22],6x)([62],2)(6x,[22])');
	test_true('(4,2x)(2x,4)');
	test_false('(7x,2)(2,5x)');
});

describe('JMValidator Invalid Syntax tests', () => {
	test_false('2[]3');
	test_false('2[4]3');
	test_false('2[345');
	test_false('2]345');
	test_false('(2x,4)(4x,2)7');
	test_false('8(2x,4)(4x,2)');
	test_false('(2x,4)6(4x,2)');
	test_false('(2x(4),(4x,2)');
	test_false('(7,4)(4,5)');
	test_false('spam spam spam!\'#!#?\'#?');
	test_false('');
	test_false(null);
	test_false('[543]3[45][27]2');
});

describe('JMValidator Misc tests', () => {

});

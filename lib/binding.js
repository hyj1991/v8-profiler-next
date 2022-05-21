'use strict';

const nodeVersion = process.versions.node;
const tags = nodeVersion.split('.');

let binding;

if (tags[0] < 12) {
  binding = require('../build/Release/profiler.node');
} else {
  const path = require('path');
  const binary = require('@xprofiler/node-pre-gyp');
  const bindingPath = binary.find(path.resolve(path.join(__dirname, '../package.json')));
  binding = require(bindingPath);
}

module.exports = binding;
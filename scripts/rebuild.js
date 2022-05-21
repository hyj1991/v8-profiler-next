'use strict';

const execCmd = require('./gyp');

execCmd('node-gyp rebuild', 'node-pre-gyp rebuild');

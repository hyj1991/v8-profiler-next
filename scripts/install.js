'use strict';

const execCmd = require('./gyp');

execCmd('node-gyp rebuild', 'node-pre-gyp install --fallback-to-build');

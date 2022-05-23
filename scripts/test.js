'use strict';

const cp = require('child_process');
const path = require('path');
const nodeVersionLessThan = require('../lib/utils').nodeVersionLessThan;

function exec(cmd) {
  cp.execSync(cmd, {
    cwd: path.join(__dirname, '../'),
    stdio: 'inherit', env: process.env
  });
}

if (nodeVersionLessThan(8)) {
  exec('npm run test-old');
} else {
  exec('npm run test-new');
}
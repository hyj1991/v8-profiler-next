'use strict';

const cp = require('child_process');
const path = require('path');
const nodeVersion = process.versions.node;

function exec(cmd) {
  cp.execSync(cmd, {
    cwd: path.join(__dirname, '../'),
    stdio: 'inherit', env: process.env
  });
}

const tags = nodeVersion.split('.');
if (tags[0] < 8) {
  exec('npm run test-old');
} else {
  exec('npm run test-new');
}
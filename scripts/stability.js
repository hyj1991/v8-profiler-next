'use strict';

const cp = require('child_process');
const path = require('path');

function exec(cmd) {
  cp.execSync(cmd, {
    cwd: path.join(__dirname, '../'),
    stdio: 'inherit', env: process.env,
    shell: '/bin/bash'
  });
}

function runOneVersion(version) {
  exec(`source ~/.tnvm/tnvm.sh && tnvm use ${version} && npm run build && npm run test`);
}

function main(versions, infinity = false) {
  do {
    for (const version of versions) {
      runOneVersion(version);
    }
  } while (infinity);
}

// test for 7u
const nodeVersions = [
  'node-v12.22.12',
  'node-v13.14.0',
  'node-v14.19.2',
  'node-v15.14.0',
  'node-v16.15.0',
  'node-v17.9.0',
];

main(nodeVersions, true);

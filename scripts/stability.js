'use strict';

const execCmd = require('./build').execCmd;
const versions = require('./7u');

function runOneVersion(version) {
  execCmd(`source ~/.tnvm/tnvm.sh && tnvm use ${version} && npm run build && npm run test`);
}

function main(versions, infinity = false) {
  do {
    for (const version of versions) {
      runOneVersion(version);
    }
  } while (infinity);
}

main(versions, true);

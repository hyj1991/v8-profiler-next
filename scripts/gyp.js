'use strict';

const path = require('path');
const fs = require('fs');
const execCmd = require('./build').execCmd;
const nodeVersionLessThan = require('../lib/utils').nodeVersionLessThan;

function writeConfig(config) {
  const options = JSON.stringify(config, null, 2);
  fs.writeFileSync(path.join(__dirname, '../config.gypi'), options, 'utf-8');
  console.log('Writing config.gypi:');
  console.log(options);
}

module.exports = function (oldCmd, newCmd) {
  const options = {
    variables: {
      action_after_build: 'false'
    }
  };

  if (nodeVersionLessThan(12)) {
    writeConfig(options);
    execCmd(oldCmd);
  } else {
    options.variables.action_after_build = 'true';
    writeConfig(options);
    execCmd(newCmd);
  }
};

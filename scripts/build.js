'use strict';

const os = require('os');
const fs = require('fs');
const cp = require('child_process');
const path = require('path');

const isWindows = os.platform() === 'win32';

function debug(message) {
  console.log(`\n\x1b[32;1m${message}\x1b[0m\n`);
}

// function error(message) {
//   console.log(`\n\x1b[31m${message}\x1b[0m\n`);
// }

const bashNotExists = !fs.existsSync('/bin/bash');

function execCmd(cmd) {
  debug(cmd);
  cp.execSync(cmd, {
    env: process.env,
    cwd: path.join(__dirname, '../'),
    stdio: 'inherit',
    shell: isWindows || bashNotExists ? undefined : '/bin/bash',
  });
}

function cleanReleaseDir() {
  const release = path.join(__dirname, '../release');
  if (!fs.existsSync(release)) {
    fs.mkdirSync(release);
  }
  const files = fs.readdirSync(release);
  for (const file of files) {
    const filePath = path.join(release, file);
    if (!fs.existsSync(filePath)) {
      continue;
    }
    fs.unlinkSync(filePath);
  }
}

exports = module.exports = versions => {
  cleanReleaseDir();

  for (const version of versions) {
    debug(`>>>>>>>> start build with ${version}`);
    const tnvmPath = path.join(os.homedir(), '.tnvm/tnvm.sh');
    let npmBin = 'npm';
    let change = `source ${tnvmPath} && tnvm use ${version}`;
    const nvmNodeVersion = /^node-v(.*)$/.exec(version)[1];
    if (isWindows) {
      npmBin = path.join(os.tmpdir(), '../../', `Roaming\\nvm\\v${nvmNodeVersion}\\npm.cmd`);
      change = `nvm use ${nvmNodeVersion}`;
    }

    const install = 'npm install --no-audit';
    const build = `${npmBin} run dep`;
    const pack = 'npx node-pre-gyp package && npx node-pre-gyp testpackage';
    const copy = `${npmBin} run copy`;
    execCmd(`${change} && ${build} && ${install} && ${pack} && ${copy}`);
    debug(`<<<<<<<< build with ${version} done.`);
  }

  debug('all build tasks done.');
};

exports.execCmd = execCmd;
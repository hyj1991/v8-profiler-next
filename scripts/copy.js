'use strict';

const path = require('path');
const fs = require('fs');
const versioning = require('@xprofiler/node-pre-gyp/lib/util/versioning.js');
const packagePath = versioning.evaluate(require('../package.json')).staged_tarball;

function copy() {
  const release = path.join(__dirname, '../release');
  if (!fs.existsSync(release)) {
    fs.mkdirSync(release);
  }

  if (!fs.existsSync(packagePath)) {
    return;
  }

  const filename = path.basename(packagePath);
  const target = path.join(release, filename);
  fs.copyFileSync(packagePath, target);
}

copy();

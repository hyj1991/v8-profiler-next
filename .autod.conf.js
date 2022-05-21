'use strict';

module.exports = {
  write: true,
  prefix: '^',
  test: [
    'test',
  ],
  dep: [
    'nan',
    '@xprofiler/node-pre-gyp',
  ],
  devdep: [
    'autod',
    'chai',
    'clang-format',
    'eslint',
    'mocha',
  ],
  exclude: [
    './build',
    './scripts',
    './test/fixtures',
    './demo.js',
    './lib/worker_threads.js'
  ],
  semver: [
    'mocha@5'
  ]
};

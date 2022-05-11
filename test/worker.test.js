'use strict';

const { canIUseWorkerThreads } = require('../lib/worker_threads');
const cp = require('child_process');
const path = require('path');
const workerFile = path.join(__dirname, 'fixtures/worker.js');
const assert = require('assert');

function fork(filepath, options = {}) {
  const proc = cp.fork(filepath, Object.assign({
    stdio: ['ignore', 'pipe', 'pipe', 'ipc'],
  }, options));
  proc.stdout.setEncoding('utf8');
  proc.stderr.setEncoding('utf8');
  return proc;
}

function getOutput(proc) {
  return new Promise(resolve => {
    let stdout = '';
    let stderr = '';
    proc.stdout.on('data', chunk => {
      stdout += chunk;
      console.log('child:', chunk);
    });
    proc.stderr.on('data', chunk => {
      stderr += chunk;
      console.error('child:', chunk);
    });

    proc.on('exit', (code, signal) => {
      if (code !== 0) {
        console.log('process exited with non-zero code: pid(%d), code(%s), signal(%s)', proc.pid, code, signal);
        console.log('stdout:\n', stdout);
        console.log('');
        console.log('stderr:\n', stderr);
        resolve({ stderr, stdout });
      }
    });
  });
}



(canIUseWorkerThreads ? describe : describe.skip)('worker_threads', function () {
  describe('cpu profiling', function () {
    let output;
    before(async function () {
      const proc = fork(workerFile);
      output = await getOutput(proc);
    });

    it('output should have content', function () {
      console.log(12333, output);
      assert(output.stdout);
      assert(!output.stderr);
    })
  });
});
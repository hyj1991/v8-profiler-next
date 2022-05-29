'use strict';

const fs = require('fs');
const cp = require('child_process');
const path = require('path');
const { canIUseWorkerThreads } = require('../lib/worker_threads');
const workerFile = path.join(__dirname, 'fixtures/worker.js');
const assert = require('assert');

const profile = {
  cpuprofile: {
    main: path.join(__dirname, 'fixtures/main.cpuprofile'),
    worker_threads: path.join(__dirname, 'fixtures/worker_threads.cpuprofile'),
  },
  heapprofile: {
    main: path.join(__dirname, 'fixtures/main.heapprofile'),
    worker_threads: path.join(__dirname, 'fixtures/worker_threads.heapprofile'),
  },
  heapsnapshot: {
    main: path.join(__dirname, 'fixtures/main.heapsnapshot'),
    worker_threads: path.join(__dirname, 'fixtures/worker_threads.heapsnapshot'),
  }
};

function cleanFile(file) {
  if (fs.existsSync(file)) {
    fs.unlinkSync(file);
  }
}

function cleanProfile() {
  cleanFile(profile.cpuprofile.main);
  cleanFile(profile.cpuprofile.worker_threads);
  cleanFile(profile.heapprofile.main);
  cleanFile(profile.heapprofile.worker_threads);
  cleanFile(profile.heapsnapshot.main);
  cleanFile(profile.heapsnapshot.worker_threads);
}

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
    });
    proc.stderr.on('data', chunk => {
      stderr += chunk;
    });

    proc.on('exit', (code, signal) => {
      if (code !== 0) {
        console.log('process exited with non-zero code: pid(%d), code(%s), signal(%s)', proc.pid, code, signal);
        console.log('stdout:\n', stdout);
        console.log('');
        console.log('stderr:\n', stderr);
      }
      resolve({ code, signal, stdout, stderr });
    });
  });
}



(canIUseWorkerThreads ? describe : describe.skip)('worker_threads', function () {
  describe('cpu profiling & heap profiling & take heapsnapshot', function () {
    let output;
    before(async function () {
      cleanProfile();

      const proc = fork(workerFile);
      output = await getOutput(proc);
    });

    after(async function () {
      cleanProfile();
    });

    it('worker.js exit succeed', function () {
      console.log(`worker.js exit info: ${JSON.stringify(output)}`);
      assert(output.code === 0);
      assert(output.signal === null);
    });

    it('worker_threads should exit succeed', function () {
      assert(!output.stderr);
      console.log(output.stdout);
      const stdout = output.stdout.split('\n').filter(line => line && !line.match(/\[thread \d+/)).join('');
      const { code: workerExitCode } = JSON.parse(stdout);
      assert(workerExitCode === 0);
    });

    it('main create cpuprofile succeed', function () {
      assert(fs.existsSync(profile.cpuprofile.main));
    });

    it('worker_threads create cpuprofile succeed', function () {
      assert(fs.existsSync(profile.cpuprofile.worker_threads));
    });

    it('main create heapprofile succeed', function () {
      assert(fs.existsSync(profile.heapprofile.main));
    });

    it('worker_threads create heapprofile succeed', function () {
      assert(fs.existsSync(profile.heapprofile.worker_threads));
    });

    it('main create heapsnapshot succeed', function () {
      assert(fs.existsSync(profile.heapsnapshot.main));
    });

    it('worker_threads create heapsnapshot succeed', function () {
      assert(fs.existsSync(profile.heapsnapshot.worker_threads));
    });
  });
});
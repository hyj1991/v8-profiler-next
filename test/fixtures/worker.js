'use strict';

const { canIUseWorkerThreads } = require('../../lib/worker_threads');
const fs = require('fs');
const path = require('path');
const v8Profiler = require(path.join(__dirname, '../../'));
v8Profiler.setGenerateType(1);

function createProfile(filename, func, ...args) {
  const profile = v8Profiler[func](...args);
  const file = path.join(__dirname, filename);
  fs.existsSync(file) && fs.unlinkSync(file);
  fs.writeFileSync(file, JSON.stringify(profile));
}

function main() {
  if (!canIUseWorkerThreads) {
    return;
  }

  const workerThreads = require('worker_threads');
  if (workerThreads.isMainThread) {
    const profiles = [];
    const w = new workerThreads.Worker(__filename, {
      env: process.env,
    });
    v8Profiler.startProfiling('main', true);
    v8Profiler.startSamplingHeapProfiling();
    w.once('exit', code => {
      // create cpu profile in main thread
      createProfile('main.cpuprofile', 'stopProfiling', 'main');

      // create heap profile in main thread
      createProfile('main.heapprofile', 'stopSamplingHeapProfiling');

      console.log(JSON.stringify({ code }));
    });
  } else {
    v8Profiler.startProfiling('worker_threads', true);
    v8Profiler.startSamplingHeapProfiling();
    const start = Date.now();
    const array = [];
    while (Date.now() - start < 2000) { array.push(new Array(1e3).fill('*')); }

    // create cpu profile in worker_threads
    createProfile('worker_threads.cpuprofile', 'stopProfiling', 'worker_threads');

    // create heap profile in worker_threads
    createProfile('worker_threads.heapprofile', 'stopSamplingHeapProfiling');
  }
}

main();
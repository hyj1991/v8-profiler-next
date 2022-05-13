'use strict';

const { canIUseWorkerThreads } = require('../../lib/worker_threads');
const fs = require('fs');
const path = require('path');
const v8Profiler = require(path.join(__dirname, '../../'));
v8Profiler.setGenerateType(1);

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
    w.once('exit', code => {
      // create cpu profile in main thread
      const profile = v8Profiler.stopProfiling('main');
      const mainProfile = path.join(__dirname, 'main.cpuprofile');
      fs.existsSync(mainProfile) && fs.unlinkSync(mainProfile);
      fs.writeFileSync(mainProfile, JSON.stringify(profile));
      console.log(JSON.stringify({ code }));
    });
  } else {
    v8Profiler.startProfiling('worker_threads', true);
    // create cpu profile in worker_threads
    const start = Date.now();
    while (Date.now() - start < 2000) { }
    const profile = v8Profiler.stopProfiling('worker_threads');
    const workerProfile = path.join(__dirname, 'worker_threads.cpuprofile');
    fs.existsSync(workerProfile) && fs.unlinkSync(workerProfile);
    fs.writeFileSync(workerProfile, JSON.stringify(profile));
  }
}

main();
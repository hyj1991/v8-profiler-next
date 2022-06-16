'use strict';

const { canIUseWorkerThreads } = require('../../lib/worker_threads');
const fs = require('fs');
const path = require('path');
const v8Profiler = require('../../');
v8Profiler.setGenerateType(1);

function createProfile(filename, func, ...args) {
  const profile = v8Profiler[func](...args);
  const file = path.join(__dirname, filename);
  fs.existsSync(file) && fs.unlinkSync(file);
  fs.writeFileSync(file, JSON.stringify(profile));
}

function createSnapshot(filename) {
  return new Promise(resolve => {
    const snapshot = v8Profiler.takeSnapshot();
    const file = path.join(__dirname, filename);
    const transform = snapshot.export();
    transform.pipe(fs.createWriteStream(file));
    transform.on('finish', () => snapshot.delete() && resolve());
  });
}

async function main() {
  if (!canIUseWorkerThreads) {
    return;
  }

  const globalThis = global;

  const workerThreads = require('worker_threads');
  if (workerThreads.isMainThread) {
    const w = new workerThreads.Worker(__filename, {
      env: process.env,
    });

    // create heapsnapshot in main thread
    await createSnapshot('main.heapsnapshot');

    v8Profiler.startProfiling('main', true);
    v8Profiler.collectSample();
    v8Profiler.startSamplingHeapProfiling();
    w.once('exit', code => {
      // create cpu profile in main thread
      createProfile('main.cpuprofile', 'stopProfiling', 'main');

      // create heap profile in main thread
      createProfile('main.heapprofile', 'stopSamplingHeapProfiling');

      console.log(JSON.stringify({ code }));
    });
  } else {
    v8Profiler.setProfilesCleanupLimit(1000);
    v8Profiler.startProfiling('worker_threads', true, 1);
    v8Profiler.startSamplingHeapProfiling();
    const start = Date.now();
    const array = globalThis.worker_array = [];
    while (Date.now() - start < 2000) {
      array.push(new Array(5e5).fill('*').join(''));
      v8Profiler.collectSample();
      await new Promise(resolve => setTimeout(resolve, 100));
    }

    // create cpu profile in worker_threads
    createProfile('worker_threads.cpuprofile', 'stopProfiling', 'worker_threads');

    // create heap profile in worker_threads
    createProfile('worker_threads.heapprofile', 'stopSamplingHeapProfiling');

    // create heapsnapshot in worker_threads
    await createSnapshot('worker_threads.heapsnapshot');
  }
}

main();
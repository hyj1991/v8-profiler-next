'use strict';
const fs = require('fs');
const path = require('path');
const cp = require('child_process');
const v8Profiler = require('../dispatch');
const version = process.version;
const title = '';

let totalEvents = 0;
let count = 0;

cp.execSync('rm -rf *.heapsnapshot && rm -rf *.cpuprofile && rm -rf *.heapprofile',
  { cwd: path.join(__dirname) });

console.log(`================ <node-${version}> test files (before) ================`);
const data = cp.execSync('ls -lah', { cwd: path.join(__dirname) });
console.log(data.toString());
console.log('============================================\n\n');

// set a leak array
const arraytest = [];
const interval = setInterval(() => {
  arraytest.push(new Array(1e2).fill('*').join());
  if (count === totalEvents) {
    console.log(`================ <node-${version}> test files (after) ================`);
    const data = cp.execSync('ls -lah', { cwd: path.join(__dirname) });
    console.log(data.toString());
    console.log('============================================');
    clearInterval(interval);
  }
}, 20);

// ex. 1 mins cpu profile
v8Profiler.startProfiling(title, true);
totalEvents++;
setTimeout(() => {
  const profiler = v8Profiler.stopProfiling(title);
  profiler.delete();
  console.log('cpu profiling done...\n');
  fs.writeFileSync(path.join(__dirname,
    `cpu-node_${version}.cpuprofile`), JSON.stringify(profiler));
  count++;
}, 60 * 1000);


const snapshot1 = v8Profiler.takeSnapshot();
totalEvents++;
// 1. not as stream
snapshot1.export(function (error, result) {
  if (error) {
    console.error(error);
    return;
  }
  console.log('snapshot not stream done...\n');
  fs.writeFileSync(path.join(__dirname,
    `snapshot-node_${version}-total.heapsnapshot`), result);
  snapshot1.delete();
  count++;
});
// 2. as stream
const snapshot2 = v8Profiler.takeSnapshot();
totalEvents++;
const transform = snapshot2.export();
transform.pipe(fs.createWriteStream(path.join(__dirname,
  `snapshot-node_${version}-stream.heapsnapshot`)));
transform.on('finish', () => {
  console.log('snapshot stream done...\n');
  snapshot2.delete();
  count++;
});


// start 1min sampling profile
v8Profiler.startSamplingHeapProfiling();
totalEvents++;
setTimeout(() => {
  // stop and get allocation profile
  const profile = v8Profiler.stopSamplingHeapProfiling();
  console.log('sampling heap profiling done...\n');
  // upload heap-node_vxxx.heapprofile into chrome dev tools -> Memory -> ALLOCATION PRODILES
  require('fs').writeFileSync(path.join(__dirname,
    `heap-node_${version}.heapprofile`), JSON.stringify(profile));
  count++;
}, 60 * 1000);
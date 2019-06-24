# v8-profiler-next

[![npm version](https://badge.fury.io/js/v8-profiler-next.svg)](https://badge.fury.io/js/v8-profiler-next)
[![Package Quality](http://npm.packagequality.com/shield/v8-profiler-next.svg)](http://packagequality.com/#?package=v8-profiler-next)
[![npm](https://img.shields.io/npm/dt/v8-profiler-next.svg)](https://www.npmjs.com/package/v8-profiler-next)
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](LICENSE)

## I. Quick Start

Support Node v4.x ~ v12.x

### take cpu profile

```js
'use strict';
const v8Profiler = require('v8-profiler-next');
const title = '';
// ex. 5 mins cpu profile
v8Profiler.startProfiling(title, true);
setTimeout(() => {
	const profiler = v8Profiler.stopProfiling(title);
	profiler.delete();
	console.log(profiler);
}, 5 * 60 * 1000);
```

### take heapsnapshot

```js
'use strict';
const v8Profiler = require('v8-profiler-next');
const snapshot = v8Profiler.takeSnapshot();
// 1. not as stream
snapshot.export(function (error, result) {
	if (error){
		console.error(error);
		return;
	}
	console.log(result);
	snapshot.delete();
});
// 2. as stream
const transform = snapshot.export();
transform.pipe(process.stdout);
transform.on('finish', snapshot.delete.bind(snapshot));
```

### take allocation profile

**Attention:** If node version <= v8.x, please use sampling heap profiling alone without cpu profiling or taking snapshot.

```js
'use strict';
const v8Profiler = require('v8-profiler-next');
// set a leak array
const arraytest = [];
setInterval(() => {
  arraytest.push(new Array(1e2).fill('*').join());
}, 20);
// start 1min sampling profile
v8Profiler.startSamplingHeapProfiling();
setTimeout(() => {
	// stop and get allocation profile
	const profile = v8Profiler.stopSamplingHeapProfiling();
	// upload shf.heapprofile into chrome dev tools -> Memory -> ALLOCATION PRODILES
  require('fs').writeFileSync('./shf.heapprofile', JSON.stringify(profile));
	console.log(profile);
}, 60 * 1000);
```

## II. License

[MIT License](LICENSE)

Copyright (c) 2018 team of [v8-profiler](github.com/node-inspector/v8-profiler), hyj1991
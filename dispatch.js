'use strict';

const Stream = require('stream').Stream;
const inherits = require('util').inherits;
const binding = require('./lib/binding');
const workerThreads = require('./lib/worker_threads');

function global_check() {
  const version = process.versions.node.split('.').map(item => Number(item));
  const greater_than_12_15_0 = version[0] > 12 || (version[0] === 12 && version[1] > 15);
  return { greater_than_12_15_0 };
}

binding.setup({
  debug: !!process.env.V8_PROFILER_DEBUG,
  thread_id: workerThreads.threadId
}, global_check);

function nodes(snapshot) {
  let n = snapshot.nodesCount, i, nodes = [];
  for (i = 0; i < n; i++) {
    nodes[i] = snapshot.getNode(i);
  }
  return nodes;
}

function nodesHist(snapshot) {
  let objects = {};
  nodes(snapshot).forEach(function (node) {
    let key = node.type === 'Object' ? node.name : node.type;
    objects[key] = objects[node.name] || 0;
    objects[key]++;
  });
  return objects;
}

function Snapshot() { }

Snapshot.prototype.getHeader = function () {
  return {
    typeId: this.typeId,
    uid: this.uid,
    title: this.title
  };
};

/**
* @param {Snapshot} other
* @returns {Object}
*/
Snapshot.prototype.compare = function (other) {
  let selfHist = nodesHist(this),
    otherHist = nodesHist(other),
    keys = Object.keys(selfHist).concat(Object.keys(otherHist)),
    diff = {};

  keys.forEach(function (key) {
    if (key in diff) { return; }

    let selfCount = selfHist[key] || 0,
      otherCount = otherHist[key] || 0;

    diff[key] = otherCount - selfCount;
  });

  return diff;
};

function ExportStream() {
  Stream.Transform.call(this);
  this._transform = function noTransform(chunk, encoding, done) {
    done(null, chunk);
  };
}
inherits(ExportStream, Stream.Transform);

/**
* @param {Stream.Writable|function} dataReceiver
* @returns {Stream|undefined}
*/
Snapshot.prototype.export = function (dataReceiver) {
  dataReceiver = dataReceiver || new ExportStream();

  let toStream = dataReceiver instanceof Stream,
    chunks = toStream ? null : [];

  function onChunk(chunk, len) {
    if (toStream) { dataReceiver.write(chunk); }
    else { chunks.push(chunk); }
  }

  function onDone() {
    if (toStream) { dataReceiver.end(); }
    else { dataReceiver(null, chunks.join('')); }
  }

  this.serialize(onChunk, onDone);

  return toStream ? dataReceiver : undefined;
};

function CpuProfile() { }

CpuProfile.prototype.getHeader = function () {
  return {
    typeId: this.typeId,
    uid: this.uid,
    title: this.title
  };
};

CpuProfile.prototype.export = function (dataReceiver) {
  dataReceiver = dataReceiver || new ExportStream();

  let toStream = dataReceiver instanceof Stream;
  let error, result;

  try {
    result = JSON.stringify(this);
  } catch (err) {
    error = err;
  }

  process.nextTick(function () {
    if (toStream) {
      if (error) {
        dataReceiver.emit('error', error);
      }

      dataReceiver.end(result);
    } else {
      dataReceiver(error, result);
    }
  });

  return toStream ? dataReceiver : undefined;
};

function SamplingHeapProfile() { }

SamplingHeapProfile.prototype.export = function (dataReceiver) {
  dataReceiver = dataReceiver || new ExportStream();

  let toStream = dataReceiver instanceof Stream;
  let error, result;

  try {
    result = JSON.stringify(this);
  } catch (err) {
    error = err;
  }

  process.nextTick(function () {
    if (toStream) {
      if (error) {
        dataReceiver.emit('error', error);
      }

      dataReceiver.end(result);
    } else {
      dataReceiver(error, result);
    }
  });

  return toStream ? dataReceiver : undefined;
};

let startTime, endTime;
let activeProfiles = [];

let profiler = {
  /*HEAP PROFILER API*/

  get snapshots() { return binding.heap.snapshots; },

  takeSnapshot: function (name, control) {
    if (typeof name === 'function') {
      control = name;
      name = '';
    }

    if (typeof control !== 'function') {
      control = function noop() { };
    }

    name = name ? '' + name : '';

    let snapshot = binding.heap.takeSnapshot(name, control);
    snapshot.__proto__ = Snapshot.prototype;
    snapshot.title = name ? name : snapshot.title;
    return snapshot;
  },

  deleteAllSnapshots: function () {
    Object.keys(binding.heap.snapshots).forEach(function (key) {
      binding.heap.snapshots[key].delete();
    });
  },

  startTrackingHeapObjects: function () {
    binding.heap.startTrackingHeapObjects();
  },

  stopTrackingHeapObjects: function () {
    binding.heap.stopTrackingHeapObjects();
  },

  getHeapStats: function (iterator, callback) {
    if (typeof iterator !== 'function') { iterator = function noop() { }; }

    if (typeof callback !== 'function') { callback = function noop() { }; }

    return binding.heap.getHeapStats(iterator, callback);
  },

  getObjectByHeapObjectId: function (id) {
    id = parseInt(id, 10);
    if (isNaN(id)) { return; }

    return binding.heap.getObjectByHeapObjectId(id);
  },

  getHeapObjectId: function (value) {
    if (!arguments.length) { return; }
    return binding.heap.getHeapObjectId(value);
  },

  /*CPU PROFILER API*/

  get profiles() { return binding.cpu.profiles; },

  setGenerateType: function (type) {
    const types = [0, 1];
    if (types.indexOf(type) > 0) {
      binding.cpu.setGenerateType(type);
    } else {
      console.error(`type should in [${types.join(', ')}], got ${type}.`);
    }
  },

  setProfilesCleanupLimit: function (limit) {
    if (!isNaN(limit)) {
      binding.cpu.setProfilesCleanupLimit(Number(limit));
    } else {
      console.error(`limit should be number.`);
    }
  },

  startProfiling: function (name, recsamples, mode) {
    if (activeProfiles.length === 0 && typeof process._startProfilerIdleNotifier === 'function') {
      process._startProfilerIdleNotifier();
    }

    if (typeof name === 'boolean') {
      recsamples = name;
      name = '';
    }

    if ([0, 1].indexOf(mode) === -1) {
      mode = 0;
    }

    recsamples = recsamples === undefined ? true : Boolean(recsamples);
    name = name && '' + name || '';

    if (activeProfiles.indexOf(name) < 0) { activeProfiles.push(name); }

    startTime = Date.now();
    binding.cpu.startProfiling(name, recsamples, mode);
  },

  stopProfiling: function (name) {
    let index = activeProfiles.indexOf(name);
    if (name && index < 0) { return; }

    let profile = binding.cpu.stopProfiling(name);
    endTime = Date.now();
    profile.__proto__ = CpuProfile.prototype;
    if (!profile.startTime) { profile.startTime = startTime; }
    if (!profile.endTime) { profile.endTime = endTime; }

    if (name) { activeProfiles.splice(index, 1); }
    else { activeProfiles.length = activeProfiles.length - 1; }

    if (activeProfiles.length === 0 && typeof process._stopProfilerIdleNotifier === 'function') {
      process._stopProfilerIdleNotifier();
    }

    return profile;
  },

  setSamplingInterval: function (num) {
    if (activeProfiles.length) {
      throw new Error('setSamplingInterval must be called when there are no profiles being recorded.');
    }

    num = parseInt(num, 10) || 1000;
    binding.cpu.setSamplingInterval(num);
  },

  deleteAllProfiles: function () {
    Object.keys(binding.cpu.profiles).forEach(function (key) {
      binding.cpu.profiles[key].delete();
    });
  },

  collectSample: function () {
    if (process.versions.modules >= 48) {
      binding.cpu.collectSample();
    }
  },

  /*SAMPLING HEAP PROFILER API*/

  startSamplingHeapProfiling: function (interval, depth) {
    if (process.versions.modules < 48) { throw new Error('Sampling heap profiler needs node version >= node-v6.0.0!'); }
    if (arguments.length === 2) { binding.samplingHeap.startSamplingHeapProfiling(interval, depth); }
    else { binding.samplingHeap.startSamplingHeapProfiling(); }
  },

  stopSamplingHeapProfiling: function () {
    if (process.versions.modules < 48) { throw new Error('Sampling heap profiler needs node version >= node-v6.0.0!'); }
    let profile = binding.samplingHeap.stopSamplingHeapProfiling();
    profile.__proto__ = SamplingHeapProfile.prototype;
    return profile;
  }
};

module.exports = profiler;
process.profiler = profiler;

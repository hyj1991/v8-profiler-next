'use strict';

let canIUseWorkerThreads = false;
let isMainThread = true;
let threadId = 0;

try {
  const workerThreads = require('worker_threads');
  isMainThread = workerThreads.isMainThread;
  threadId = workerThreads.threadId;
  canIUseWorkerThreads = true;
} catch (err) {
  err;
}

module.exports = {
  isMainThread,
  threadId,
  canIUseWorkerThreads,
};

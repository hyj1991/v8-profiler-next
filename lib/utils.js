'use strict';

exports.nodeVersionLessThan = function (version) {
  const nodeVersion = process.versions.node;
  const tags = nodeVersion.split('.');
  return Number(tags[0]) < Number(version);
};
'use strict';

const binding = require('../lib/binding');
const expect = require('chai').expect;

const NODE_V_010 = /^v0\.10\.\d+$/.test(process.version);

describe('binding', function () {
  describe('Profiler container', function () {
    it('has expected structure', function () {
      const properties = ['cpu', 'heap'];

      properties.forEach(function (prop) {
        expect(binding).to.have.property(prop);
      });
    });
  });

  describe('CPU', function () {
    const cpu = binding.cpu;

    function deleteAllProfiles() {
      cpu.profiles.slice().forEach(function (profile) {
        profile.delete();
      });
    }

    after(deleteAllProfiles);

    describe('Profiler', function () {

      it('has expected structure', function () {
        const properties = ['startProfiling', 'stopProfiling', 'profiles'];
        if (process.versions.modules >= 48) {
          properties.push('collectSample');
        }

        properties.forEach(function (prop) {
          expect(cpu).to.have.property(prop);
        });
      });
    });

    describe('Profile', function () {
      afterEach(() => cpu.setGenerateType(0));

      it('has expected structure', function () {
        cpu.startProfiling('', true);
        const profile = cpu.stopProfiling();
        const properties = NODE_V_010 ?
          ['delete', 'typeId', 'uid', 'title', 'head'] :
          ['delete', 'typeId', 'uid', 'title', 'head', 'startTime', 'endTime', 'samples', 'timestamps'];
        properties.forEach(function (prop) {
          expect(profile).to.have.property(prop);
        });
      });

      it('has expected structure with generateType 1', function () {
        cpu.setGenerateType(1);
        cpu.setProfilesCleanupLimit([]);
        cpu.startProfiling('', true, 1);
        const profile = cpu.stopProfiling();
        const properties = NODE_V_010 ?
          ['delete', 'typeId', 'uid', 'title', 'head'] :
          ['delete', 'typeId', 'uid', 'title', 'nodes', 'startTime', 'endTime', 'samples', 'timeDeltas'];
        properties.forEach(function (prop) {
          expect(profile).to.have.property(prop);
        });
      });

      it('should delete itself from profiler cache', function () {
        cpu.startProfiling('', true);
        const profile = cpu.stopProfiling();
        const oldProfilesLength = Object.keys(cpu.profiles).length;
        profile.delete();
        expect(oldProfilesLength - Object.keys(cpu.profiles).length).to.equal(1);
      });
    });

    describe('Profile Node', function () {
      it('has expected structure', function () {
        cpu.startProfiling('P');
        const profile = cpu.stopProfiling();
        const mainProps = ['functionName', 'url', 'lineNumber', 'children',
          'bailoutReason', 'id', 'hitCount'];
        const extendedProps = NODE_V_010 ? [] : ['scriptId'];
        const properties = mainProps.concat(extendedProps);

        properties.forEach(function (prop) {
          expect(profile.head).to.have.property(prop);
        });
      });
    });

    describe('Profile Nodes', function () {
      it('has expected structure', function () {
        cpu.setGenerateType(1);
        cpu.setProfilesCleanupLimit(null);
        cpu.startProfiling('P');
        const profile = cpu.stopProfiling('P');
        const mainProps = ['id', 'hitCount', 'callFrame', 'children'];
        const callFrameMainProps = ['functionName', 'scriptId', 'lineNumber', 'columnNumber',
          'bailoutReason', 'url'];
        const extendedProps = NODE_V_010 ? [] : ['scriptId'];
        const callFrameProps = callFrameMainProps.concat(extendedProps);

        mainProps.forEach(function (prop) {
          for (const node of profile.nodes) {
            expect(node).to.have.property(prop);
          }
        });

        callFrameProps.forEach(function (prop) {
          for (const node of profile.nodes) {
            expect(node.callFrame).to.have.property(prop);
          }
        });
      });
    });
  });

  describe('HEAP', function () {
    const heap = binding.heap;

    function deleteAllSnapshots() {
      Object.keys(binding.heap.snapshots).forEach(function (key) {
        binding.heap.snapshots[key].delete();
      });
    }

    after(deleteAllSnapshots);

    describe('Profiler', function () {
      it('has expected structure', function () {
        const properties = [
          'takeSnapshot',
          'startTrackingHeapObjects',
          'stopTrackingHeapObjects',
          'getHeapStats',
          'snapshots'
        ];

        properties.forEach(function (prop) {
          expect(heap).to.have.property(prop);
        });
      });
    });

    describe('Snapshot', function () {
      it('has expected structure', function () {
        const snapshot = heap.takeSnapshot('', function () { });
        const properties = [
          'delete', 'serialize', 'getNode', 'root',
          'typeId', 'uid', 'title', 'nodesCount', 'maxSnapshotJSObjectId'
        ];

        properties.forEach(function (prop) {
          expect(snapshot).to.have.property(prop);
        });
      });
    });
  });
});

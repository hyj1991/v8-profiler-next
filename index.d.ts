import { Stream } from "stream";

/**
 * 
 * @param type 0 for "flat" profile, 1 for "tree" profile
 */
export function setGenerateType(type: 0 | 1): void;

/**
 * 
 * @param limit Maximum number of profiles to keep in memory
 */
export function setProfilesCleanupLimit(limit: number): void;

/**
 * 
 * @param num Sampling interval in bytes. Default is 512 * 1024.
 */
export function setSamplingInterval(num: number): void;

/**
 * Collects a sample.
 */
export function collectSample(): void;

/**
 * Starts the profiler with a new profile.
 * @param name Name for the profile. "undefined" if not defined.
 * @param recsamples Is true by default.
 */
export function startProfiling(recsamples?: boolean): void;
export function startProfiling(name?: string, recsamples?: boolean, mode?: 0 | 1): void;

/**
 * Stops the profiler for a specific profile.
 * @param name Name of the profile. "undefined" if not defined.
 */
export function stopProfiling(name?: string): CpuProfile;

/**
 * Deletes all profiles.
 */
export function deleteAllProfiles(): void;

/**
 * Deletes all snapshots.
 */
export function deleteAllSnapshots(): void;

/**
 * Starts the sampling profiler with a new profile.
 */
export function startSamplingHeapProfiling(): void;
export function startSamplingHeapProfiling(
  interval: number,
  depth: number
): void;

/**
 * Stops the sampling profiler for a specific profile.
 */
export function stopSamplingHeapProfiling(): SamplingHeapProfile;

export function getHeapStats(
  iterator: Function | undefined,
  callback: Function | undefined
): number;

/**
 * 
 * @param id Heap object id
 */
export function getObjectByHeapObjectId(id: number): Object;

/**
 * Starts tracking heap objects.
 */
export function startTrackingHeapObjects(): void;

/**
 * Stops tracking heap objects.
 */
export function stopTrackingHeapObjects(): void;

/**
 * 
 * @param value Object to get the heap object id from
 */
export function getHeapObjectId(value: Object): number;

/**
 * Takes a snapshot.
 * @param name Name of the snapshot
 * @param control Function to execute
 */
export function takeSnapshot(control?: Function): Snapshot;
export function takeSnapshot(name?: string, control?: Function): Snapshot;

export let snapshots: { [key: string]: Snapshot };
export let profiles: Profile[];

export class Profile {
  /**
   * Exports data of the profile.
   * @param dataReceiver Can be a stream or callback. If not defined or a stream, returns a new stream.
   */
  export(dataReceiver?: Stream): ExportStream;
  export(dataReceiver: DataReceiver): void;
}

export class SamplingHeapProfile extends Profile {
  head: SamplingHeapProfileNode;
}

export class CpuProfile extends Profile {
  getHeader(): Header;
  delete(): void;
  endTime: Date;
  startTime: Date;
  head: CpuProfileNode;
  samples: number[];
  timestamps: number[];
  typeId: string;
  uid: string;
  title: string;
}

/**
 * @param error Error if the CpuProfiler encountered an error
 * @param result Result as stringified JSON object
 */
type DataReceiver = (
  error: Error | undefined,
  result: string | undefined
) => void;

export class ExportStream extends Stream.Transform { }

export class Snapshot extends Profile {
  getHeader(): Header;
  compare(other: Snapshot): Object;
  getNode(index: number): SnapshotNode;
  getNodeById(id: number): SnapshotNode; // todo: NOT WORKING
  serialize(iterator: Function, callback: Function): void;
  delete(): Object;
  root: SnapshotNode;
  typeId: string;
  title: string;
  uid: number;
  nodesCount: number;
  maxSnapshotJSObjectId: number;
}

export interface SnapshotNode {
  children: SnapshotNode[];
  type: string;
  name: string;
  id: number;
  shallowSize: number;
  from?: SnapshotNode;
  to?: SnapshotNode;
}

export interface Header {
  typeId: string;
  uid: string;
  title: string;
}

export interface CpuProfileNode {
  functionName: string;
  url: string;
  lineNumber: number;
  callUID?: number;
  bailoutReason: string;
  id: number;
  scriptId: number;
  hitCount: number;
  children: CpuProfileNode[];
}

export interface SamplingHeapProfileNode {
  callframe: {
    functionName: string;
    scriptId: number;
    url: string;
    lineNumber: number;
    columnNumber: number;
  };
  selfSize: number;
  children: SamplingHeapProfileNode[];
}

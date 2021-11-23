import { Stream } from "stream";

/**
 * Starts the profiler with a new profile.
 * @param name Name for the profile. "undefined" if not defined.
 * @param recsamples Is true by default.
 */
export function startProfiling(recsamples?: boolean): void;
export function startProfiling(name?: string, recsamples?: boolean): void;

/**
 * Stops the profiler for a specific profile.
 * @param name Name of the profile. "undefined" if not defined.
 */
export function setGenerateType(type: 0 | 1): void;
export function stopProfiling(name?: string): CpuProfile;
export function deleteAllProfiles(): void;
export function getObjectByHeapObjectId(id: number): Object;
export function deleteAllSnapshots(): void;
export function startSamplingHeapProfiling(): void;
export function startSamplingHeapProfiling(
  interval: number,
  depth: number
): void;
export function stopSamplingHeapProfiling(): SamplingHeapProfile;
export let profiles: Profile[];
export function startTrackingHeapObjects(): void;
export function stopTrackingHeapObjects(): void;
export function getHeapObjectId(value: Object): number;
export function setSamplingInterval(num: number): void;
export function takeSnapshot(control?: Function): Snapshot;
export function takeSnapshot(name?: string, control?: Function): Snapshot;
export function getHeapStats(
  iterator: Function | undefined,
  callback: Function | undefined
): number;
export let snapshots: { [key: string]: Snapshot };

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

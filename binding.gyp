{
  'targets': [
    {
      'target_name': 'profiler',
      # Electron 4+ requires this to be true
      'win_delay_load_hook': 'true',
      'sources': [
        'src/profiler.cc',
        'src/cpu_profiler.cc',
        'src/cpu_profile.cc',
        'src/cpu_profile_node.cc',
        'src/heap_profiler.cc',
        'src/heap_snapshot.cc',
        'src/heap_output_stream.cc',
        'src/heap_graph_node.cc',
        'src/heap_graph_edge.cc',
        'src/sampling_heap_profile.cc'
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

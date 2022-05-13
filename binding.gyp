{
  'targets': [
    {
      'target_name': 'profiler',
      'win_delay_load_hook': 'false',
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
        'src/sampling_heap_profile.cc',
        'src/environment_data.cc'
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ],
      'conditions':[
        ['OS == "linux"', {
          'cflags': ['-O2', '-std=c++14'],
        }],
        ['OS == "mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'OTHER_CFLAGS': [
              '-std=c++14',
              '-Wconversion',
              '-Wno-sign-conversion',
            ]
          }
        }],
      ]
    }
  ]
}

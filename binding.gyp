{
  'includes': [
    'config.gypi'
  ],

  'variables': {
    'action_after_build': 'false'
  },

  'targets': [
    {
      'target_name': 'profiler',
      'win_delay_load_hook': 'false',
      'sources': [
        'src/cpu_profiler/cpu_profiler.cc',
        'src/cpu_profiler/cpu_profile.cc',
        'src/cpu_profiler/cpu_profile_node.cc',
        'src/heap_profiler/sampling_heap_profiler.cc',
        'src/heapsnapshot/heap_profiler.cc',
        'src/heapsnapshot/heap_snapshot.cc',
        'src/heapsnapshot/heap_output_stream.cc',
        'src/heapsnapshot/heap_graph_node.cc',
        'src/heapsnapshot/heap_graph_edge.cc',
        'src/profiler.cc',
        'src/environment_data.cc'
      ],
      'include_dirs' : [
        'src',
        '<!(node -e "require(\'nan\')")'
      ],
      'conditions':[
        ['OS == "linux"', {
          'cflags': [
            '-O2',
            '-std=c++17',
            '-Wno-sign-compare',
            '-Wno-cast-function-type',
          ],
        }],
        ['OS == "mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'OTHER_CFLAGS': [
              '-std=c++17',
              '-Wconversion',
              '-Wno-sign-conversion',
            ]
          }
        }],
      ]
    },
  ],

  'conditions': [
    [
      'action_after_build == "true"',
      {
        'targets': [
          {
            'target_name': 'action_after_build',
            'type': 'none',
            'dependencies': ['<(module_name)'],
            'copies': [
              {
                'files': ['<(PRODUCT_DIR)/<(module_name).node'],
                'destination': '<(module_path)'
              }
            ]
          },
        ]
      }
    ]
  ],
}

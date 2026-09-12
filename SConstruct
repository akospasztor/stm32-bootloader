# Add the required toolchains to the path
# import os
# os.environ["PATH"] += os.pathsep + r'C:/gcc/10-2020-q4/bin'


# Command line options
AddOption('--all',
          dest='build_config',
          action='store_const',
          const='all',
          help='Build both debug and release configurations',
)
AddOption('--release',
          dest='build_config',
          action='store_const',
          const='release',
          help='Build the release configuration',
)
AddOption('--verbose',
          dest='verbose',
          action='store_true',
          help='Print detailed output of actions',
)

# Build configurations
configurations = [
    {
        'name': 'debug',
        'optimization_flags': [
            '-Og',
            '-g',
        ],
    },
    {
        'name': 'release',
        'optimization_flags': [
            '-O2',
        ],
    },
]

# Get selected configuration (if not specified, defaults to debug)
selected_config = GetOption('build_config') or 'debug'

# Build configuration
for build_config in configurations:
    if selected_config == build_config['name'] or selected_config == 'all':
        SConscript('SConscript', exports=['build_config'])

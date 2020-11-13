import os

# Add the required toolchains to the path
os.environ["PATH"] += os.pathsep + r'C:/Program Files (x86)/GNU Tools ARM Embedded/8 2019-q3-update/bin'

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
        SConscript('projects/STM32L496-Discovery/GCC/SConscript',
                   exports=['build_config'])

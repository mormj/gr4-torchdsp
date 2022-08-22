scheduler_triton_sources = [
    'graph_executor.cc',
    'thread_wrapper.cc',
    'scheduler_triton.cc',
]
scheduler_triton_deps = [gnuradio_gr_dep, threads_dep, fmt_dep, pmtf_dep, yaml_dep, gnuradio_blocklib_torchdsp_dep]

incdir = include_directories('../include', '../include/gnuradio/schedulers/triton')
gnuradio_scheduler_triton_lib = library('gnuradio-scheduler-triton', 
    scheduler_triton_sources, include_directories : incdir, 
    install : true,
    link_language : 'cpp',
    link_args : ['-lgnuradio-runtime'],
    dependencies : scheduler_triton_deps)

gnuradio_scheduler_triton_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_scheduler_triton_lib,
                       dependencies : scheduler_triton_deps )

torchdsp_sources += [
    'triton_model_impl.cc',
    'shm_utils.cc',
    'buffer_triton.cc'
]
torchdsp_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep, json_dep, triton_dep]

block_cpp_args = ['-DHAVE_TRITON']

incdir = include_directories(['../include/gnuradio/torchdsp','../include'])
gnuradio_blocklib_torchdsp_lib = library('gnuradio-blocklib-torchdsp', 
    torchdsp_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : torchdsp_deps,
    link_args : ['-lgnuradio-runtime'],
    cpp_args : block_cpp_args)

gnuradio_blocklib_torchdsp_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_torchdsp_lib,
                       dependencies : torchdsp_deps)

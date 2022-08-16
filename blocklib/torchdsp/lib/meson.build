torchdsp_sources += [
    'triton_model_impl.cc',
    'shm_utils.cc',
    'buffer_triton.cc'
]
torchdsp_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep, json_dep, triton_dep]

block_cpp_args = ['-DHAVE_CPU']

# if cuda_dep.found() and get_option('enable_cuda')
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_torchdsp_cu = library('gnuradio-blocklib-torchdsp-cu', 
#         torchdsp_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_torchdsp_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_torchdsp_cu,
#                         dependencies : cuda_dep)

#     torchdsp_deps += [gnuradio_blocklib_torchdsp_cu_dep, cuda_dep]

# endif

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

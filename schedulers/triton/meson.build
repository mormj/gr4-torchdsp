subdir('include/gnuradio/schedulers/triton')
subdir('lib')
# subdir('test') # tests should only be those that exercise the triton scheduler specifically
# subdir('bench')
if (get_option('enable_python'))
    subdir('python/schedulers/triton')
endif

install_data(sources : 'triton.scheduler.conf.yml', install_dir : 'etc/gnuradio/conf.d')

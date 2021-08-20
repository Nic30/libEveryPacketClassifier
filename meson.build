
project('packetClassificators', ['c', 'cpp'], default_options : ['cpp_std=c++17', 'c_std=c11'])


cpp = meson.get_compiler('cpp')
likwid = cpp.find_library('likwid', required: true)

pcv_proj = subproject('packet_classifiers_vectorized')
pcv_dep = pcv_proj.get_variable('pcv_dep')

nuevomatch_proj = subproject('nuevomatch')
nuevomatch_dep = nuevomatch_proj.get_variable('nuevomatch_dep')

subdir('src')
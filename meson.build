
project('packetClassificators', ['c', 'cpp'], default_options : ['cpp_std=c++17', 'c_std=c11'])
pcv_proj = subproject('packet_classifiers_vectorized')
pcv_dep = pcv_proj.get_variable('pcv_dep')

subdir('src')
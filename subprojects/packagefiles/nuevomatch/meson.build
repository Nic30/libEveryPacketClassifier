project('nuevomatch', ['c', 'cpp'], default_options : ['cpp_std=c++17', 'c_std=c11'])

nuevomatch_lib_sources = [
	#'src/algorithms.cpp',        'src/lookup.cpp',             'src/rqrmi_model.cpp',
	#'src/argument_handler.cpp',  'src/matrix_operations.cpp',  'src/rqrmi_tools.cpp',
	#'src/cpu_core_tools.cpp',    'src/nuevomatch.cpp',
	'src/rule_db.cpp',
	#'src/em_table.cpp',
	'src/object_io.cpp',
	# 'src/simd_aux.cpp',
	#'src/interval_set.cpp',      'src/python_library.cpp',
	'src/string_operations.cpp',
	'src/logging.cpp',
	# 'src/rqrmi_fast.cpp',
	'src/vector_list.cpp',

	'vendor/cut_split.cpp',  'vendor/efficuts.cpp',  'vendor/hyper_split.cpp',  #'vendor/neurocuts.cpp',  #'vendor/tuple_merge.cpp',
]
nuevomatch_lib_include = [include_directories('vendor'), include_directories('include')]
nuevomatch_lib_static = static_library('nuevomatch',
    nuevomatch_lib_sources,
	cpp_args: ['-DNDEBUG', '-DLIKWID_PERFMON'],
    install : true,
	include_directories : nuevomatch_lib_include,
)
nuevomatch_dep = declare_dependency(
  link_with : nuevomatch_lib_static,
  include_directories : nuevomatch_lib_include
)
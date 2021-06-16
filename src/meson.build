
srcs = [
	'BitVector/EqnMatcher.cpp',
	'BitVector/LongestPrefixMatch.cpp',
	'BitVector/BitSet.cpp',
	'BitVector/BitVector.cpp',
	'ClassBenchTraceGenerator/trace_tools.cc',
	'TupleMerge/SlottedTable.cpp',
	'TupleMerge/TupleMergeOnline.cpp',
	'TupleMerge/TupleMergeOffline.cpp',
	'HyperSplit/HyperSplit.cpp',
	'HyperSplit/HyperSplit_nodes.cpp',
	'Trees/TreeUtils.cpp',
	'ByteCuts/ByteCuts.cpp',
	'ByteCuts/ByteCutsNode.cpp',
	'ByteCuts/TreeBuilder.cpp',
	'HyperCuts/HyperCuts.cpp',
	'IO/InputReader.cpp',
	'IO/OutputWriter.cpp',
	'Utilities/MapExtensions.cpp',
	'Utilities/IntervalUtilities.cpp',
	'Utilities/Tcam.cpp',
	'Simulation.cpp',
	'OVS/TupleSpaceSearch.cpp',
	'OVS/cmap.cpp',
	'PartitionSort/test_red_black_tree.cpp',
	'PartitionSort/OptimizedMITree.cpp',
	'PartitionSort/red_black_tree.cpp',
	'PartitionSort/PartitionSort.cpp',
	'PartitionSort/SortableRulesetPartitioner.cpp',
]

EXTRA_CXX_ARGS = [
	'-std=c++17',
 	'-fopenmp',
 	'-m64',
	'-march=haswell',
	'-mavx2',
 	#'-pg', '-no-pie',
    #'-O3', '-DNDEBUG',
	'-g',
]
EXTRA_C_ARGS = [
 	#'-pg', '-no-pie',
    #'-O3', '-DNDEBUG',
	'-g',
]
EXTRA_LINK_ARGS = [
    #'-pg', '-no-pie'
]

boost_thread = dependency('boost', modules: ['thread', 'system' ])

packetClassificatorsCommon = shared_library('packetClassificatorsCommon',
                        srcs,
                        #include_directories : inc,
                        dependencies : [boost_thread],
                        install : true,
                        cpp_args: EXTRA_CXX_ARGS,
                        c_args: EXTRA_C_ARGS,
                        #link_args: EXTRA_LINK_ARGS
                        )
boost_unittest = dependency('boost', modules: ['unit_test_framework', ])
thread_dep = dependency('threads')

cc = meson.get_compiler('c')
libgomp = cc.find_library('gomp')

main = executable('packetClassificators',
	['packetClassificators.cpp', ],
	link_with: [packetClassificatorsCommon],
	dependencies: [libgomp, thread_dep, boost_thread, pcv_dep, nuevomatch_dep],
	cpp_args: EXTRA_CXX_ARGS,
    c_args: EXTRA_C_ARGS,
    link_args: EXTRA_LINK_ARGS
)


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
	'far/far.cpp',
	'far/exceptions.cpp',
	'OVS/TupleSpaceSearch.cpp',
	'OVS/cmap.cpp',
	'PartitionSort/test_red_black_tree.cpp',
	'PartitionSort/OptimizedMITree.cpp',
	'PartitionSort/red_black_tree.cpp',
	'PartitionSort/PartitionSort.cpp',
	'PartitionSort/SortableRulesetPartitioner.cpp',
]

EXTRA_CXX_ARGS = ['-std=c++17']

packetClassificatorsCommon = shared_library('packetClassificatorsCommon',
                        srcs,
                        #include_directories : inc,
                        #dependencies : glib_dep,
                        install : true,
                        cpp_args: EXTRA_CXX_ARGS)

boost_unittest = dependency('boost', modules : ['unit_test_framework', ])


main = executable('packetClassificators',
	['packetClassificators.cpp', ],
	link_with: [packetClassificatorsCommon, ],
	cpp_args: EXTRA_CXX_ARGS)

far_tests = [
	['fragmentedValue_test', 'Fragmented value Tests'],
	['llrbTree_test', 'Left Learning Red black tree Tests'], 
	['ruleGroupResolver_test', 'Rule group resolver Tests'],
	['compressedDecisionTree_test', 'Tests for Compressed decision tree and its compiler'],
]

foreach t : far_tests
	test_exe = executable(t[0],
		['far/tests/' + t[0] + '.cpp'],
		dependencies : [ boost_unittest, ],
		link_with: [packetClassificatorsCommon, ],
		cpp_args: EXTRA_CXX_ARGS)
	test(t[1], test_exe, workdir : meson.source_root())
endforeach

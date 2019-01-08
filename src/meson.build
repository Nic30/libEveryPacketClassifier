

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

packetClassificatorsCommon = shared_library('packetClassificatorsCommon',
                        srcs,
                        #include_directories : inc,
                        #dependencies : glib_dep,
                        install : true)

boost_unittest = dependency('boost', modules : ['unit_test_framework', ])


main = executable('packetClassificators',
	['packetClassificators.cpp', ],
	link_with: [packetClassificatorsCommon, ])

fragmented_value_test = executable('fragmented_value_test',
	['far/fragmented_value_test.cpp'],
	dependencies : [ boost_unittest, ],
	link_with: [packetClassificatorsCommon, ])
test('Fragmented value Tests', fragmented_value_test)

rbtree_unittest = executable('rbtree_unittest',
	['far/rbtree_test.cpp'],
	dependencies : [boost_unittest, ],
	link_with: [packetClassificatorsCommon, ])
test('Red black tree Tests', rbtree_unittest)

rule_group_resolver_test = executable('rule_group_resolver_test',
	srcs + ['far/rule_group_resolver_test.cpp'],
	dependencies : [boost_unittest, ],
	link_with: [packetClassificatorsCommon, ])
test('Rule group resolver Tests', rule_group_resolver_test)

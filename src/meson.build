

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
	'OVS/TupleSpaceSearch.cpp',
	'OVS/cmap.cpp',
	'PartitionSort/test_red_black_tree.cpp',
	'PartitionSort/OptimizedMITree.cpp',
	'PartitionSort/red_black_tree.cpp',
	'PartitionSort/PartitionSort.cpp',
	'PartitionSort/SortableRulesetPartitioner.cpp',
]

boost_unittest = dependency('boost', modules : ['unit_test_framework', ])


main = executable('packetClassificators', srcs + [	'packetClassificators.cpp', ])

rbtree_unittest = executable('rbtree_unittest',
	srcs + ['far/rbtree_test.cpp'],
	dependencies : [boost_unittest, ])
test('Red black tree Tests', rbtree_unittest)
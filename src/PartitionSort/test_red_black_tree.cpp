#include "OptimizedMITree.h"

#include <stdio.h>
#include <ctype.h>

int rb_selftest0() {
	Rule r1(4);
	r1.priority = 1;
	r1.range[0] = {1, 3};
	r1.range[1] = {1, 3};
	r1.range[2] = {0, 1};
	r1.range[3] = {0, 1};
	Rule r2(4);
	r2.priority = 2;
	r2.range[0] = {1, 3};
	r2.range[1] = {1, 3};
	r2.range[2] = {0, 1};
	r2.range[3] = {0, 3};
	Rule r3(4);
	r3.priority = 3;
	r3.range[0] = {2, 2};
	r3.range[1] = {2, 2};
	r3.range[2] = {0, 1};
	r3.range[3] = {0, 2};
	Rule r4(4);
	r4.priority = 4;
	r4.range[0] = {1, 3};
	r4.range[1] = {1, 3};
	r4.range[2] = {0, 1};
	r4.range[3] = {2, 5};
	Rule r5(4);
	r5.priority = 5;
	r5.range[0] = {1, 3};
	r5.range[1] = {4, 5};
	r5.range[2] = {0, 1};
	r5.range[3] = {0, 1};

	OptimizedMITree mitree;
	bool prioritychange = 0;
	mitree.Insertion(r1, prioritychange);
	//mitree.Insertion(r4,prioritychange);
	//mitree.Insertion(r3,prioritychange);
	mitree.Insertion(r4, prioritychange);

	//printf("Inserting r5\n");
	mitree.Insertion(r5, prioritychange);

	printf("Done\n");
	Packet p5 = { 1, 4, 1, 1 };
	Packet p1 = { 1, 1, 1, 1 };
	Packet p2 = { 1, 1, 1, 2 };
	printf("p1 result: %d\n", mitree.ClassifyAPacket(p1));
	printf("p2 result: %d\n", mitree.ClassifyAPacket(p2));
	printf("p5 result: %d\n", mitree.ClassifyAPacket(p5));
	//r1.priority = 120;
	//mitree.Deletion(r5, prioritychange); 
	mitree.Deletion(r1, prioritychange);

	printf("last query\n");
	printf("p1 result: %d\n", mitree.ClassifyAPacket(p1));

	//NOW TEST SERIALIZE
	//only r4 alone
	auto vr4 = mitree.SerializeIntoRules();
	auto PrintRule = [](const Rule& r) {
		printf("Priority %d\n", r.priority);
		for (int i = 0; i < r.dim; i++) {
			std::cout << r.range[i] << " " << std::endl;
		}
		std::cout << std::endl;
	};
	auto PrintVectorRules = [&PrintRule](const std::vector<Rule>& vr) {
		for (const auto &r : vr) {
			PrintRule(r);
		}
	};

	PrintVectorRules(vr4);

	mitree.Insertion(r1);
	mitree.Insertion(r5);

	PrintVectorRules(mitree.SerializeIntoRules());

	//mitree.Deletion(r1, prioritychange);
	//printf("p1 result: %d\n", mitree.ClassifyAPacket(p1));
	return 0;
}

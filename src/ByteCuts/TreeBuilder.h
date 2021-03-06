/*
 * MIT License
 *
 * Copyright (c) 2017 by J. Daly at Michigan State University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include "ByteCutsNode.h"

SpanRange GetSpan(const Rule& rule, uint8_t dim, uint8_t left, uint8_t right);
void CleanRules(std::vector<Rule>& rules);

class TreeBuilder {
public:
	typedef std::function<bool(const Rule&, uint8_t dim, uint8_t nl, uint8_t nr)> Allower;
	typedef std::function<ByteCutsNode*(const std::vector<Rule>&, std::vector<Rule>&, int, int)> Builder;
	/*
	 * @param dim: number of dimension
	 * @param nl: number of most left bit
	 * @param dim: number of most right bit
	 * */
	using SearchStepApplyFn = std::function<void(size_t fitness, size_t penalty, uint8_t dim, uint8_t nl, uint8_t nr)>;

	TreeBuilder(size_t leafSize) : leafSize(leafSize) {
		allowableDims = {0, 1, 4};
		splitDims = {2, 3};
	}

	std::tuple<uint8_t, uint8_t, uint8_t, size_t> BestSpan(const std::vector<Rule>& rules, Allower isAllowed, int penaltyRate);
	void BestSpanSearch(const std::vector<Rule>& rules, Allower isAllowed, SearchStepApplyFn searchStepApplyFn);
	std::tuple<uint8_t, uint8_t, uint8_t, size_t> BestSpanMinPart(const std::vector<Rule>& rules, Allower isAllowed, int penaltyRate);
	std::tuple<uint8_t, uint8_t, uint8_t, size_t> BestSpanMinPenalty(const std::vector<Rule>& rules, Allower isAllowed, int penaltyRate);
	std::tuple<uint8_t, uint16_t, size_t> BestSplit(const std::vector<Rule>& rules);
	
	ByteCutsNode* BuildNode(const std::vector<Rule>& rules, std::vector<Rule>& remain, int depth, int penaltyRate);
	ByteCutsNode* BuildPrimaryRoot(const std::vector<Rule>& rules, std::vector<Rule>& remain);
	ByteCutsNode* BuildSecondaryRoot(const std::vector<Rule>& rules, std::vector<Rule>& remain);
	
	bool BuiltSplit() const { return madeHyperSplit; }
private:
	ByteCutsNode* BuildNodeHelper(
			const std::vector<Rule>& rules, 
			std::vector<Rule>& remain, 
			int depth,
			Allower isAllowed, 
			Builder builder,
			int penaltyRate);
	ByteCutsNode* BuildRootHelper(
			const std::vector<Rule>& rules, 
			std::vector<Rule>& remain, 
			int depth,
			Allower isAllowed, 
			Builder builder,
			int penaltyRate);
	ByteCutsNode* BuildCutNode(
			const std::vector<Rule>& rules, 
			std::vector<Rule>& remain, 
			int depth,
			Allower isAllowed, 
			Builder builder,
			int penaltyRate,
			uint8_t d, uint8_t nl, uint8_t nr);

	std::vector<uint8_t> allowableDims;
	std::vector<uint8_t> splitDims;
	size_t leafSize;
	size_t numNodes = 0;
	
	bool madeHyperSplit = false;
};

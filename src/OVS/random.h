/*
* Copyright (c) 2008, 2009, 2010, 2012 Nicira, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "../ElementaryClasses.h"

thread_local Random ovs_rand;

uint32_t random_uint32(void) {
	return ovs_rand.random_unsigned_int();
}
uint64_t random_uint64(void) {
	printf("warning random_uint64 unimplemented\n");
	return ovs_rand.random_unsigned_int();
}

static inline int
random_range(int max)
{
	return ovs_rand.random_unsigned_int() % max;
}

static inline uint8_t
random_uint8(void)
{
	return ovs_rand.random_unsigned_int();
}

static inline uint16_t
random_uint16(void)
{
	return ovs_rand.random_unsigned_int();
}

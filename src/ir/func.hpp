#pragma once

#include "../meow/core.hpp"
#include "block.hpp"

namespace buaac {

	class Func {

	public:

		BlocksPtr blocks;

		string func_name;
		
		Func(string func_name) :func_name(func_name) {
			blocks = std::make_shared<Blocks>();
			blocks->clear();
			blocks->emplace_back(Block(func_name));
		}
		
	};
	
}
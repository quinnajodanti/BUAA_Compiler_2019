#ifndef REG_POOL_HPP
#define REG_POOL_HPP

#include "../meow/core.hpp"

namespace buaac {
	class RegPool {

		using Id = string;
		using Reg = string;
		
		std::map<string, int> memPool;
	
	public:
		int mempool_size = 0;
		std::map<Id, Reg> regPool;
		std::map<Reg, bool> availReg;
		vector<vector<Reg>> t_stack;
		vector<Reg> globalRegs = {
			"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
			"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
			"$v1",
			"$k1",

		};
		

		void resetRegPool() {
			mempool_size = 0;
			memPool.clear();
			regPool.clear();
			for (int i = 0; i < globalRegs.size(); i++) {
				availReg[globalRegs[i]] = true;
			}
			t_stack.clear();
		}

		string getRegPool(string id) {
			if (notFound(regPool, id)) {
				if (haveAvailReg()) {
					allocRegPool(id);
				} else {
					return id;
				}
			}
			return regPool[id];
		}

		void checkAndRelease(map<Id, VarRange> ident_to_range, int block_index, int block_line_number) {
			vector<Id> erase_list;
			
			for (auto id : regPool) {
				if (ident_to_range[id.first].out(block_index, block_line_number)) {
					availReg[id.second] = true;
					erase_list.push_back(id.first);
					// debugln("ReleaReg: {} {}", id.first, id.second);
				}
			}

			for (auto id : erase_list) {
				regPool.erase(id);
			}
		}
		
		void releaseRegPool(string id) {
			
		}

		int getMemPool(string id) {
			if (memPool.find(id) == memPool.end()) {
				return allocMemPool(id);
			}
			return memPool[id];
		}
		
	private:
		

		int allocMemPool(string id) {
			memPool[id] = mempool_size;
			return mempool_size++;
		}

		

		string getReg() {
			for (int i = 0; i < globalRegs.size(); i++) {
				if (availReg[globalRegs[i]]) {
					availReg[globalRegs[i]] = false;
					return globalRegs[i];
				}
			}
			panic("no avail reg");
		}

		bool haveAvailReg() {
			for (int i = 0; i < globalRegs.size(); i++) {
				if (availReg[globalRegs[i]]) {
					return true;
				}
			}
			return false;
		}

		void allocRegPool(string id) {
			string reg = getReg();
			regPool[id] = reg;
			// debugln("AllocReg {} {}", id, reg);
		}

		

	};
}


#endif

//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once

#include <iostream>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

class address_scanner;

class address_scan_result {
public:
	address_scan_result(std::vector<ptrdiff_t> locations);

	ptrdiff_t operator[](long index);

	address_scan_result mutual_set(const address_scan_result &);

	size_t size();

private:
	std::vector<ptrdiff_t> _locations;
};

template<typename T>
class address_scan_state {
public:
	address_scan_state() {}

	size_t size() {
		return _values.size();
	}

	const std::map<ptrdiff_t, T> &values() {
		return _values;
	};
private:
	friend class address_scanner;
	address_scan_state(uint8_t *memory, size_t size) {
		for (size_t i = 0; i < size - sizeof(T); i++) {
			_values[i] = *((T *) (memory + i));
		}
	}

	address_scan_state(std::map<ptrdiff_t, T> values) : _values{values} {
	}

	address_scan_state<T> filter(address_scan_state<T> previous, std::function<bool(T, T)> comparator) {
		std::map<ptrdiff_t, T> results;

		for(auto &values : previous._values) {

			if(comparator(_values[values.first], values.second))
			{
				results[values.first] = _values[values.first];
			}
		}

		return address_scan_state<T>(results);
	}

	address_scan_state<T> exclusive(address_scan_state<T> previous) {
		return filter(previous, [](T lhs, T rhs)->bool { return lhs != rhs; });
	}

	address_scan_state<T> same(address_scan_state<T> previous) {
		return filter(previous, [](T lhs, T rhs)->bool { return lhs == rhs; });
	}

	address_scan_state<T> greaterThan(address_scan_state<T> previous) {
		return filter(previous, [](T lhs, T rhs)->bool { return lhs > rhs; });
	}

	address_scan_state<T> lessThan(address_scan_state<T> previous) {
		return filter(previous, [](T lhs, T rhs)->bool { return lhs < rhs; });
	}

	std::map<ptrdiff_t, T> _values;
};


class address_scanner {
public:
	address_scanner(uint8_t *memory, size_t size);

	template<typename T>
	address_scan_result find_value(T target_value) {
		std::vector<ptrdiff_t> locations;
		for (int i = 0; i < _size - sizeof(T); i++) {
			if (*((T *) (_memory + i)) == target_value) {
				locations.push_back(i);
			}
		}
		return address_scan_result{locations};
	}

	template<typename T>
	address_scan_state<T> snapshot() {
		return address_scan_state<T>{_memory, _size};
	}

	template<typename T>
	address_scan_state<T> changed_value(address_scan_state<T> &previousState) {
		address_scan_state<T> currentState{_memory, _size};
		return currentState.exclusive(previousState);
	}

	template<typename T>
	address_scan_state<T> unchanged_value(address_scan_state<T> &previousState) {
		address_scan_state<T> currentState{_memory, _size};
		return currentState.same(previousState);
	}

	template<typename T>
	address_scan_state<T> increased_value(address_scan_state<T> &previousState) {
		address_scan_state<T> currentState{_memory, _size};
		return currentState.greaterThan(previousState);
	}

	template<typename T>
	address_scan_state<T> decreased_value(address_scan_state<T> &previousState) {
		address_scan_state<T> currentState{_memory, _size};
		return currentState.lessThan(previousState);
	}

private:
	uint8_t *_memory;
	size_t _size;
};

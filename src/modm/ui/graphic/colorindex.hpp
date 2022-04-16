#pragma once

#include <initializer_list>
// #include <ranges>
#include <modm/ui/color/concepts.hpp>

namespace modm::graphic {

	/**
	 * @brief				A very sinple C-array wrapper, inplied for type-safety and some other protection.
	 * 
	 * @tparam ValueType 	The output color:Color. Can also be a non-color::Color Type, May be const, * or &
	 * @tparam KeyType		The color::ColorGray<D> used for the 'compressed' source
	 * @tparam N			You can explicitely overwrite the number of Values for developement purpose.
	 * 						This also enables to take foreign data: Just static_cast a ColorIndex<ValueType, KeyType, 0>
	 * 						onto some interresting data determine your images colors with the current 1s and 0s of the memory-area.
	 * 						
	 * 						Same works with the Buffers by the way ;) Just static_cast them wherever you want and don't instantiate anything!
	 */

template<typename ValueType, color::Color KeyType, int N = KeyType::max + 1>
class ColorIndex {
	ValueType index[N];
	
public:
	// ColorIndex() = default;

	consteval ColorIndex(const std::initializer_list<ValueType> list)
	{
		std::ranges::copy(list, std::begin(index));
	};

	// ValueType& operator[](KeyType key)
	// { return index[key.value()]; }

	ValueType operator[](KeyType key) const
	{ return index[key.value()]; }
};
} // namespace modm::graphic
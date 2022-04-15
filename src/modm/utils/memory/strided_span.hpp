
namespace modm {

/**
 * @brief 		std::span like but with fixed Stride.
 * 				Inspired from: https://www.boost.org/doc/libs/1_47_0/libs/range/doc/html/range/reference/adaptors/reference/strided.html
 * 
 * 				Not fully implemented yet. Just the basics to make it work for graphic::Buffer.
 */

// OPTIMIZE Try passing stride as const rvalue instead of template parameter
template<typename T, std::ptrdiff_t Stride>
class strided_span {
public:
	// member types
	using element_type           = T;
	using value_type             = std::remove_cv_t<T>;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;
	using pointer                = T*;
	using const_pointer          = const T*;
	using reference              = element_type&;
	using const_reference        = const element_type&;
	// using iterator = __gnu_cxx::__normal_iterator<pointer, span>;
	// using reverse_iterator       = std::reverse_iterator<iterator>;

	constexpr strided_span(T *data, size_type extend)
		: ptr(data), extend(extend) {}

	constexpr strided_span
	subspan(size_type offset, size_type count) {
		// modm_assert(offset <= size());
		return strided_span(ptr + Stride * offset, Stride * count);
	}

	constexpr size_type
    size() const noexcept
    { return extend; }

	pointer data() {
		return ptr;
	}

	struct iterator
	{
		using iterator_category = std::contiguous_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = T;
    	using pointer           = T*;
	    using reference         = T&;

		iterator(T *ptr) : ptr(ptr) {}

		void operator++() { ptr += Stride; }
		void operator--() { ptr -= Stride; }

		iterator operator+(difference_type value) { return {ptr + Stride * value}; }
		iterator operator-(difference_type value) { return {ptr - Stride * value}; }

		void operator+=(difference_type value) {ptr += Stride * value; }
		void operator-=(difference_type value) {ptr -= Stride * value; }

		// bool operator<=>(const iterator& other) const = default;
		bool operator==(const iterator& other) const
		{ return ptr == other.ptr; }

		value_type operator*() const { return *this->ptr; }
		reference operator*() { return *this->ptr; }
		
	// protected:
		pointer ptr;
	};

	struct reverse_iterator : public iterator
	{
		using iterator_category = std::contiguous_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = T;
    	using pointer           = T*;
	    using reference         = T&;
		
		void operator++() { this->ptr -= Stride; }
		void operator--() { this->ptr += Stride; }

		void operator+=(difference_type value) {this->ptr -= Stride * value; }
		void operator-=(difference_type value) {this->ptr += Stride * value; }

		bool operator==(const iterator& other) { return this->ptr == other.ptr; }

		value_type operator*() const { return *this->ptr; }
		reference operator*() { return *this->ptr; }
	};

	// TODO suppress : -Warray-bounds
	constexpr iterator
	begin() noexcept { return iterator(ptr); }
	
	constexpr iterator
	end() noexcept { return iterator(ptr + extend); }

	// FIXME reverse iteration not yet tested
	constexpr reverse_iterator
	rbegin() noexcept { return reverse_iterator(this->end()); }

	constexpr reverse_iterator
	rend() noexcept { return reverse_iterator(this->begin()); }

private:
	T *ptr;
	const size_type extend;
};
}
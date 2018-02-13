#include <cstddef>
// #include <stdexcept>
#include <string>

class str_const {
    const char * const p_;
    const std::size_t sz_;
public:
    template <std::size_t N>
    constexpr str_const( const char( & a )[ N ] )
    : p_( a ), sz_( N - 1 ) {}
    constexpr char operator[]( std::size_t n ) const {
        return n < sz_ ? p_[ n ] : throw std::out_of_range( "" );
    }
    constexpr std::size_t size() const { return sz_; }

    operator std::string() const { return std::string(p_); }
};
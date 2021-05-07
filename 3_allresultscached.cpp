#include<iostream>
#include<iomanip>
#include<sstream>
#include<array>

using std::array;
using std::ostream;
using Rep = std::uint32_t;
constexpr Rep MaxDep = 16;
constexpr Rep DoubleMaxDeg = 32;
constexpr Rep calculateDegree(Rep arg){
    Rep ret=0;
    for(;arg;arg>>=1) ++ret;
    return ret-1;
}

template<Rep genpoly>
struct DegTerm{
    constexpr static size_t num = 1 << calculateDegree(genpoly);
};

template <Rep genpoly>
constexpr Rep slowMult(Rep a,Rep b){
    constexpr Rep degree = calculateDegree(genpoly);
    constexpr Rep degreeTerm = DegTerm<genpoly>::num;
    constexpr Rep upperLimit = DoubleMaxDeg - 1 - degree;
    Rep ret=0;
    while(b!=0){
        ret ^= a * (b&1);
        a*=2; b/=2;
    }
    Rep genpoly_shifted = genpoly << upperLimit;
    Rep degreeTerm_shifted = degreeTerm << upperLimit;
    while(genpoly_shifted >= genpoly){
        ret ^= (genpoly_shifted) * ( !!(ret & degreeTerm_shifted) );
        genpoly_shifted/=2;
        degreeTerm_shifted/=2;
    }
    return ret;
}

template<Rep genpoly>
constexpr array<array<Rep,DegTerm<genpoly>::num>,DegTerm<genpoly>::num> makeMultTable(){
    constexpr Rep numberOfElements = DegTerm<genpoly>::num;
    array<array<Rep,numberOfElements>,numberOfElements> multTable {0};

    for(Rep i=0;i<numberOfElements;++i){
        for(Rep j=0;j<numberOfElements;++j){
            multTable[i][j] = slowMult<genpoly>(i,j);
        }
    }
    return multTable;
}

template<Rep genpoly>
constexpr array<array<Rep,DegTerm<genpoly>::num>,DegTerm<genpoly>::num> makeDivTable(){
    constexpr Rep numberOfElements = DegTerm<genpoly>::num;
    constexpr Rep order = numberOfElements-1;
    array<array<Rep,numberOfElements>,numberOfElements> divTable {0};

    for(Rep right=1;right<numberOfElements;++right){
        Rep inv=1;
        for(int j=0;j<order-1;++j) inv = slowMult<genpoly>(inv,right);
        for(Rep left=0;left<numberOfElements;++left){
            divTable[left][right] = slowMult<genpoly>(left,inv);
        }
    }
    return divTable;
}

template<Rep GeneratingPolynomial>
class GF2{
    static_assert(GeneratingPolynomial < 0x00000200);
    private:
    static constexpr Rep genpoly = GeneratingPolynomial;
    static constexpr Rep degree = calculateDegree(genpoly);
    static constexpr Rep degreeTerm = DegTerm<genpoly>::num;
    static constexpr Rep order = degreeTerm-1;
    static constexpr Rep equivDegreeTerm = genpoly^degreeTerm;
    static constexpr array<array<Rep,degreeTerm>,degreeTerm> multTable = makeMultTable<genpoly>();
    static constexpr array<array<Rep,degreeTerm>,degreeTerm> divTable = makeDivTable<genpoly>();

    Rep rep;
    public:
    GF2(Rep __rep=0): rep{__rep} {}
    const GF2 operator+(const GF2& other) const{
        return GF2(rep^other.rep);
    }
    inline const GF2 operator-(const GF2& other) const{
        return rep + other;
    }
    inline const GF2 operator^(const GF2& other) const{
        return rep + other;
    }
    const GF2 operator-() const{
        return GF2(rep);
    } 
    const GF2 operator*(const GF2& other) const{
        return GF2(multTable.at(rep).at(other.rep));
    }
    const GF2 operator/(const GF2& other) const{
        if (other.rep==0) throw std::invalid_argument("Division by Zero");
        return GF2(divTable.at(rep).at(other.rep));
    }
    GF2& operator+=(const GF2& other){
        rep ^= other.rep; return (*this);
    }
    GF2& operator-=(const GF2& other){
        rep ^= other.rep; return (*this);
    }
    GF2& operator^=(const GF2& other){
        rep ^= other.rep; return (*this);
    }
    GF2& operator*=(const GF2& other){
        rep = multTable.at(rep).at(other.rep); return (*this);
    }
    GF2& operator/=(const GF2& other){
        if(other.rep==0) throw std::invalid_argument("Division by Zero");
        rep = divTable.at(rep).at(other.rep);  return (*this);
    }
    void print(ostream& out) const{
        std::stringstream ss;
        ss << std::hex << rep ;
        out << ss.str();
    }
};

template<Rep genpoly>
ostream& operator<<(ostream& out,const GF2<genpoly>& x){
    x.print(out); return out;
}

int main(){
    GF2<0x11d> x(0x05),y(0x03);
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x*=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x/=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
}

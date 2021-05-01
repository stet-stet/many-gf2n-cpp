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
//workaround to enable degree term calculation in template parameters/
template<Rep N>
struct DegTerm{
    constexpr static size_t num = 1 << calculateDegree(N);
};
template<Rep genpoly>
constexpr Rep slowMult(Rep a,Rep b){
    constexpr Rep degree = calculateDegree(genpoly);
    constexpr Rep degreeTerm = 1<<degree;
    constexpr Rep upperLimit = DoubleMaxDeg - 1 - degree;
    Rep ret=0;
    while(b!=0){
        ret += a * (b&1);
        a*=2; b/=2;
    }
    Rep genpoly_shifted = genpoly << upperLimit;
    Rep degreeTerm_shifted = degreeTerm << upperLimit;
    while(genpoly_shifted >= genpoly){
        ret ^= (genpoly_shifted) * ( !!(ret & degreeTerm_shifted) );
        genpoly_shifted/=2;
        degreeTerm_shifted/=2;
    }
    /*
    for(Rep i=upper_limit;i>=0;--i){
        ret ^= (  ( genpoly<<i ) * ( !!( ret&(degreeTerm<<i) ) )  );
    }*/ 
    // the above for-loop does not work, because 
    // 1. unsigned integer overflow is apparently banned in constexpr context
    // 2. compiler tests constexpr expressions so that they will not act fuzzy "whatever" we do
    // 3. genpoly << (2^32-1) , of course, causes overflow.
    return ret;
} 

template<Rep genpoly,Rep gen>
constexpr array<Rep,DegTerm<genpoly>::num> makeExpTable(){
    constexpr Rep order = DegTerm<genpoly>::num-1;
    Rep a = 1u;
    array<Rep,DegTerm<genpoly>::num> ret{0};
    for(int i=0;i<order;++i){
        ret[i] = a;
        a = slowMult<genpoly>(a,gen); // gen is on the right for a reason
    }
    return ret;
}

template<Rep genpoly,Rep gen>
constexpr array<int,DegTerm<genpoly>::num> makeLogTable(){
    constexpr Rep order = DegTerm<genpoly>::num-1;
    Rep a = 1;
    array<int,DegTerm<genpoly>::num> ret{0};
    for(int i=0;i<order;++i){
        ret[a] = i;
        a = slowMult<genpoly>(a,gen);
    }
    return ret;
}

template<Rep GeneratingPolynomial,Rep generator>
class GF2{
    static_assert(GeneratingPolynomial<0x00020000);
    private:
    static constexpr Rep gen = generator;
    static constexpr Rep genpoly = GeneratingPolynomial;
    static constexpr Rep degree = calculateDegree(genpoly);
    static constexpr Rep degreeTerm = DegTerm<genpoly>::num;
    static constexpr Rep order = degreeTerm-1;
    static constexpr Rep equivDegreeTerm = genpoly^degreeTerm;
    static constexpr array<Rep,DegTerm<genpoly>::num> gf_exp = makeExpTable<genpoly,gen>() ;
    static constexpr array<int,DegTerm<genpoly>::num> gf_log = makeLogTable<genpoly,gen>() ;
    Rep rep;
    //=======================NONSTATIC==========================
    public:
    GF2(Rep __rep=0): rep{__rep} {}
    const GF2 operator+(const GF2& other) const{
        return GF2(rep^other.rep);
    }
    const GF2 operator-(const GF2& other) const{
        return GF2(rep^other.rep);
    }
    const GF2 operator^(const GF2& other) const{
        return GF2(rep^other.rep);
    }
    const GF2 operator-() const{
        return GF2(rep);
    }
    const GF2 operator*(const GF2& other) const{
        if(rep==0||other.rep==0) return GF2(0);
        return GF2(gf_exp.at((gf_log.at(rep)+gf_log.at(other.rep))%order));
    }
    const GF2 operator/(const GF2& other) const{
        if(rep==0) return GF2(0);
        else if(other.rep==0) throw std::invalid_argument("Division by Zero");
        return GF2(gf_exp.at((gf_log.at(rep)-gf_log.at(other.rep)+order)%order));
    }
    GF2& operator+=(const GF2& other){
        rep ^= other; return (*this);
    }
    GF2& operator-=(const GF2& other){
        rep ^= other; return (*this);
    }
    GF2& operator^=(const GF2& other){
        rep ^= other; return (*this);
    }
    GF2& operator*=(const GF2& other){
        if(rep==0 || other.rep==0) rep=0;
        else rep = gf_exp.at((gf_log.at(rep)+gf_log.at(other.rep))%order);
        return (*this);
    }
    GF2& operator/=(const GF2& other){
        if(rep==0) return (*this);
        else if(other.rep==0) throw std::invalid_argument("Division by Zero");
        rep = gf_exp.at((gf_log.at(rep)-gf_log.at(other.rep)+order)%order);
        return (*this);
    }
    void print(ostream& out) const{
        std::stringstream ss;
        ss << std::hex << rep;
        out << ss.str();
    }
};

template<Rep genpoly,Rep gen>
ostream& operator<<(ostream& out,const GF2<genpoly,gen>& x){
    x.print(out); return out;
}

int main(){
    GF2<0x11d,0x02> x(0x05),y(0x03);
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << x*y << std::endl;
    x*=y;
    std::cout << std::setw(2)<< x*y << std::endl;
    x/=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
} // this compiles well
// compile-time evaluation of gf_exp and gf_log is a success 
// (otherwise the initialization of array<Rep,~~~> should result in errors)

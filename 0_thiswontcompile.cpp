#include<iostream>
#include<sstream>
#include<map>

using std::map;
using std::ostream;
using Rep = std::uint32_t;
constexpr Rep MaxDep = 16;
constexpr Rep DoubleMaxDeg = 32;

constexpr Rep calculateDegree(Rep arg){
    Rep ret=0;
    for(;arg;arg>>=1) ++ret;
    return ret;
}
template<Rep GeneratingPolynomial,Rep generator>
class GF2{
    public:
    class FieldElement{
        private: 
        Rep rep;
        public:
        FieldElement() = delete;
        FieldElement(Rep x): rep{x} {}
        const FieldElement operator+(const FieldElement& other) const{
            return FieldElement(other.rep ^ rep);
        }
        const FieldElement operator-(const FieldElement& other) const{
            return FieldElement(other.rep ^ rep);
        }
        const FieldElement operator^(const FieldElement& other) const{
            return FieldElement(other.rep ^ rep);
        }
        const FieldElement operator*(const FieldElement& other) const{
            if(rep==0 || other.rep==0) return FieldElement(0);
            return FieldElement(gf_exp[(gf_log[rep]+gf_log[other.rep])%order]);
        }
        const FieldElement operator/(const FieldElement& other) const{
            if(rep==0) return FieldElement(0);
            else if(other.rep==0) throw std::invalid_argument("Division by Zero");
            return FieldElement(gf_exp[(gf_log[rep]-gf_log[other.rep]+order)%order]);
        }
        FieldElement& operator-() const{
            return (*this);
        }
        FieldElement& operator+=(const FieldElement& other){
            rep ^= other.rep;
            return (*this);
        }
        FieldElement& operator-=(const FieldElement& other){
            rep ^= other.rep;
            return (*this);
        }
        FieldElement& operator^=(const FieldElement& other){
            rep ^= other.rep;
            return (*this); // do we really have to define this three times????
        }
        // ++ and -- would not make much sense
        FieldElement& operator*=(const FieldElement& other){
            if(rep==0 || other.rep==0) rep=0;
            else rep = gf_exp[(gf_log[rep]+gf_log[other.rep])%order];
            return (*this);
        }
        FieldElement& operator/=(const FieldElement& other){
            if(rep==0) return FieldElement(0);
            else if(other.rep==0) throw std::invalid_argument("Division by Zero");
            rep = gf_exp[(gf_log[rep]-gf_log[other.rep]+order)%order];
            return (*this);
        }
        void print(ostream& out) const{
            std::stringstream ss;
            ss << std::hex << rep;
            out << ss.str();
        }
    };

    static_assert(GeneratingPolynomial<0x00020000);
    //TODO: make a "static_assert(irreducible(GeneratingPolynomial));"
    private:
    static constexpr Rep gen = generator;
    static constexpr Rep genpoly = GeneratingPolynomial;
    static constexpr Rep degree = calculateDegree(GeneratingPolynomial);
    static constexpr Rep degreeTerm = 1<<degree;
    static constexpr Rep order = degreeTerm-1;
    static constexpr Rep equivDegreeTerm = genpoly^degreeTerm;
    static map<int,Rep> gf_exp; // used std::set to make code more readable
    static map<Rep,int> gf_log; // but should we really use std::set???
    public:
    GF2(){
        Rep a = 1;
        for(int i=0;i<order;++i){
            gf_exp[i] = a;
            gf_log[a] = i;
            a = slowMult(a,gen);
        }
    }
    Rep slowMult(Rep a,Rep b){
        Rep ret=0;
        while(b>0){
            ret += a * (b&1);
            a*=2;
        }
        for(Rep i=DoubleMaxDeg-1-degree;i>=0;--i){
            if ( ret & (degreeTerm << i)) ret ^= (genpoly<<i);
        }
        return ret;
    }
    FieldElement operator()(Rep rep){
        return FieldElement(rep);
    }
};

template<Rep genpoly,Rep gen>
ostream& operator<<(ostream& out,const GF2<genpoly,gen>& x){
    std::stringstream ss;
    ss << "GF2(" << std::hex << genpoly << " " << gen << ")";
    out << ss.str();
    return out;
}

template<Rep genpoly,Rep gen>
ostream& operator<<(ostream& out,const typename GF2<genpoly,gen>::FieldElement& x){
    x.print(); return out;
}

int main(){
    GF2<0x11d,0x02> field;
    GF2<0x11d,0x02>::FieldElement x = field(0x05);
    GF2<0x11d,0x02>::FieldElement y = field(0x03);
    std::cout << field << std::endl;
    std::cout << x*y << std::endl;
    x*=y;
    std::cout << x*y << std::endl;
    x/=y;
    std::cout << x*y << x << y << std::endl;
} // a quick test to see if everything even works.

#include<iostream>
#include<iomanip>
#include<sstream>
#include<array>
#include<type_traits>

using std::array;
using std::ostream;

namespace GF2Four{

    using Rudimentary = int;
    // "Rep" = inner representation for GF2 elements, to be introduces
    // "Rudimentary" = to be used where int is needed but bigger than "Rep".

    template<typename Rep>
    constexpr Rudimentary calculateDegree(Rep arg){
        int ret=-1;
        for(;arg;arg>>=1) ++ret;
        return ret;
    }

    template<typename Rep>
    constexpr Rudimentary degreeTerm(Rep arg){
        return 1<<calculateDegree(arg);
    }

    template<typename Rep>
    constexpr Rep slowMult(Rudimentary genpoly, Rep a, Rep b){
        //truncate longest term from genpoly
        Rep addTerm = (genpoly - degreeTerm(genpoly));
        Rep highestOrderTerm = (degreeTerm(genpoly) / 2);
        Rep ret=0;
        while(b!=0){
            ret ^= a*(b&1);
            b/=2;
            Rep temp = !!( a >= highestOrderTerm ) ;
            a -= highestOrderTerm * temp;
            a *= 2;
            a ^= temp * addTerm;
        }
        return ret;
    }

    template<typename Rep,Rudimentary genpoly>
    constexpr auto makeTables(Rep gen){
        constexpr Rudimentary order = degreeTerm(genpoly) - 1;
        Rep a = 1;
        array<Rep,degreeTerm(genpoly)> expTable{0};
        array<Rep,degreeTerm(genpoly)> divTable{0};
        for(Rep i=0;i<order;++i){
            expTable[i] = a;
            divTable[a] = i;
            a = slowMult(genpoly,a,gen);
        }
        return std::make_pair(expTable,divTable);
    }

    template<Rudimentary genpoly>
    class GF2{
        public:
        using Rep = typename std::conditional<(genpoly < 0x00000200), std::uint8_t, std::uint32_t>::type;

        static_assert(genpoly >= 0x00000010); // no, if you want GF(2), just please use bool
        static_assert(genpoly < 0x00020000);
        
        private:
        constexpr static Rep gen = 2u; //hardcoded for now
        constexpr static Rudimentary order = degreeTerm(genpoly)-1;
        constexpr static auto gf_tables = makeTables<Rep,genpoly>(gen);
        constexpr static auto& gf_exp = gf_tables.first;
        constexpr static auto& gf_log = gf_tables.second;
        //static constexpr auto [gf_exp,gf_log] = makeTables(0x02); //why won't this compile?
        Rep rep;
        
        public:
        GF2(Rep __rep=0): rep{__rep} {}
        inline const GF2 operator+(const GF2& other) const{ return GF2(rep^other.rep); }
        inline const GF2 operator-(const GF2& other) const{ return rep + other; }
        inline const GF2 operator^(const GF2& other) const{ return rep + other; }
        inline const GF2 operator-() const{ return GF2(rep); }
        const GF2 operator*(const GF2& other) const{
            if( !(rep * other.rep) ) return GF2(0);
            return GF2(gf_exp.at((gf_log.at(rep)+gf_log.at(other.rep))%order));
        }
        const GF2 operator/(const GF2& other) const{
            if ( !rep ) return GF2(0);
            else if( !(other.rep) ) throw std::invalid_argument("Division by Zero");
            return GF2(gf_exp.at((gf_log.at(rep)-gf_log.at(other.rep)+order)%order));
        }
        GF2& operator+=(const GF2& other){ rep^=other; return(*this); }
        GF2& operator-=(const GF2& other){ return (rep+=other); }
        GF2& operator^=(const GF2& other){ return (rep+=other); }
        GF2& operator*=(const GF2& other){
            if( !(rep * other.rep) ) rep = 0;
            else rep = gf_exp.at((gf_log.at(rep)+gf_log.at(other.rep))%order);
            return (*this);
        }
        GF2& operator/=(const GF2& other){
            if( !rep ) return (*this);
            else if (!other.rep ) throw std::invalid_argument("Division by Zero");
            rep = gf_exp.at((gf_log.at(rep)-gf_log.at(other.rep)+order)%order);
            return (*this);
        }
        void print(ostream& out) const{
            std::stringstream ss;
            ss << std::hex << (unsigned int)rep;
            out << ss.str();
        }
    };
    
    template<Rudimentary genpoly>
    ostream& operator<<(ostream& out,const GF2<genpoly>& x){
        x.print(out); return out;
    }
}

/*
int main(){
    constexpr GF2Four::Rudimentary poly = 0x11d;
    GF2Four::GF2<poly> x(0x05), y(0x03);
    std::cout << typeid(GF2Four::GF2<poly>::Rep).name() << std::endl;
    std::cout << "uint8_t : " << typeid(std::uint8_t).name() << " / uint32_t : " << typeid(std::uint32_t).name() << std::endl;
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x*=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x/=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
}*/

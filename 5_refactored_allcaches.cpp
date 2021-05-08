#include<iostream>
#include<iomanip>
#include<sstream>
#include<array>
#include<type_traits>

using std::array;
using std::ostream;

namespace GF2Five{

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
        Rudimentary numberOfElements = degreeTerm(genpoly);
        Rudimentary order = numberOfElements-1;

        array<Rep,degreeTerm(genpoly)> expTable{0};
        array<Rudimentary,degreeTerm(genpoly)> logTable{0};
        Rep a=1;
        for(Rudimentary i=0;i<order;++i){
            expTable[i] = a;
            logTable[a] = i;
            a = slowMult(genpoly,a,gen);
        }

        array<array<Rep,degreeTerm(genpoly)>,degreeTerm(genpoly)> multTable {0};
        array<array<Rep,degreeTerm(genpoly)>,degreeTerm(genpoly)> divTable {0};
        bool overflowedR=false, overflowedL = false;
        for(Rudimentary right=0;right<numberOfElements&&!overflowedR;++right){
            overflowedR = (right +1 < right);
            for(Rudimentary left=0;left<numberOfElements&&!overflowedL;++left){ // is this even OK?
                multTable[left][right] = expTable[(logTable[left]+logTable[right])%order];
                divTable[left][right] = expTable[(order + logTable[left] - logTable[right])%order];
                overflowedL = (left+1 < left);
            }
        }
        return std::make_pair(multTable,divTable);
    }


    template<Rudimentary genpoly>
    class GF2{
        public:
        using Rep = typename std::conditional<(genpoly < 0x00000200), std::uint8_t, std::uint32_t>::type; 
        // uint8_t it is.

        static_assert(genpoly >= 0x00000010); // no, if you want GF(2), just please use bool
        static_assert(genpoly < 0x00000200);
        
        private:
        constexpr static Rep gen = 2u; //hardcoded for now
        constexpr static Rudimentary order = degreeTerm(genpoly)-1;
        constexpr static auto gf_tables = makeTables<Rep,genpoly>(0x02); // hardcoded for now
        constexpr static auto& multTable = gf_tables.first;
        constexpr static auto& divTable = gf_tables.second;
        //static constexpr auto [gf_exp,gf_log] = makeTables(0x02); //why won't this compile?
        Rep rep;
        
        public:
        explicit GF2(Rep __rep=0): rep{__rep} {}
        inline const GF2 operator+(const GF2& other) const{ return GF2(rep^other.rep); }
        inline const GF2 operator-(const GF2& other) const{ return GF2(rep^other.rep); }
        inline const GF2 operator^(const GF2& other) const{ return GF2(rep^other.rep); }
        inline const GF2 operator-() const{ return GF2(rep); }
        const GF2 operator*(const GF2& other) const{
            return GF2(multTable[rep][other.rep]);
        }
        const GF2 operator/(const GF2& other) const{
            if(other.rep==0) throw std::invalid_argument("Division by Zero");
            return GF2(divTable[rep][other.rep]);
        }
        GF2& operator+=(const GF2& other){ rep ^= other.rep; return *this; }
        GF2& operator-=(const GF2& other){ rep ^= other.rep; return *this; }
        GF2& operator^=(const GF2& other){ rep ^= other.rep; return *this; }
        GF2& operator*=(const GF2& other){
            rep = multTable[rep][other.rep]; return *this;
        }
        GF2& operator/=(const GF2& other){
            if (!other.rep ) throw std::invalid_argument("Division by Zero");
            rep = divTable[rep][other.rep]; return *this;
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
    constexpr GF2Five::Rudimentary poly = 0x11d;
    GF2Five::GF2<poly> x(0x05), y(0x03);
    std::cout << typeid(GF2Five::GF2<poly>::Rep).name() << std::endl;
    std::cout << "uint8_t : " << typeid(std::uint8_t).name() << " / uint32_t : " << typeid(std::uint32_t).name() << std::endl;
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x*=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
    x/=y;
    std::cout << std::setw(2) << x*y << ' ' << std::setw(2)<<x << ' ' << std::setw(2)<<y << std::endl;
}*/

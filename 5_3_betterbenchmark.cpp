#include"5_refactored_allcaches.cpp"
#include<vector>
#include<random>
#include<chrono>
using namespace std::chrono;
using std::vector;
using std::size_t;
using std::cout;
using std::endl;
constexpr int genpoly = 0x11d;
using GF2 = GF2Five::GF2<genpoly>;
//function for benchmarking.
template<class Tp>
inline __attribute__((always_inline)) void DoNotOptimize(Tp const& value){
    // prevents calculation skip by putting value on register once,
    // this is an "observable event"
    // order of calculation cannot be changed before/after a call to DoNotOptimize
    asm volatile("" : :  "r,m"(value) : "memory");
} 

auto makeRandomInts(int howmany){
    vector<int> ret(howmany);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1,214800000);
    for(size_t i=0;i<howmany;++i){
        ret[i]= dis(gen);
        if(ret[i]%2==0) ret[i]--;
    }
    return ret;
}

auto makeRandomPairs(int howmany){
    vector<GF2> ret(howmany);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1,255);
    for(size_t i=0;i<howmany;++i){
        ret[i] = GF2(dis(gen));
    }
    return ret; // is RVO applied here?
}

int main(){
    int howmany = 20000000;
    auto testees = makeRandomPairs(howmany);
    auto randomints = makeRandomInts(howmany);
    cout << "5_2" <<endl;
    GF2 sum {GF2(1)}; // TODO: make constructors explicit. ok?
    // benchmarking starts here.
    // see https://stackoverflow.com/a/38025837
    auto a = system_clock::now();
    DoNotOptimize(sum);
    for(size_t i=0;i<howmany;i++){
        sum *= testees[i];
    }
    DoNotOptimize(sum);
    auto b = system_clock::now();

    cout << "result(not unrolled): " << sum << endl;
    cout << duration_cast<microseconds>(b-a).count() << " us (loop not unrolled)" << endl;

    GF2 sum1 {GF2(1)};
    GF2 sum2 {GF2(1)};
    GF2 sum3 {GF2(1)};
    GF2 sum4 {GF2(1)};

    auto aa = system_clock::now();
    DoNotOptimize(sum1); DoNotOptimize(sum2); DoNotOptimize(sum3); DoNotOptimize(sum4);
    for(size_t i=0;i<howmany;i+=4){
        sum1 *= testees[i];
        sum2 *= testees[i+1];
        sum3 *= testees[i+2];
        sum4 *= testees[i+3];
    }

    sum1 = sum1*sum2*sum3*sum4;
    DoNotOptimize(sum1);
    auto bb = system_clock::now();
    cout << "result(should be the same as above): " << sum1 << endl;
    cout << duration_cast<microseconds>(bb-aa).count() << " us (loop unrolled manually)" << endl;

    cout << endl << "compared to int multiplications:" << endl << endl;

    int sumsum = 1;
    auto c = system_clock::now();
    DoNotOptimize(sumsum);
    for(size_t i=0;i<howmany;i++){
        sumsum *= randomints[i];
    }
    DoNotOptimize(sumsum);
    auto d = system_clock::now();
    cout << "result(not unrolled):" << sumsum << endl;
    cout << duration_cast<microseconds>(d-c).count() << " us (loop not unrolled)"<< endl;


    int sumsum1=1,sumsum2=1,sumsum3=1,sumsum4=1;
    auto cc = system_clock::now();
    DoNotOptimize(sumsum1);  DoNotOptimize(sumsum2); DoNotOptimize(sumsum3); DoNotOptimize(sumsum4);
    for(size_t i=0;i<howmany;i+=4){
        sumsum1 *= randomints[i];
        sumsum2 *= randomints[i+1];
        sumsum3 *= randomints[i+2];
        sumsum4 *= randomints[i+3];
    }

    sumsum1 = sumsum1*sumsum2*sumsum3*sumsum4;
    DoNotOptimize(sumsum1);
    auto dd = system_clock::now();
    cout << "result(should be the same as above):" << sumsum1 << endl;
    cout << duration_cast<microseconds>(dd-cc).count() << " us (loop unrolled manually)" << endl;

}

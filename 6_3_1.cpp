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

    GF2 sum {GF2(1)}; // TODO: make constructors explicit. ok?
    // benchmarking starts here.
    // see https://stackoverflow.com/a/38025837
    auto a = system_clock::now();
    for(size_t i=0;i<howmany;i++){
        sum *= testees[i];
    }
    auto b = system_clock::now();

    cout << "result(not unrolled): " << sum << endl;
    cout << duration_cast<microseconds>(b-a).count() << " us (loop not unrolled)" << endl;
}

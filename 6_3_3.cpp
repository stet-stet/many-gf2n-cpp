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

    GF2 sum1 {GF2(1)};
    GF2 sum2 {GF2(1)};
    GF2 sum3 {GF2(1)};
    GF2 sum4 {GF2(1)};
    GF2 sum5 {GF2(1)};
    GF2 sum6 {GF2(1)};
    GF2 sum7 {GF2(1)};
    GF2 sum8 {GF2(1)};

    auto aa = system_clock::now();
    for(size_t i=0;i<howmany;i+=8){
        sum1 *= testees[i];
        sum2 *= testees[i+1];
        sum3 *= testees[i+2];
        sum4 *= testees[i+3];
	sum5 *= testees[i+4];
	sum6 *= testees[i+5];
	sum7 *= testees[i+6];
	sum8 *= testees[i+7];
    }

    sum1 = sum1*sum2*sum3*sum4*sum5*sum6*sum7*sum8;
    auto bb = system_clock::now();
    cout << "result(should be the same as above): " << sum1 << endl;
    cout << duration_cast<microseconds>(bb-aa).count() << " us (loop unrolled manually)" << endl;
}

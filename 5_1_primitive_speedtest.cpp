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
    vector<std::pair<int,int>> ret(howmany);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0,214800000);
    for(size_t i=0;i<howmany;++i){
        ret[i].first = dis(gen);
        ret[i].second = dis(gen);
    }
    return ret;
}

auto makeRandomPairs(int howmany){
    vector<std::pair<GF2,GF2>> ret(howmany);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0,255);
    for(size_t i=0;i<howmany;++i){
        ret[i].first = GF2(dis(gen));
        ret[i].second = GF2(dis(gen));
    }
    return ret; // is RVO applied here?
}

int main(){
    int howmany = 20000000;
    auto testees = makeRandomPairs(howmany);
    auto randomints = makeRandomInts(howmany);
    GF2 sum {0};
    auto a = system_clock::now(); // is it a good idea to use system_clock?

    for(auto&& x:testees){
        sum += (x.first * x.second);
    }

    auto b = system_clock::now();

    cout << "result: " << sum << endl; //takes much less time, but why is the result 0?
    cout << duration_cast<microseconds>(b-a).count() << endl;

    cout << endl << "compared to:" << endl << endl;

    auto c = system_clock::now();
    int sumsum = 0;

    for(auto&& x:randomints){
        sumsum += (x.first*x.second);
    }

    auto d = system_clock::now();
    cout << "result:" << sumsum << endl;
    cout << howmany << " int multiplications taking:" << duration_cast<microseconds>(d-c).count() << endl;

    //Results: multiplication for GF2<0x11d> takes about 3x time than for ints. why?
}

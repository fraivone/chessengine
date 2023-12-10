// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
// #include <immintrin.h>

using Vector=std::vector<int>;

// Vector simAdd(const Vector& a, const Vector& b){
//     Vector result(a.size());

//     constexpr auto FLOATS_IN_AVX_REGISTER = 8u;
//     const auto itemsWithSIMD = (a.size()/ FLOATS_IN_AVX_REGISTER) * FLOATS_IN_AVX_REGISTER;

//     auto i = 0u;
//     // vectorized addition
//     for(; i<itemsWithSIMD; i+=FLOATS_IN_AVX_REGISTER){
//         // load from memory to the specific SMID register
//         // u stands for unalligned data
//         // ps stands for single precision (float)
//         auto aRegister = _mm256_loadu_ps(a.data()+1);
//         auto bRegister = _mm256_loadu_ps(b.data()+1);

//         auto sum = _mm256_add_ps(aRegister, bRegister);

//         // store "sum" in the result vector
//         // that's defined in the memory
//         _mm256_storeu_ps(result.data() + i, sum);
//     }

//     return result;
// }

Vector scalarAdd(const Vector& a, const Vector& b){
    Vector result(a.size());

    for(int i =0; i<result.size(); i++)
        result[i] = a[i] + b[i];

    return result;    
}


int main(){

    using namespace std;
    Vector a(17,1U);
    int N = 100000;

    chrono::steady_clock::time_point begin,end;
    begin = chrono::steady_clock::now();
    // for(int j =0; j < N; j++)
    //     auto result = simAdd(a,a);
    end = chrono::steady_clock::now();
    cout<<"SIMD Took "<<chrono::duration_cast<chrono::microseconds>(end - begin).count()/1000000.<<endl;


    begin = chrono::steady_clock::now();
    for(int j =0; j < N; j++)
        auto result = scalarAdd(a,a);
    end = chrono::steady_clock::now();
    cout<<"scalarAdd Took "<<chrono::duration_cast<chrono::microseconds>(end - begin).count()/1000000.<<endl;

    
    return 1;

}




// int main(int argc, char **argv) {

//     std::chrono::steady_clock::time_point begin,end;
//     begin = std::chrono::steady_clock::now();
//     int t = 0;
//     for(int j =0; j < 100000000; j++)
//         t++;

//     end = std::chrono::steady_clock::now();
//     std::cout<<"Took "<<std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.<<std::endl;
    
//     return 1;
// };
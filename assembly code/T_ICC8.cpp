#include <https://raw.githubusercontent.com/vectorclass/version2/master/vectorclass.h>
#include <array>
#include <vector>

using V = Vec8f;
using T = float;
constexpr static int N = 1;
constexpr int M = 8;

using VBlock = std::array<V, V::size()>;
alignas(64) std::array<VBlock, N> Inputs, Outputs;
alignas(64) std::array<V, V::size()> V_Inputs, V_Outputs;
V v_inputs, v_outputs;

V _h1, _h2;
V _rd0_22, _rd0_12, _rd0_21, _rd0_11, _rd1_22, _rd1_12, _rd1_21, _rd1_11, _rd2_22, _rd2_12, _rd2_21, _rd2_11, _rd3_22, _rd3_12, _rd3_21, _rd3_11;

template<typename V> class Shift{

    private:

        V _buffer{0};

    public:

        inline void shift(const T x){ 

            _buffer = blend8<1,2,3,4,5,6,7,8>(_buffer, x);
            
        };

        inline void shift(const V xv){ _buffer = xv; };

        inline T operator[](const int idx){ return (idx < 0) ? _buffer[M+idx] : _buffer[idx]; };

};

Shift<V> _S;

inline std::array<V,M> T_ICC(const std::array<V,M>& w){

    std::array<V,M> y;

    // step 1: initialization
    y[M-2] = mul_add(_rd0_22, _S[-2], w[M-2]);
    y[M-2] = mul_add(_rd0_12, _S[-1], y[M-2]);
    y[M-1] = mul_add(_rd0_21, _S[-2], w[M-1]);
    y[M-1] = mul_add(_rd0_11, _S[-1], y[M-1]);

    // step 2: first recursion
    V tmp2 = permute8<-1,0,-1,2,-1,4,-1,6>(y[M-2]);
    V tmp1 = permute8<-1,0,-1,2,-1,4,-1,6>(y[M-1]);

    y[M-2] = mul_add(tmp2, _rd1_22, y[M-2]);
    y[M-2] = mul_add(tmp1, _rd1_12, y[M-2]);
    y[M-1] = mul_add(tmp2, _rd1_21, y[M-1]);
    y[M-1] = mul_add(tmp1, _rd1_11, y[M-1]);

    // step 3: second recursion
    tmp2 = permute8<-1,-1,1,1,-1,-1,5,5>(y[M-2]);
    tmp1 = permute8<-1,-1,1,1,-1,-1,5,5>(y[M-1]);

    y[M-2] = mul_add(tmp2, _rd2_22, y[M-2]);
    y[M-2] = mul_add(tmp1, _rd2_12, y[M-2]);
    y[M-1] = mul_add(tmp2, _rd2_21, y[M-1]);
    y[M-1] = mul_add(tmp1, _rd2_11, y[M-1]);

    // step 4: third recursion
    tmp2 = permute8<-1,-1,-1,-1,3,3,3,3>(y[M-2]);
    tmp1 = permute8<-1,-1,-1,-1,3,3,3,3>(y[M-1]);

    y[M-2] = mul_add(tmp2, _rd3_22, y[M-2]);
    y[M-2] = mul_add(tmp1, _rd3_12, y[M-2]);
    y[M-1] = mul_add(tmp2, _rd3_21, y[M-1]);
    y[M-1] = mul_add(tmp1, _rd3_11, y[M-1]);

    // shuffle and forward
    V yi2 = blend8<8,0,1,2,3,4,5,6>(y[M-2], _S[-2]);
    V yi1 = blend8<8,0,1,2,3,4,5,6>(y[M-1], _S[-1]);

    for (auto i=0; i<M-2; i++){
        
        y[i] = mul_add(yi2, _h2[i], w[i]);
        y[i] = mul_add(yi1, _h1[i], y[i]);

    };

    // _S.shift(y[M-2][M-1]);
    // _S.shift(y[M-1][M-1]); 
    
    return y; 

};

int main(){

    for (auto n=0; n<N; n++) {

        Outputs[n] = T_ICC(Inputs[n]);

    }

    return 0;

}



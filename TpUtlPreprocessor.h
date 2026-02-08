#pragma once

//#define TP_PP_DT_STR(x) #x
//#define TP_PP_STR(x) TP_PP_DT_STR(x)

#define TP_PP_SELF(x) x
#define TP_PP_EMPTY(x)
#define TP_PP_EXPAND(x) x


//#define TP_PP_DT_COUNT(A,B,C,D,RET,...) RET
#define TP_PP_DT_COUNT(A,B,C,D,E,F,G,H,RET,...) RET
//#define TP_PP_COUNT(...) TP_PP_DT_COUNT(__VA_ARGS__,4,3,2,1,0)
#define TP_PP_COUNT(...) TP_PP_DT_COUNT(__VA_ARGS__,8,7,6,5,4,3,2,1,0)



#define TP_PP_DT_SECRET_CAT_IMPL(A,B) A##B
#define TP_PP_DT_SECRET_CAT(A,B) TP_PP_DT_SECRET_CAT_IMPL(A,B)

#define TP_PP_DT_CAT_2(A,B)     A##B
#define TP_PP_CAT_0()
#define TP_PP_CAT_1(A)       A
#define TP_PP_CAT_2(A,B)     TP_PP_DT_CAT_2(A,B)
#define TP_PP_CAT_3(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_2(__VA_ARGS__))
#define TP_PP_CAT_4(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_3(__VA_ARGS__))
#define TP_PP_CAT_5(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_4(__VA_ARGS__))
#define TP_PP_CAT_6(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_5(__VA_ARGS__))
#define TP_PP_CAT_7(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_6(__VA_ARGS__))
#define TP_PP_CAT_8(A,...)   TP_PP_CAT_2(A,TP_PP_CAT_7(__VA_ARGS__))

#define TP_PP_DT_CAT(...) TP_PP_DT_SECRET_CAT(TP_PP_CAT_, TP_PP_COUNT(__VA_ARGS__) )(__VA_ARGS__)
#define TP_PP_CAT(...) TP_PP_DT_CAT(__VA_ARGS__)


#define TP_PP_STR_0()
#define TP_PP_STR_1(A) #A
#define TP_PP_STR_2(B,...) TP_PP_STR_1(B), TP_PP_STR_1(__VA_ARGS__)
#define TP_PP_STR_3(C,...) TP_PP_STR_1(C), TP_PP_STR_2(__VA_ARGS__)
#define TP_PP_STR_4(D,...) TP_PP_STR_1(D), TP_PP_STR_3(__VA_ARGS__)
#define TP_PP_STR_5(D,...) TP_PP_STR_1(D), TP_PP_STR_4(__VA_ARGS__)
#define TP_PP_STR_6(D,...) TP_PP_STR_1(D), TP_PP_STR_5(__VA_ARGS__)
#define TP_PP_STR_7(D,...) TP_PP_STR_1(D), TP_PP_STR_6(__VA_ARGS__)
#define TP_PP_STR_8(D,...) TP_PP_STR_1(D), TP_PP_STR_7(__VA_ARGS__)

#define TP_PP_DT_STR(...) TP_PP_CAT_2(TP_PP_STR_, TP_PP_COUNT(__VA_ARGS__) )(__VA_ARGS__)
#define TP_PP_STR(...) TP_PP_DT_STR(__VA_ARGS__)


#define TP_PP_CALL_1_0(fnc)
#define TP_PP_CALL_1_1(fnc,A)     fnc(A);
#define TP_PP_CALL_1_2(fnc,A,...) fnc(A); TP_PP_CALL_1_1(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_3(fnc,A,...) fnc(A); TP_PP_CALL_1_2(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_4(fnc,A,...) fnc(A); TP_PP_CALL_1_3(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_5(fnc,A,...) fnc(A); TP_PP_CALL_1_4(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_6(fnc,A,...) fnc(A); TP_PP_CALL_1_5(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_7(fnc,A,...) fnc(A); TP_PP_CALL_1_6(fnc,__VA_ARGS__)
#define TP_PP_CALL_1_8(fnc,A,...) fnc(A); TP_PP_CALL_1_7(fnc,__VA_ARGS__)

#define TP_PP_DT_CALL_1(FNC,...) TP_PP_CAT_2(TP_PP_CALL_1_, TP_PP_COUNT(__VA_ARGS__) )(FNC,__VA_ARGS__)
#define TP_PP_CALL_1(FNC,...) TP_PP_DT_CALL_1(FNC,__VA_ARGS__)
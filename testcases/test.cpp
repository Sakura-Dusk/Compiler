#include "../src/parser/parser.h"
#include "../src/AST/node/basic.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../testcases/testcases/"+path;
    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    fclose(stdin);
    std::cin.clear();
    code.push_back('\n');
    return code;
}

void runParser(std::string path)
{
    const auto code=openFile(path);
    Parser(code).work();
}
TEST(IR1, misc1) {
    EXPECT_NO_THROW(runParser("misc1.in"));
}
TEST(semantic1, type8) {
    EXPECT_ANY_THROW(runParser("type8.in"));
}
TEST(semantic1, misc14) {
    EXPECT_NO_THROW(runParser("misc14.in"));
}
TEST(semantic1, if11) {
    EXPECT_ANY_THROW(runParser("if11.in"));
}
TEST(semantic1, array8) {
    EXPECT_ANY_THROW(runParser("array8.in"));
}
TEST(semantic1, basic4) {
    EXPECT_NO_THROW(runParser("basic4.in"));
}
TEST(semantic1, basic3) {
    EXPECT_ANY_THROW(runParser("basic3.in"));
}
TEST(semantic1, basic1) {
    EXPECT_NO_THROW(runParser("basic1.in"));
}
TEST(semantic1, basic12) {
    EXPECT_ANY_THROW(runParser("basic12.in"));
}
TEST(semantic1, expr10) {
    EXPECT_ANY_THROW(runParser("expr10.in"));
}
TEST(semantic1, expr36) {
    EXPECT_NO_THROW(runParser("expr36.in"));
}
TEST(semantic1, expr7) {
    EXPECT_ANY_THROW(runParser("expr7.in"));
}
TEST(semantic1, misc34) {
    EXPECT_NO_THROW(runParser("misc34.in"));
}
TEST(semantic1, expr32) {
    EXPECT_ANY_THROW(runParser("expr32.in"));
}
TEST(semantic1, misc7) {
    EXPECT_NO_THROW(runParser("misc7.in"));
}
TEST(semantic1, expr31) {
    EXPECT_ANY_THROW(runParser("expr31.in"));
}
TEST(semantic1, basic37) {
    EXPECT_NO_THROW(runParser("basic37.in"));
}
TEST(semantic1, misc36) {
    EXPECT_NO_THROW(runParser("misc36.in"));
}
TEST(semantic1, if7) {
    EXPECT_NO_THROW(runParser("if7.in"));
}
TEST(semantic1, array4) {
    EXPECT_ANY_THROW(runParser("array4.in"));
}
TEST(semantic1, misc24) {
    EXPECT_NO_THROW(runParser("misc24.in"));
}
TEST(semantic1, expr38) {
    EXPECT_NO_THROW(runParser("expr38.in"));
}
TEST(semantic1, expr28) {
    EXPECT_ANY_THROW(runParser("expr28.in"));
}
TEST(semantic1, basic21) {
    EXPECT_NO_THROW(runParser("basic21.in"));
}
TEST(semantic1, expr12) {
    EXPECT_ANY_THROW(runParser("expr12.in"));
}
TEST(semantic1, expr40) {
    EXPECT_ANY_THROW(runParser("expr40.in"));
}
TEST(semantic1, basic11) {
    EXPECT_NO_THROW(runParser("basic11.in"));
}
TEST(semantic1, expr16) {
    EXPECT_ANY_THROW(runParser("expr16.in"));
}
TEST(semantic1, basic28) {
    EXPECT_ANY_THROW(runParser("basic28.in"));
}
TEST(semantic1, type18) {
    EXPECT_ANY_THROW(runParser("type18.in"));
}
TEST(semantic1, basic20) {
    EXPECT_NO_THROW(runParser("basic20.in"));
}
TEST(semantic1, misc1) {
    EXPECT_NO_THROW(runParser("misc1.in"));
}
TEST(semantic1, type4) {
    EXPECT_ANY_THROW(runParser("type4.in"));
}
TEST(semantic1, misc26) {
    EXPECT_NO_THROW(runParser("misc26.in"));
}
TEST(semantic1, misc57) {
    EXPECT_ANY_THROW(runParser("misc57.in"));
}
TEST(semantic1, expr33) {
    EXPECT_NO_THROW(runParser("expr33.in"));
}
TEST(semantic1, misc12) {
    EXPECT_NO_THROW(runParser("misc12.in"));
}
TEST(semantic1, misc47) {
    EXPECT_NO_THROW(runParser("misc47.in"));
}
TEST(semantic1, return1) {
    EXPECT_ANY_THROW(runParser("return1.in"));
}
TEST(semantic1, misc60) {
    EXPECT_ANY_THROW(runParser("misc60.in"));
}
TEST(semantic1, return4) {
    EXPECT_ANY_THROW(runParser("return4.in"));
}
TEST(semantic1, basic31) {
    EXPECT_ANY_THROW(runParser("basic31.in"));
}
TEST(semantic1, misc53) {
    EXPECT_NO_THROW(runParser("misc53.in"));
}
TEST(semantic1, misc38) {
    EXPECT_NO_THROW(runParser("misc38.in"));
}
TEST(semantic1, misc6) {
    EXPECT_NO_THROW(runParser("misc6.in"));
}
TEST(semantic1, basic27) {
    EXPECT_NO_THROW(runParser("basic27.in"));
}
TEST(semantic1, expr29) {
    EXPECT_ANY_THROW(runParser("expr29.in"));
}
TEST(semantic1, autoref9) {
    EXPECT_NO_THROW(runParser("autoref9.in"));
}
TEST(semantic1, return8) {
    EXPECT_NO_THROW(runParser("return8.in"));
}
TEST(semantic1, basic6) {
    EXPECT_ANY_THROW(runParser("basic6.in"));
}
TEST(semantic1, autoref2) {
    EXPECT_NO_THROW(runParser("autoref2.in"));
}
TEST(semantic1, expr18) {
    EXPECT_ANY_THROW(runParser("expr18.in"));
}
TEST(semantic1, if9) {
    EXPECT_NO_THROW(runParser("if9.in"));
}
TEST(semantic1, basic32) {
    EXPECT_ANY_THROW(runParser("basic32.in"));
}
TEST(semantic1, misc9) {
    EXPECT_NO_THROW(runParser("misc9.in"));
}
TEST(semantic1, type9) {
    EXPECT_ANY_THROW(runParser("type9.in"));
}
TEST(semantic1, return2) {
    EXPECT_NO_THROW(runParser("return2.in"));
}
TEST(semantic1, type2) {
    EXPECT_ANY_THROW(runParser("type2.in"));
}
TEST(semantic1, misc18) {
    EXPECT_NO_THROW(runParser("misc18.in"));
}
TEST(semantic1, misc65) {
    EXPECT_NO_THROW(runParser("misc65.in"));
}
TEST(semantic1, autoref6) {
    EXPECT_NO_THROW(runParser("autoref6.in"));
}
TEST(semantic1, return11) {
    EXPECT_ANY_THROW(runParser("return11.in"));
}
TEST(semantic1, basic17) {
    EXPECT_NO_THROW(runParser("basic17.in"));
}
TEST(semantic1, expr24) {
    EXPECT_ANY_THROW(runParser("expr24.in"));
}
TEST(semantic1, autoref3) {
    EXPECT_NO_THROW(runParser("autoref3.in"));
}
TEST(semantic1, misc30) {
    EXPECT_NO_THROW(runParser("misc30.in"));
}
TEST(semantic1, misc31) {
    EXPECT_NO_THROW(runParser("misc31.in"));
}
TEST(semantic1, if14) {
    EXPECT_ANY_THROW(runParser("if14.in"));
}
TEST(semantic1, expr34) {
    EXPECT_NO_THROW(runParser("expr34.in"));
}
TEST(semantic1, if15) {
    EXPECT_ANY_THROW(runParser("if15.in"));
}
TEST(semantic1, misc33) {
    EXPECT_NO_THROW(runParser("misc33.in"));
}
TEST(semantic1, expr9) {
    EXPECT_ANY_THROW(runParser("expr9.in"));
}
TEST(semantic1, type20) {
    EXPECT_ANY_THROW(runParser("type20.in"));
}
TEST(semantic1, array3) {
    EXPECT_NO_THROW(runParser("array3.in"));
}
TEST(semantic1, misc16) {
    EXPECT_NO_THROW(runParser("misc16.in"));
}
TEST(semantic1, type17) {
    EXPECT_ANY_THROW(runParser("type17.in"));
}
TEST(semantic1, loop5) {
    EXPECT_NO_THROW(runParser("loop5.in"));
}
TEST(semantic1, array6) {
    EXPECT_ANY_THROW(runParser("array6.in"));
}
TEST(semantic1, loop3) {
    EXPECT_NO_THROW(runParser("loop3.in"));
}
TEST(semantic1, basic40) {
    EXPECT_NO_THROW(runParser("basic40.in"));
}
TEST(semantic1, misc52) {
    EXPECT_NO_THROW(runParser("misc52.in"));
}
TEST(semantic1, if4) {
    EXPECT_NO_THROW(runParser("if4.in"));
}
TEST(semantic1, misc51) {
    EXPECT_NO_THROW(runParser("misc51.in"));
}
TEST(semantic1, expr20) {
    EXPECT_ANY_THROW(runParser("expr20.in"));
}
TEST(semantic1, misc58) {
    EXPECT_ANY_THROW(runParser("misc58.in"));
}
TEST(semantic1, array1) {
    EXPECT_NO_THROW(runParser("array1.in"));
}
TEST(semantic1, basic39) {
    EXPECT_NO_THROW(runParser("basic39.in"));
}
TEST(semantic1, return12) {
    EXPECT_NO_THROW(runParser("return12.in"));
}
TEST(semantic1, misc15) {
    EXPECT_NO_THROW(runParser("misc15.in"));
}
TEST(semantic1, misc61) {
    EXPECT_ANY_THROW(runParser("misc61.in"));
}
TEST(semantic1, type1) {
    EXPECT_ANY_THROW(runParser("type1.in"));
}
TEST(semantic1, misc37) {
    EXPECT_NO_THROW(runParser("misc37.in"));
}
TEST(semantic1, expr1) {
    EXPECT_ANY_THROW(runParser("expr1.in"));
}
TEST(semantic1, expr15) {
    EXPECT_ANY_THROW(runParser("expr15.in"));
}
TEST(semantic1, misc11) {
    EXPECT_NO_THROW(runParser("misc11.in"));
}
TEST(semantic1, type6) {
    EXPECT_ANY_THROW(runParser("type6.in"));
}
TEST(semantic1, return14) {
    EXPECT_NO_THROW(runParser("return14.in"));
}
TEST(semantic1, misc4) {
    EXPECT_NO_THROW(runParser("misc4.in"));
}
TEST(semantic1, basic23) {
    EXPECT_NO_THROW(runParser("basic23.in"));
}
TEST(semantic1, loop7) {
    EXPECT_ANY_THROW(runParser("loop7.in"));
}
TEST(semantic1, return6) {
    EXPECT_ANY_THROW(runParser("return6.in"));
}
TEST(semantic1, basic16) {
    EXPECT_ANY_THROW(runParser("basic16.in"));
}
TEST(semantic1, if6) {
    EXPECT_NO_THROW(runParser("if6.in"));
}
TEST(semantic1, return3) {
    EXPECT_ANY_THROW(runParser("return3.in"));
}
TEST(semantic1, basic15) {
    EXPECT_ANY_THROW(runParser("basic15.in"));
}
TEST(semantic1, expr21) {
    EXPECT_ANY_THROW(runParser("expr21.in"));
}
TEST(semantic1, misc40) {
    EXPECT_NO_THROW(runParser("misc40.in"));
}
TEST(semantic1, return5) {
    EXPECT_ANY_THROW(runParser("return5.in"));
}
TEST(semantic1, expr27) {
    EXPECT_ANY_THROW(runParser("expr27.in"));
}
TEST(semantic1, if5) {
    EXPECT_NO_THROW(runParser("if5.in"));
}
TEST(semantic1, if3) {
    EXPECT_NO_THROW(runParser("if3.in"));
}
TEST(semantic1, misc22) {
    EXPECT_NO_THROW(runParser("misc22.in"));
}
TEST(semantic1, autoref1) {
    EXPECT_NO_THROW(runParser("autoref1.in"));
}
TEST(semantic1, expr30) {
    EXPECT_ANY_THROW(runParser("expr30.in"));
}
TEST(semantic1, misc13) {
    EXPECT_NO_THROW(runParser("misc13.in"));
}
TEST(semantic1, if8) {
    EXPECT_NO_THROW(runParser("if8.in"));
}
TEST(semantic1, type7) {
    EXPECT_ANY_THROW(runParser("type7.in"));
}
TEST(semantic1, misc10) {
    EXPECT_NO_THROW(runParser("misc10.in"));
}
TEST(semantic1, basic10) {
    EXPECT_ANY_THROW(runParser("basic10.in"));
}
TEST(semantic1, return15) {
    EXPECT_NO_THROW(runParser("return15.in"));
}
TEST(semantic1, type12) {
    EXPECT_ANY_THROW(runParser("type12.in"));
}
TEST(semantic1, misc27) {
    EXPECT_NO_THROW(runParser("misc27.in"));
}
TEST(semantic1, misc25) {
    EXPECT_NO_THROW(runParser("misc25.in"));
}
TEST(semantic1, misc19) {
    EXPECT_NO_THROW(runParser("misc19.in"));
}
TEST(semantic1, basic29) {
    EXPECT_NO_THROW(runParser("basic29.in"));
}
TEST(semantic1, misc39) {
    EXPECT_NO_THROW(runParser("misc39.in"));
}
TEST(semantic1, expr22) {
    EXPECT_ANY_THROW(runParser("expr22.in"));
}
TEST(semantic1, if12) {
    EXPECT_ANY_THROW(runParser("if12.in"));
}
TEST(semantic1, misc8) {
    EXPECT_NO_THROW(runParser("misc8.in"));
}
TEST(semantic1, type19) {
    EXPECT_ANY_THROW(runParser("type19.in"));
}
TEST(semantic1, basic35) {
    EXPECT_ANY_THROW(runParser("basic35.in"));
}
TEST(semantic1, loop10) {
    EXPECT_ANY_THROW(runParser("loop10.in"));
}
TEST(semantic1, return13) {
    EXPECT_NO_THROW(runParser("return13.in"));
}
TEST(semantic1, expr35) {
    EXPECT_ANY_THROW(runParser("expr35.in"));
}
TEST(semantic1, basic13) {
    EXPECT_ANY_THROW(runParser("basic13.in"));
}
TEST(semantic1, expr25) {
    EXPECT_ANY_THROW(runParser("expr25.in"));
}
TEST(semantic1, misc29) {
    EXPECT_NO_THROW(runParser("misc29.in"));
}
TEST(semantic1, misc64) {
    EXPECT_ANY_THROW(runParser("misc64.in"));
}
TEST(semantic1, misc23) {
    EXPECT_NO_THROW(runParser("misc23.in"));
}
TEST(semantic1, misc43) {
    EXPECT_NO_THROW(runParser("misc43.in"));
}
TEST(semantic1, misc55) {
    EXPECT_NO_THROW(runParser("misc55.in"));
}
TEST(semantic1, expr4) {
    EXPECT_ANY_THROW(runParser("expr4.in"));
}
TEST(semantic1, loop8) {
    EXPECT_ANY_THROW(runParser("loop8.in"));
}
TEST(semantic1, basic5) {
    EXPECT_ANY_THROW(runParser("basic5.in"));
}
TEST(semantic1, expr37) {
    EXPECT_ANY_THROW(runParser("expr37.in"));
}
TEST(semantic1, misc50) {
    EXPECT_NO_THROW(runParser("misc50.in"));
}
TEST(semantic1, if10) {
    EXPECT_NO_THROW(runParser("if10.in"));
}
TEST(semantic1, misc41) {
    EXPECT_NO_THROW(runParser("misc41.in"));
}
TEST(semantic1, type10) {
    EXPECT_ANY_THROW(runParser("type10.in"));
}
TEST(semantic1, misc49) {
    EXPECT_NO_THROW(runParser("misc49.in"));
}
TEST(semantic1, basic25) {
    EXPECT_NO_THROW(runParser("basic25.in"));
}
TEST(semantic1, basic18) {
    EXPECT_NO_THROW(runParser("basic18.in"));
}
TEST(semantic1, array2) {
    EXPECT_NO_THROW(runParser("array2.in"));
}
TEST(semantic1, misc63) {
    EXPECT_ANY_THROW(runParser("misc63.in"));
}
TEST(semantic1, misc3) {
    EXPECT_NO_THROW(runParser("misc3.in"));
}
TEST(semantic1, type11) {
    EXPECT_ANY_THROW(runParser("type11.in"));
}
TEST(semantic1, expr26) {
    EXPECT_ANY_THROW(runParser("expr26.in"));
}
TEST(semantic1, array7) {
    EXPECT_ANY_THROW(runParser("array7.in"));
}
TEST(semantic1, autoref5) {
    EXPECT_NO_THROW(runParser("autoref5.in"));
}
TEST(semantic1, basic8) {
    EXPECT_ANY_THROW(runParser("basic8.in"));
}
TEST(semantic1, expr13) {
    EXPECT_NO_THROW(runParser("expr13.in"));
}
TEST(semantic1, return10) {
    EXPECT_ANY_THROW(runParser("return10.in"));
}
TEST(semantic1, return9) {
    EXPECT_ANY_THROW(runParser("return9.in"));
}
TEST(semantic1, expr11) {
    EXPECT_ANY_THROW(runParser("expr11.in"));
}
TEST(semantic1, misc44) {
    EXPECT_NO_THROW(runParser("misc44.in"));
}
TEST(semantic1, misc5) {
    EXPECT_NO_THROW(runParser("misc5.in"));
}
TEST(semantic1, misc2) {
    EXPECT_NO_THROW(runParser("misc2.in"));
}
TEST(semantic1, expr23) {
    EXPECT_ANY_THROW(runParser("expr23.in"));
}
TEST(semantic1, loop9) {
    EXPECT_ANY_THROW(runParser("loop9.in"));
}
TEST(semantic1, basic24) {
    EXPECT_NO_THROW(runParser("basic24.in"));
}
TEST(semantic1, if1) {
    EXPECT_NO_THROW(runParser("if1.in"));
}
TEST(semantic1, basic26) {
    EXPECT_NO_THROW(runParser("basic26.in"));
}
TEST(semantic1, expr6) {
    EXPECT_ANY_THROW(runParser("expr6.in"));
}
TEST(semantic1, autoref7) {
    EXPECT_ANY_THROW(runParser("autoref7.in"));
}
TEST(semantic1, type14) {
    EXPECT_ANY_THROW(runParser("type14.in"));
}
TEST(semantic1, loop4) {
    EXPECT_NO_THROW(runParser("loop4.in"));
}
TEST(semantic1, return7) {
    EXPECT_ANY_THROW(runParser("return7.in"));
}
TEST(semantic1, array5) {
    EXPECT_ANY_THROW(runParser("array5.in"));
}
TEST(semantic1, misc48) {
    EXPECT_NO_THROW(runParser("misc48.in"));
}
TEST(semantic1, expr3) {
    EXPECT_ANY_THROW(runParser("expr3.in"));
}
TEST(semantic1, expr8) {
    EXPECT_ANY_THROW(runParser("expr8.in"));
}
TEST(semantic1, basic33) {
    EXPECT_ANY_THROW(runParser("basic33.in"));
}
TEST(semantic1, basic19) {
    EXPECT_NO_THROW(runParser("basic19.in"));
}
TEST(semantic1, basic14) {
    EXPECT_ANY_THROW(runParser("basic14.in"));
}
TEST(semantic1, expr17) {
    EXPECT_NO_THROW(runParser("expr17.in"));
}
TEST(semantic1, misc17) {
    EXPECT_NO_THROW(runParser("misc17.in"));
}
TEST(semantic1, misc59) {
    EXPECT_ANY_THROW(runParser("misc59.in"));
}
TEST(semantic1, misc32) {
    EXPECT_NO_THROW(runParser("misc32.in"));
}
TEST(semantic1, type15) {
    EXPECT_ANY_THROW(runParser("type15.in"));
}
TEST(semantic1, basic38) {
    EXPECT_ANY_THROW(runParser("basic38.in"));
}
TEST(semantic1, misc20) {
    EXPECT_NO_THROW(runParser("misc20.in"));
}
TEST(semantic1, basic22) {
    EXPECT_NO_THROW(runParser("basic22.in"));
}
TEST(semantic1, expr19) {
    EXPECT_NO_THROW(runParser("expr19.in"));
}
TEST(semantic1, basic30) {
    EXPECT_ANY_THROW(runParser("basic30.in"));
}
TEST(semantic1, loop6) {
    EXPECT_ANY_THROW(runParser("loop6.in"));
}
TEST(semantic1, misc62) {
    EXPECT_ANY_THROW(runParser("misc62.in"));
}
TEST(semantic1, expr39) {
    EXPECT_ANY_THROW(runParser("expr39.in"));
}
TEST(semantic1, loop2) {
    EXPECT_NO_THROW(runParser("loop2.in"));
}
TEST(semantic1, loop1) {
    EXPECT_NO_THROW(runParser("loop1.in"));
}
TEST(semantic1, misc45) {
    EXPECT_NO_THROW(runParser("misc45.in"));
}
TEST(semantic1, autoref4) {
    EXPECT_NO_THROW(runParser("autoref4.in"));
}
TEST(semantic1, misc56) {
    EXPECT_ANY_THROW(runParser("misc56.in"));
}
TEST(semantic1, if13) {
    EXPECT_ANY_THROW(runParser("if13.in"));
}
TEST(semantic1, type3) {
    EXPECT_ANY_THROW(runParser("type3.in"));
}
TEST(semantic1, misc54) {
    EXPECT_NO_THROW(runParser("misc54.in"));
}
TEST(semantic1, expr2) {
    EXPECT_ANY_THROW(runParser("expr2.in"));
}
TEST(semantic1, misc42) {
    EXPECT_NO_THROW(runParser("misc42.in"));
}
TEST(semantic1, type5) {
    EXPECT_ANY_THROW(runParser("type5.in"));
}
TEST(semantic1, type16) {
    EXPECT_ANY_THROW(runParser("type16.in"));
}
TEST(semantic1, type13) {
    EXPECT_ANY_THROW(runParser("type13.in"));
}
TEST(semantic1, if2) {
    EXPECT_NO_THROW(runParser("if2.in"));
}
TEST(semantic1, basic34) {
    EXPECT_ANY_THROW(runParser("basic34.in"));
}
TEST(semantic1, expr14) {
    EXPECT_ANY_THROW(runParser("expr14.in"));
}
TEST(semantic1, expr5) {
    EXPECT_ANY_THROW(runParser("expr5.in"));
}
TEST(semantic1, misc28) {
    EXPECT_NO_THROW(runParser("misc28.in"));
}
TEST(semantic1, basic2) {
    EXPECT_ANY_THROW(runParser("basic2.in"));
}
TEST(semantic1, misc21) {
    EXPECT_NO_THROW(runParser("misc21.in"));
}
TEST(semantic1, misc46) {
    EXPECT_NO_THROW(runParser("misc46.in"));
}
TEST(semantic1, basic9) {
    EXPECT_ANY_THROW(runParser("basic9.in"));
}
TEST(semantic1, basic7) {
    EXPECT_ANY_THROW(runParser("basic7.in"));
}
TEST(semantic1, misc35) {
    EXPECT_NO_THROW(runParser("misc35.in"));
}
TEST(semantic1, basic36) {
    EXPECT_NO_THROW(runParser("basic36.in"));
}
TEST(semantic1, autoref8) {
    EXPECT_ANY_THROW(runParser("autoref8.in"));
}
TEST(semantic2, comprehensive33) {
    EXPECT_NO_THROW(runParser("comprehensive33.in"));
}
TEST(semantic2, comprehensive36) {
    EXPECT_NO_THROW(runParser("comprehensive36.in"));
}
TEST(semantic2, comprehensive49) {
    EXPECT_NO_THROW(runParser("comprehensive49.in"));
}
TEST(semantic2, comprehensive34) {
    EXPECT_NO_THROW(runParser("comprehensive34.in"));
}
TEST(semantic2, comprehensive27) {
    EXPECT_NO_THROW(runParser("comprehensive27.in"));
}
TEST(semantic2, comprehensive21) {
    EXPECT_NO_THROW(runParser("comprehensive21.in"));
}
TEST(semantic2, comprehensive42) {
    EXPECT_NO_THROW(runParser("comprehensive42.in"));
}
TEST(semantic2, comprehensive25) {
    EXPECT_NO_THROW(runParser("comprehensive25.in"));
}
TEST(semantic2, comprehensive7) {
    EXPECT_NO_THROW(runParser("comprehensive7.in"));
}
TEST(semantic2, comprehensive22) {
    EXPECT_NO_THROW(runParser("comprehensive22.in"));
}
TEST(semantic2, comprehensive23) {
    EXPECT_NO_THROW(runParser("comprehensive23.in"));
}
TEST(semantic2, comprehensive41) {
    EXPECT_NO_THROW(runParser("comprehensive41.in"));
}
TEST(semantic2, comprehensive26) {
    EXPECT_NO_THROW(runParser("comprehensive26.in"));
}
TEST(semantic2, comprehensive32) {
    EXPECT_NO_THROW(runParser("comprehensive32.in"));
}
TEST(semantic2, comprehensive5) {
    EXPECT_NO_THROW(runParser("comprehensive5.in"));
}
TEST(semantic2, comprehensive12) {
    EXPECT_NO_THROW(runParser("comprehensive12.in"));
}
TEST(semantic2, comprehensive48) {
    EXPECT_NO_THROW(runParser("comprehensive48.in"));
}
TEST(semantic2, comprehensive19) {
    EXPECT_NO_THROW(runParser("comprehensive19.in"));
}
TEST(semantic2, comprehensive13) {
    EXPECT_NO_THROW(runParser("comprehensive13.in"));
}
TEST(semantic2, comprehensive31) {
    EXPECT_NO_THROW(runParser("comprehensive31.in"));
}
TEST(semantic2, comprehensive38) {
    EXPECT_NO_THROW(runParser("comprehensive38.in"));
}
TEST(semantic2, comprehensive16) {
    EXPECT_NO_THROW(runParser("comprehensive16.in"));
}
TEST(semantic2, comprehensive29) {
    EXPECT_NO_THROW(runParser("comprehensive29.in"));
}
TEST(semantic2, comprehensive17) {
    EXPECT_NO_THROW(runParser("comprehensive17.in"));
}
TEST(semantic2, comprehensive46) {
    EXPECT_NO_THROW(runParser("comprehensive46.in"));
}
TEST(semantic2, comprehensive8) {
    EXPECT_NO_THROW(runParser("comprehensive8.in"));
}
TEST(semantic2, comprehensive2) {
    EXPECT_NO_THROW(runParser("comprehensive2.in"));
}
TEST(semantic2, comprehensive4) {
    EXPECT_NO_THROW(runParser("comprehensive4.in"));
}
TEST(semantic2, comprehensive20) {
    EXPECT_NO_THROW(runParser("comprehensive20.in"));
}
TEST(semantic2, comprehensive11) {
    EXPECT_NO_THROW(runParser("comprehensive11.in"));
}
TEST(semantic2, comprehensive45) {
    EXPECT_NO_THROW(runParser("comprehensive45.in"));
}
TEST(semantic2, comprehensive18) {
    EXPECT_NO_THROW(runParser("comprehensive18.in"));
}
TEST(semantic2, comprehensive35) {
    EXPECT_NO_THROW(runParser("comprehensive35.in"));
}
TEST(semantic2, comprehensive40) {
    EXPECT_NO_THROW(runParser("comprehensive40.in"));
}
TEST(semantic2, comprehensive37) {
    EXPECT_NO_THROW(runParser("comprehensive37.in"));
}
TEST(semantic2, comprehensive6) {
    EXPECT_NO_THROW(runParser("comprehensive6.in"));
}
TEST(semantic2, comprehensive44) {
    EXPECT_NO_THROW(runParser("comprehensive44.in"));
}
TEST(semantic2, comprehensive14) {
    EXPECT_NO_THROW(runParser("comprehensive14.in"));
}
TEST(semantic2, comprehensive30) {
    EXPECT_NO_THROW(runParser("comprehensive30.in"));
}
TEST(semantic2, comprehensive28) {
    EXPECT_NO_THROW(runParser("comprehensive28.in"));
}
TEST(semantic2, comprehensive3) {
    EXPECT_NO_THROW(runParser("comprehensive3.in"));
}
TEST(semantic2, comprehensive9) {
    EXPECT_NO_THROW(runParser("comprehensive9.in"));
}
TEST(semantic2, comprehensive47) {
    EXPECT_NO_THROW(runParser("comprehensive47.in"));
}
TEST(semantic2, comprehensive1) {
    EXPECT_NO_THROW(runParser("comprehensive1.in"));
}
TEST(semantic2, comprehensive15) {
    EXPECT_NO_THROW(runParser("comprehensive15.in"));
}
TEST(semantic2, comprehensive39) {
    EXPECT_NO_THROW(runParser("comprehensive39.in"));
}
TEST(semantic2, comprehensive50) {
    EXPECT_NO_THROW(runParser("comprehensive50.in"));
}
TEST(semantic2, comprehensive24) {
    EXPECT_NO_THROW(runParser("comprehensive24.in"));
}
TEST(semantic2, comprehensive43) {
    EXPECT_NO_THROW(runParser("comprehensive43.in"));
}
TEST(semantic2, comprehensive10) {
    EXPECT_NO_THROW(runParser("comprehensive10.in"));
}
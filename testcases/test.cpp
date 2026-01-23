#include "../src/parser/parser.h"
#include "../src/AST/node/basic.h"
#include "../src/semantic/semantic.h"
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

std::string openFile(std::string path)
{
    path = "../../testcases/testcases/" + path;
    std::ifstream inputFile(path);
    std::string code((std::istreambuf_iterator<char>(inputFile)),
                     std::istreambuf_iterator<char>());
    inputFile.close();
    code.push_back('\n');
    return code;
}

// 在子进程中运行语义检查
int runSemanticInChildProcess(const std::string& path) {
    pid_t pid = fork();

    if (pid < 0) {
        // fork失败
        return -1;
    } else if (pid == 0) {
        // 子进程
        try {
            const auto code = openFile(path);
            Semantic_check(Parser(code).work());
            exit(0);  // 正常执行，退出码为0
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in child process: " << e.what() << std::endl;
            exit(1);  // 抛出异常，退出码为1
        } catch (...) {
            std::cerr << "Unknown exception caught in child process" << std::endl;
            exit(1);  // 未知异常，退出码为1
        }
    } else {
        // 父进程
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            // 正常退出
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // 被信号终止（如段错误等）
            std::cerr << "Child process terminated by signal: " << WTERMSIG(status) << std::endl;
            return -WTERMSIG(status);  // 返回负的信号值
        } else {
            // 其他情况
            return -999;
        }
    }
}

// 包装测试宏的辅助函数
void expectNoThrowInSubprocess(const std::string& path) {
    int result = runSemanticInChildProcess(path);

    // 检查是否正常退出且退出码为0
    EXPECT_GE(result, 0) << "Test case " << path << " crashed or was terminated by signal!";
    if (result >= 0) {
        EXPECT_EQ(result, 0) << "Test case " << path << " threw an exception (exit code: " << result << ")";
    }
}

void expectAnyThrowInSubprocess(const std::string& path) {
    int result = runSemanticInChildProcess(path);

    EXPECT_TRUE(result == 1);

    // // 检查是否抛出异常（退出码为1表示捕获到异常）
    // if (result >= 0) {
    //     EXPECT_NE(result, 0) << "Test case " << path << " did not throw an exception as expected";
    //     // 注意：我们期望退出码为1（表示捕获到异常）
    //     // 但其他非0退出码也被视为抛出异常
    // } else {
    //     // 程序崩溃（如段错误）也被视为某种"异常"抛出
    //     // 这取决于你的测试需求
    //     EXPECT_TRUE(result < 0) << "Test case " << path << " crashed with signal: " << -result;
    // }
}


// 一个更通用的测试宏，用于批量添加测试
#define TEST_SEMANTIC_NO_THROW(test_name, file_name) \
    TEST(SemanticNoThrow, test_name) { \
        expectNoThrowInSubprocess(file_name); \
    }

#define TEST_SEMANTIC_THROW(test_name, file_name) \
    TEST(SemanticThrow, test_name) { \
        expectAnyThrowInSubprocess(file_name); \
    }

// 使用宏添加测试
// 根据原测试用例转换的结果：

// 原测试：EXPECT_NO_THROW
TEST_SEMANTIC_NO_THROW(semantic1_misc14, "misc14.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic4, "basic4.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic1, "basic1.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr36, "expr36.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc34, "misc34.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc7, "misc7.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic37, "basic37.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc36, "misc36.in")
TEST_SEMANTIC_NO_THROW(semantic1_if7, "if7.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc24, "misc24.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr38, "expr38.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic21, "basic21.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic11, "basic11.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic20, "basic20.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc1, "misc1.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc26, "misc26.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr33, "expr33.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc12, "misc12.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc47, "misc47.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc53, "misc53.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc38, "misc38.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc6, "misc6.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic27, "basic27.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref9, "autoref9.in")
TEST_SEMANTIC_NO_THROW(semantic1_return8, "return8.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref2, "autoref2.in")
TEST_SEMANTIC_NO_THROW(semantic1_if9, "if9.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc9, "misc9.in")
TEST_SEMANTIC_NO_THROW(semantic1_return2, "return2.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc18, "misc18.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc65, "misc65.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref6, "autoref6.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic17, "basic17.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref3, "autoref3.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc30, "misc30.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc31, "misc31.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr34, "expr34.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc33, "misc33.in")
TEST_SEMANTIC_NO_THROW(semantic1_array3, "array3.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc16, "misc16.in")
TEST_SEMANTIC_NO_THROW(semantic1_loop5, "loop5.in")
TEST_SEMANTIC_NO_THROW(semantic1_loop3, "loop3.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic40, "basic40.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc52, "misc52.in")
TEST_SEMANTIC_NO_THROW(semantic1_if4, "if4.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc51, "misc51.in")
TEST_SEMANTIC_NO_THROW(semantic1_array1, "array1.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic39, "basic39.in")
TEST_SEMANTIC_NO_THROW(semantic1_return12, "return12.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc15, "misc15.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc37, "misc37.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc11, "misc11.in")
TEST_SEMANTIC_NO_THROW(semantic1_return14, "return14.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc4, "misc4.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic23, "basic23.in")
TEST_SEMANTIC_NO_THROW(semantic1_if6, "if6.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc40, "misc40.in")
TEST_SEMANTIC_NO_THROW(semantic1_if5, "if5.in")
TEST_SEMANTIC_NO_THROW(semantic1_if3, "if3.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc22, "misc22.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref1, "autoref1.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc13, "misc13.in")
TEST_SEMANTIC_NO_THROW(semantic1_if8, "if8.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc10, "misc10.in")
TEST_SEMANTIC_NO_THROW(semantic1_return15, "return15.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc27, "misc27.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc25, "misc25.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc19, "misc19.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic29, "basic29.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc39, "misc39.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc8, "misc8.in")
TEST_SEMANTIC_NO_THROW(semantic1_return13, "return13.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc29, "misc29.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc23, "misc23.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc43, "misc43.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc55, "misc55.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc50, "misc50.in")
TEST_SEMANTIC_NO_THROW(semantic1_if10, "if10.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc41, "misc41.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc49, "misc49.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic25, "basic25.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic18, "basic18.in")
TEST_SEMANTIC_NO_THROW(semantic1_array2, "array2.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc3, "misc3.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref5, "autoref5.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr13, "expr13.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc44, "misc44.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc5, "misc5.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc2, "misc2.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic24, "basic24.in")
TEST_SEMANTIC_NO_THROW(semantic1_if1, "if1.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic26, "basic26.in")
TEST_SEMANTIC_NO_THROW(semantic1_loop4, "loop4.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc48, "misc48.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic19, "basic19.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr17, "expr17.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc17, "misc17.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc32, "misc32.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc20, "misc20.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic22, "basic22.in")
TEST_SEMANTIC_NO_THROW(semantic1_expr19, "expr19.in")
TEST_SEMANTIC_NO_THROW(semantic1_loop2, "loop2.in")
TEST_SEMANTIC_NO_THROW(semantic1_loop1, "loop1.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc45, "misc45.in")
TEST_SEMANTIC_NO_THROW(semantic1_autoref4, "autoref4.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc54, "misc54.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc42, "misc42.in")
TEST_SEMANTIC_NO_THROW(semantic1_if2, "if2.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc28, "misc28.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc21, "misc21.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc46, "misc46.in")
TEST_SEMANTIC_NO_THROW(semantic1_misc35, "misc35.in")
TEST_SEMANTIC_NO_THROW(semantic1_basic36, "basic36.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive33, "comprehensive33.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive36, "comprehensive36.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive49, "comprehensive49.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive34, "comprehensive34.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive27, "comprehensive27.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive21, "comprehensive21.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive42, "comprehensive42.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive25, "comprehensive25.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive7, "comprehensive7.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive22, "comprehensive22.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive23, "comprehensive23.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive41, "comprehensive41.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive26, "comprehensive26.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive32, "comprehensive32.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive5, "comprehensive5.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive12, "comprehensive12.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive48, "comprehensive48.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive19, "comprehensive19.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive13, "comprehensive13.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive31, "comprehensive31.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive38, "comprehensive38.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive16, "comprehensive16.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive29, "comprehensive29.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive17, "comprehensive17.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive46, "comprehensive46.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive8, "comprehensive8.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive2, "comprehensive2.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive4, "comprehensive4.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive20, "comprehensive20.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive11, "comprehensive11.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive45, "comprehensive45.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive18, "comprehensive18.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive35, "comprehensive35.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive40, "comprehensive40.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive37, "comprehensive37.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive6, "comprehensive6.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive44, "comprehensive44.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive14, "comprehensive14.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive30, "comprehensive30.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive28, "comprehensive28.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive3, "comprehensive3.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive9, "comprehensive9.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive47, "comprehensive47.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive1, "comprehensive1.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive15, "comprehensive15.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive39, "comprehensive39.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive50, "comprehensive50.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive24, "comprehensive24.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive43, "comprehensive43.in")
TEST_SEMANTIC_NO_THROW(semantic2_comprehensive10, "comprehensive10.in")

// 原测试：EXPECT_ANY_THROW
TEST_SEMANTIC_THROW(semantic1_type8, "type8.in")
TEST_SEMANTIC_THROW(semantic1_if11, "if11.in")
TEST_SEMANTIC_THROW(semantic1_array8, "array8.in")
TEST_SEMANTIC_THROW(semantic1_basic3, "basic3.in")
TEST_SEMANTIC_THROW(semantic1_basic12, "basic12.in")
TEST_SEMANTIC_THROW(semantic1_expr10, "expr10.in")
TEST_SEMANTIC_THROW(semantic1_expr7, "expr7.in")
TEST_SEMANTIC_THROW(semantic1_expr32, "expr32.in")
TEST_SEMANTIC_THROW(semantic1_expr31, "expr31.in")
TEST_SEMANTIC_THROW(semantic1_array4, "array4.in")
TEST_SEMANTIC_THROW(semantic1_expr28, "expr28.in")
TEST_SEMANTIC_THROW(semantic1_expr12, "expr12.in")
TEST_SEMANTIC_THROW(semantic1_expr40, "expr40.in")
TEST_SEMANTIC_THROW(semantic1_expr16, "expr16.in")
TEST_SEMANTIC_THROW(semantic1_basic28, "basic28.in")
TEST_SEMANTIC_THROW(semantic1_type18, "type18.in")
TEST_SEMANTIC_THROW(semantic1_type4, "type4.in")
TEST_SEMANTIC_THROW(semantic1_misc57, "misc57.in")
TEST_SEMANTIC_THROW(semantic1_return1, "return1.in")
TEST_SEMANTIC_THROW(semantic1_misc60, "misc60.in")
TEST_SEMANTIC_THROW(semantic1_return4, "return4.in")
TEST_SEMANTIC_THROW(semantic1_basic31, "basic31.in")
TEST_SEMANTIC_THROW(semantic1_expr29, "expr29.in")
TEST_SEMANTIC_THROW(semantic1_basic6, "basic6.in")
TEST_SEMANTIC_THROW(semantic1_expr18, "expr18.in")
TEST_SEMANTIC_THROW(semantic1_basic32, "basic32.in")
TEST_SEMANTIC_THROW(semantic1_type9, "type9.in")
TEST_SEMANTIC_THROW(semantic1_type2, "type2.in")
TEST_SEMANTIC_THROW(semantic1_return11, "return11.in")
TEST_SEMANTIC_THROW(semantic1_expr24, "expr24.in")
TEST_SEMANTIC_THROW(semantic1_if14, "if14.in")
TEST_SEMANTIC_THROW(semantic1_if15, "if15.in")
TEST_SEMANTIC_THROW(semantic1_expr9, "expr9.in")
TEST_SEMANTIC_THROW(semantic1_type20, "type20.in")
TEST_SEMANTIC_THROW(semantic1_type17, "type17.in")
TEST_SEMANTIC_THROW(semantic1_array6, "array6.in")
TEST_SEMANTIC_THROW(semantic1_expr20, "expr20.in")
TEST_SEMANTIC_THROW(semantic1_misc58, "misc58.in")
TEST_SEMANTIC_THROW(semantic1_misc61, "misc61.in")
TEST_SEMANTIC_THROW(semantic1_type1, "type1.in")
TEST_SEMANTIC_THROW(semantic1_expr1, "expr1.in")
TEST_SEMANTIC_THROW(semantic1_expr15, "expr15.in")
TEST_SEMANTIC_THROW(semantic1_type6, "type6.in")
TEST_SEMANTIC_THROW(semantic1_loop7, "loop7.in")
TEST_SEMANTIC_THROW(semantic1_return6, "return6.in")
TEST_SEMANTIC_THROW(semantic1_basic16, "basic16.in")
TEST_SEMANTIC_THROW(semantic1_return3, "return3.in")
TEST_SEMANTIC_THROW(semantic1_basic15, "basic15.in")
TEST_SEMANTIC_THROW(semantic1_expr21, "expr21.in")
TEST_SEMANTIC_THROW(semantic1_return5, "return5.in")
TEST_SEMANTIC_THROW(semantic1_expr27, "expr27.in")
TEST_SEMANTIC_THROW(semantic1_expr30, "expr30.in")
TEST_SEMANTIC_THROW(semantic1_type7, "type7.in")
TEST_SEMANTIC_THROW(semantic1_basic10, "basic10.in")
TEST_SEMANTIC_THROW(semantic1_type12, "type12.in")
TEST_SEMANTIC_THROW(semantic1_expr22, "expr22.in")
TEST_SEMANTIC_THROW(semantic1_if12, "if12.in")
TEST_SEMANTIC_THROW(semantic1_type19, "type19.in")
TEST_SEMANTIC_THROW(semantic1_basic35, "basic35.in")
TEST_SEMANTIC_THROW(semantic1_loop10, "loop10.in")
TEST_SEMANTIC_THROW(semantic1_expr35, "expr35.in")
TEST_SEMANTIC_THROW(semantic1_basic13, "basic13.in")
TEST_SEMANTIC_THROW(semantic1_expr25, "expr25.in")
TEST_SEMANTIC_THROW(semantic1_misc64, "misc64.in")
TEST_SEMANTIC_THROW(semantic1_expr4, "expr4.in")
TEST_SEMANTIC_THROW(semantic1_loop8, "loop8.in")
TEST_SEMANTIC_THROW(semantic1_basic5, "basic5.in")
TEST_SEMANTIC_THROW(semantic1_expr37, "expr37.in")
TEST_SEMANTIC_THROW(semantic1_type10, "type10.in")
TEST_SEMANTIC_THROW(semantic1_misc63, "misc63.in")
TEST_SEMANTIC_THROW(semantic1_type11, "type11.in")
TEST_SEMANTIC_THROW(semantic1_expr26, "expr26.in")
TEST_SEMANTIC_THROW(semantic1_array7, "array7.in")
TEST_SEMANTIC_THROW(semantic1_basic8, "basic8.in")
TEST_SEMANTIC_THROW(semantic1_return10, "return10.in")
TEST_SEMANTIC_THROW(semantic1_return9, "return9.in")
TEST_SEMANTIC_THROW(semantic1_expr11, "expr11.in")
TEST_SEMANTIC_THROW(semantic1_expr23, "expr23.in")
TEST_SEMANTIC_THROW(semantic1_loop9, "loop9.in")
TEST_SEMANTIC_THROW(semantic1_expr6, "expr6.in")
TEST_SEMANTIC_THROW(semantic1_autoref7, "autoref7.in")
TEST_SEMANTIC_THROW(semantic1_type14, "type14.in")
TEST_SEMANTIC_THROW(semantic1_return7, "return7.in")
TEST_SEMANTIC_THROW(semantic1_array5, "array5.in")
TEST_SEMANTIC_THROW(semantic1_expr3, "expr3.in")
TEST_SEMANTIC_THROW(semantic1_expr8, "expr8.in")
TEST_SEMANTIC_THROW(semantic1_basic33, "basic33.in")
TEST_SEMANTIC_THROW(semantic1_basic14, "basic14.in")
TEST_SEMANTIC_THROW(semantic1_misc59, "misc59.in")
TEST_SEMANTIC_THROW(semantic1_type15, "type15.in")
TEST_SEMANTIC_THROW(semantic1_basic38, "basic38.in")
TEST_SEMANTIC_THROW(semantic1_basic30, "basic30.in")
TEST_SEMANTIC_THROW(semantic1_loop6, "loop6.in")
TEST_SEMANTIC_THROW(semantic1_misc62, "misc62.in")
TEST_SEMANTIC_THROW(semantic1_expr39, "expr39.in")
TEST_SEMANTIC_THROW(semantic1_misc56, "misc56.in")
TEST_SEMANTIC_THROW(semantic1_if13, "if13.in")
TEST_SEMANTIC_THROW(semantic1_type3, "type3.in")
TEST_SEMANTIC_THROW(semantic1_expr2, "expr2.in")
TEST_SEMANTIC_THROW(semantic1_type5, "type5.in")
TEST_SEMANTIC_THROW(semantic1_type16, "type16.in")
TEST_SEMANTIC_THROW(semantic1_type13, "type13.in")
TEST_SEMANTIC_THROW(semantic1_basic34, "basic34.in")
TEST_SEMANTIC_THROW(semantic1_expr14, "expr14.in")
TEST_SEMANTIC_THROW(semantic1_expr5, "expr5.in")
TEST_SEMANTIC_THROW(semantic1_basic2, "basic2.in")
TEST_SEMANTIC_THROW(semantic1_basic9, "basic9.in")
TEST_SEMANTIC_THROW(semantic1_basic7, "basic7.in")
TEST_SEMANTIC_THROW(semantic1_autoref8, "autoref8.in")

/*

#include "../src/parser/parser.h"
#include "../src/AST/node/basic.h"
#include "../src/semantic/semantic.h"
#include<iostream>
#include <fstream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../testcases/testcases/"+path;
    std::ifstream inputFile(path);
    std::string code((std::istreambuf_iterator<char>(inputFile)),
                     std::istreambuf_iterator<char>());
    // std::cout << "show code:\n" << code << std::endl;
    inputFile.close();
    code.push_back('\n');
    return code;
}

void runSemantic(std::string path)
{
    const auto code=openFile(path);
    Semantic_check(Parser(code).work());
}

TEST(IR1, misc1) {
    EXPECT_NO_THROW(runSemantic("misc1.in"));
}
TEST(semantic1, type8) {
    EXPECT_ANY_THROW(runSemantic("type8.in"));
}
TEST(semantic1, misc14) {
    EXPECT_NO_THROW(runSemantic("misc14.in"));
}
TEST(semantic1, if11) {
    EXPECT_ANY_THROW(runSemantic("if11.in"));
}
TEST(semantic1, array8) {
    EXPECT_ANY_THROW(runSemantic("array8.in"));
}
TEST(semantic1, basic4) {
    EXPECT_NO_THROW(runSemantic("basic4.in"));
}
TEST(semantic1, basic3) {
    EXPECT_ANY_THROW(runSemantic("basic3.in"));
}
TEST(semantic1, basic1) {
    EXPECT_NO_THROW(runSemantic("basic1.in"));
}
TEST(semantic1, basic12) {
    EXPECT_ANY_THROW(runSemantic("basic12.in"));
}
TEST(semantic1, expr10) {
    EXPECT_ANY_THROW(runSemantic("expr10.in"));
}
TEST(semantic1, expr36) {
    EXPECT_NO_THROW(runSemantic("expr36.in"));
}
TEST(semantic1, expr7) {
    EXPECT_ANY_THROW(runSemantic("expr7.in"));
}
TEST(semantic1, misc34) {
    EXPECT_NO_THROW(runSemantic("misc34.in"));
}
TEST(semantic1, expr32) {
    EXPECT_ANY_THROW(runSemantic("expr32.in"));
}
TEST(semantic1, misc7) {
    EXPECT_NO_THROW(runSemantic("misc7.in"));
}
TEST(semantic1, expr31) {
    EXPECT_ANY_THROW(runSemantic("expr31.in"));
}
TEST(semantic1, basic37) {
    EXPECT_NO_THROW(runSemantic("basic37.in"));
}
TEST(semantic1, misc36) {
    EXPECT_NO_THROW(runSemantic("misc36.in"));
}
TEST(semantic1, if7) {
    EXPECT_NO_THROW(runSemantic("if7.in"));
}
TEST(semantic1, array4) {
    EXPECT_ANY_THROW(runSemantic("array4.in"));
}
TEST(semantic1, misc24) {
    EXPECT_NO_THROW(runSemantic("misc24.in"));
}
TEST(semantic1, expr38) {
    EXPECT_NO_THROW(runSemantic("expr38.in"));
}
TEST(semantic1, expr28) {
    EXPECT_ANY_THROW(runSemantic("expr28.in"));
}
TEST(semantic1, basic21) {
    EXPECT_NO_THROW(runSemantic("basic21.in"));
}
TEST(semantic1, expr12) {
    EXPECT_ANY_THROW(runSemantic("expr12.in"));
}
TEST(semantic1, expr40) {
    EXPECT_ANY_THROW(runSemantic("expr40.in"));
}
TEST(semantic1, basic11) {
    EXPECT_NO_THROW(runSemantic("basic11.in"));
}
TEST(semantic1, expr16) {
    EXPECT_ANY_THROW(runSemantic("expr16.in"));
}
TEST(semantic1, basic28) {
    EXPECT_ANY_THROW(runSemantic("basic28.in"));
}
TEST(semantic1, type18) {
    EXPECT_ANY_THROW(runSemantic("type18.in"));
}
TEST(semantic1, basic20) {
    EXPECT_NO_THROW(runSemantic("basic20.in"));
}
TEST(semantic1, misc1) {
    EXPECT_NO_THROW(runSemantic("misc1.in"));
}
TEST(semantic1, type4) {
    EXPECT_ANY_THROW(runSemantic("type4.in"));
}
TEST(semantic1, misc26) {
    EXPECT_NO_THROW(runSemantic("misc26.in"));
}
TEST(semantic1, misc57) {
    EXPECT_ANY_THROW(runSemantic("misc57.in"));
}
TEST(semantic1, expr33) {
    EXPECT_NO_THROW(runSemantic("expr33.in"));
}
TEST(semantic1, misc12) {
    EXPECT_NO_THROW(runSemantic("misc12.in"));
}
TEST(semantic1, misc47) {
    EXPECT_NO_THROW(runSemantic("misc47.in"));
}
TEST(semantic1, return1) {
    EXPECT_ANY_THROW(runSemantic("return1.in"));
}
TEST(semantic1, misc60) {
    EXPECT_ANY_THROW(runSemantic("misc60.in"));
}
TEST(semantic1, return4) {
    EXPECT_ANY_THROW(runSemantic("return4.in"));
}
TEST(semantic1, basic31) {
    EXPECT_ANY_THROW(runSemantic("basic31.in"));
}
TEST(semantic1, misc53) {
    EXPECT_NO_THROW(runSemantic("misc53.in"));
}
TEST(semantic1, misc38) {
    EXPECT_NO_THROW(runSemantic("misc38.in"));
}
TEST(semantic1, misc6) {
    EXPECT_NO_THROW(runSemantic("misc6.in"));
}
TEST(semantic1, basic27) {
    EXPECT_NO_THROW(runSemantic("basic27.in"));
}
TEST(semantic1, expr29) {
    EXPECT_ANY_THROW(runSemantic("expr29.in"));
}
TEST(semantic1, autoref9) {
    EXPECT_NO_THROW(runSemantic("autoref9.in"));
}
TEST(semantic1, return8) {
    EXPECT_NO_THROW(runSemantic("return8.in"));
}
TEST(semantic1, basic6) {
    EXPECT_ANY_THROW(runSemantic("basic6.in"));
}
TEST(semantic1, autoref2) {
    EXPECT_NO_THROW(runSemantic("autoref2.in"));
}
TEST(semantic1, expr18) {
    EXPECT_ANY_THROW(runSemantic("expr18.in"));
}
TEST(semantic1, if9) {
    EXPECT_NO_THROW(runSemantic("if9.in"));
}
TEST(semantic1, basic32) {
    EXPECT_ANY_THROW(runSemantic("basic32.in"));
}
TEST(semantic1, misc9) {
    EXPECT_NO_THROW(runSemantic("misc9.in"));
}
TEST(semantic1, type9) {
    EXPECT_ANY_THROW(runSemantic("type9.in"));
}
TEST(semantic1, return2) {
    EXPECT_NO_THROW(runSemantic("return2.in"));
}
TEST(semantic1, type2) {
    EXPECT_ANY_THROW(runSemantic("type2.in"));
}
TEST(semantic1, misc18) {
    EXPECT_NO_THROW(runSemantic("misc18.in"));
}
TEST(semantic1, misc65) {
    EXPECT_NO_THROW(runSemantic("misc65.in"));
}
TEST(semantic1, autoref6) {
    EXPECT_NO_THROW(runSemantic("autoref6.in"));
}
TEST(semantic1, return11) {
    EXPECT_ANY_THROW(runSemantic("return11.in"));
}
TEST(semantic1, basic17) {
    EXPECT_NO_THROW(runSemantic("basic17.in"));
}
TEST(semantic1, expr24) {
    EXPECT_ANY_THROW(runSemantic("expr24.in"));
}
TEST(semantic1, autoref3) {
    EXPECT_NO_THROW(runSemantic("autoref3.in"));
}
TEST(semantic1, misc30) {
    EXPECT_NO_THROW(runSemantic("misc30.in"));
}
TEST(semantic1, misc31) {
    EXPECT_NO_THROW(runSemantic("misc31.in"));
}
TEST(semantic1, if14) {
    EXPECT_ANY_THROW(runSemantic("if14.in"));
}
TEST(semantic1, expr34) {
    EXPECT_NO_THROW(runSemantic("expr34.in"));
}
TEST(semantic1, if15) {
    EXPECT_ANY_THROW(runSemantic("if15.in"));
}
TEST(semantic1, misc33) {
    EXPECT_NO_THROW(runSemantic("misc33.in"));
}
TEST(semantic1, expr9) {
    EXPECT_ANY_THROW(runSemantic("expr9.in"));
}
TEST(semantic1, type20) {
    EXPECT_ANY_THROW(runSemantic("type20.in"));
}
TEST(semantic1, array3) {
    EXPECT_NO_THROW(runSemantic("array3.in"));
}
TEST(semantic1, misc16) {
    EXPECT_NO_THROW(runSemantic("misc16.in"));
}
TEST(semantic1, type17) {
    EXPECT_ANY_THROW(runSemantic("type17.in"));
}
TEST(semantic1, loop5) {
    EXPECT_NO_THROW(runSemantic("loop5.in"));
}
TEST(semantic1, array6) {
    EXPECT_ANY_THROW(runSemantic("array6.in"));
}
TEST(semantic1, loop3) {
    EXPECT_NO_THROW(runSemantic("loop3.in"));
}
TEST(semantic1, basic40) {
    EXPECT_NO_THROW(runSemantic("basic40.in"));
}
TEST(semantic1, misc52) {
    EXPECT_NO_THROW(runSemantic("misc52.in"));
}
TEST(semantic1, if4) {
    EXPECT_NO_THROW(runSemantic("if4.in"));
}
TEST(semantic1, misc51) {
    EXPECT_NO_THROW(runSemantic("misc51.in"));
}
TEST(semantic1, expr20) {
    EXPECT_ANY_THROW(runSemantic("expr20.in"));
}
TEST(semantic1, misc58) {
    EXPECT_ANY_THROW(runSemantic("misc58.in"));
}
TEST(semantic1, array1) {
    EXPECT_NO_THROW(runSemantic("array1.in"));
}
TEST(semantic1, basic39) {
    EXPECT_NO_THROW(runSemantic("basic39.in"));
}
TEST(semantic1, return12) {
    EXPECT_NO_THROW(runSemantic("return12.in"));
}
TEST(semantic1, misc15) {
    EXPECT_NO_THROW(runSemantic("misc15.in"));
}
TEST(semantic1, misc61) {
    EXPECT_ANY_THROW(runSemantic("misc61.in"));
}
TEST(semantic1, type1) {
    EXPECT_ANY_THROW(runSemantic("type1.in"));
}
TEST(semantic1, misc37) {
    EXPECT_NO_THROW(runSemantic("misc37.in"));
}
TEST(semantic1, expr1) {
    EXPECT_ANY_THROW(runSemantic("expr1.in"));
}
TEST(semantic1, expr15) {
    EXPECT_ANY_THROW(runSemantic("expr15.in"));
}
TEST(semantic1, misc11) {
    EXPECT_NO_THROW(runSemantic("misc11.in"));
}
TEST(semantic1, type6) {
    EXPECT_ANY_THROW(runSemantic("type6.in"));
}
TEST(semantic1, return14) {
    EXPECT_NO_THROW(runSemantic("return14.in"));
}
TEST(semantic1, misc4) {
    EXPECT_NO_THROW(runSemantic("misc4.in"));
}
TEST(semantic1, basic23) {
    EXPECT_NO_THROW(runSemantic("basic23.in"));
}
TEST(semantic1, loop7) {
    EXPECT_ANY_THROW(runSemantic("loop7.in"));
}
TEST(semantic1, return6) {
    EXPECT_ANY_THROW(runSemantic("return6.in"));
}
TEST(semantic1, basic16) {
    EXPECT_ANY_THROW(runSemantic("basic16.in"));
}
TEST(semantic1, if6) {
    EXPECT_NO_THROW(runSemantic("if6.in"));
}
TEST(semantic1, return3) {
    EXPECT_ANY_THROW(runSemantic("return3.in"));
}
TEST(semantic1, basic15) {
    EXPECT_ANY_THROW(runSemantic("basic15.in"));
}
TEST(semantic1, expr21) {
    EXPECT_ANY_THROW(runSemantic("expr21.in"));
}
TEST(semantic1, misc40) {
    EXPECT_NO_THROW(runSemantic("misc40.in"));
}
TEST(semantic1, return5) {
    EXPECT_ANY_THROW(runSemantic("return5.in"));
}
TEST(semantic1, expr27) {
    EXPECT_ANY_THROW(runSemantic("expr27.in"));
}
TEST(semantic1, if5) {
    EXPECT_NO_THROW(runSemantic("if5.in"));
}
TEST(semantic1, if3) {
    EXPECT_NO_THROW(runSemantic("if3.in"));
}
TEST(semantic1, misc22) {
    EXPECT_NO_THROW(runSemantic("misc22.in"));
}
TEST(semantic1, autoref1) {
    EXPECT_NO_THROW(runSemantic("autoref1.in"));
}
TEST(semantic1, expr30) {
    EXPECT_ANY_THROW(runSemantic("expr30.in"));
}
TEST(semantic1, misc13) {
    EXPECT_NO_THROW(runSemantic("misc13.in"));
}
TEST(semantic1, if8) {
    EXPECT_NO_THROW(runSemantic("if8.in"));
}
TEST(semantic1, type7) {
    EXPECT_ANY_THROW(runSemantic("type7.in"));
}
TEST(semantic1, misc10) {
    EXPECT_NO_THROW(runSemantic("misc10.in"));
}
TEST(semantic1, basic10) {
    EXPECT_ANY_THROW(runSemantic("basic10.in"));
}
TEST(semantic1, return15) {
    EXPECT_NO_THROW(runSemantic("return15.in"));
}
TEST(semantic1, type12) {
    EXPECT_ANY_THROW(runSemantic("type12.in"));
}
TEST(semantic1, misc27) {
    EXPECT_NO_THROW(runSemantic("misc27.in"));
}
TEST(semantic1, misc25) {
    EXPECT_NO_THROW(runSemantic("misc25.in"));
}
TEST(semantic1, misc19) {
    EXPECT_NO_THROW(runSemantic("misc19.in"));
}
TEST(semantic1, basic29) {
    EXPECT_NO_THROW(runSemantic("basic29.in"));
}
TEST(semantic1, misc39) {
    EXPECT_NO_THROW(runSemantic("misc39.in"));
}
TEST(semantic1, expr22) {
    EXPECT_ANY_THROW(runSemantic("expr22.in"));
}
TEST(semantic1, if12) {
    EXPECT_ANY_THROW(runSemantic("if12.in"));
}
TEST(semantic1, misc8) {
    EXPECT_NO_THROW(runSemantic("misc8.in"));
}
TEST(semantic1, type19) {
    EXPECT_ANY_THROW(runSemantic("type19.in"));
}
TEST(semantic1, basic35) {
    EXPECT_ANY_THROW(runSemantic("basic35.in"));
}
TEST(semantic1, loop10) {
    EXPECT_ANY_THROW(runSemantic("loop10.in"));
}
TEST(semantic1, return13) {
    EXPECT_NO_THROW(runSemantic("return13.in"));
}
TEST(semantic1, expr35) {
    EXPECT_ANY_THROW(runSemantic("expr35.in"));
}
TEST(semantic1, basic13) {
    EXPECT_ANY_THROW(runSemantic("basic13.in"));
}
TEST(semantic1, expr25) {
    EXPECT_ANY_THROW(runSemantic("expr25.in"));
}
TEST(semantic1, misc29) {
    EXPECT_NO_THROW(runSemantic("misc29.in"));
}
TEST(semantic1, misc64) {
    EXPECT_ANY_THROW(runSemantic("misc64.in"));
}
TEST(semantic1, misc23) {
    EXPECT_NO_THROW(runSemantic("misc23.in"));
}
TEST(semantic1, misc43) {
    EXPECT_NO_THROW(runSemantic("misc43.in"));
}
TEST(semantic1, misc55) {
    EXPECT_NO_THROW(runSemantic("misc55.in"));
}
TEST(semantic1, expr4) {
    EXPECT_ANY_THROW(runSemantic("expr4.in"));
}
TEST(semantic1, loop8) {
    EXPECT_ANY_THROW(runSemantic("loop8.in"));
}
TEST(semantic1, basic5) {
    EXPECT_ANY_THROW(runSemantic("basic5.in"));
}
TEST(semantic1, expr37) {
    EXPECT_ANY_THROW(runSemantic("expr37.in"));
}
TEST(semantic1, misc50) {
    EXPECT_NO_THROW(runSemantic("misc50.in"));
}
TEST(semantic1, if10) {
    EXPECT_NO_THROW(runSemantic("if10.in"));
}
TEST(semantic1, misc41) {
    EXPECT_NO_THROW(runSemantic("misc41.in"));
}
TEST(semantic1, type10) {
    EXPECT_ANY_THROW(runSemantic("type10.in"));
}
TEST(semantic1, misc49) {
    EXPECT_NO_THROW(runSemantic("misc49.in"));
}
TEST(semantic1, basic25) {
    EXPECT_NO_THROW(runSemantic("basic25.in"));
}
TEST(semantic1, basic18) {
    EXPECT_NO_THROW(runSemantic("basic18.in"));
}
TEST(semantic1, array2) {
    EXPECT_NO_THROW(runSemantic("array2.in"));
}
TEST(semantic1, misc63) {
    EXPECT_ANY_THROW(runSemantic("misc63.in"));
}
TEST(semantic1, misc3) {
    EXPECT_NO_THROW(runSemantic("misc3.in"));
}
TEST(semantic1, type11) {
    EXPECT_ANY_THROW(runSemantic("type11.in"));
}
TEST(semantic1, expr26) {
    EXPECT_ANY_THROW(runSemantic("expr26.in"));
}
TEST(semantic1, array7) {
    EXPECT_ANY_THROW(runSemantic("array7.in"));
}
TEST(semantic1, autoref5) {
    EXPECT_NO_THROW(runSemantic("autoref5.in"));
}
TEST(semantic1, basic8) {
    EXPECT_ANY_THROW(runSemantic("basic8.in"));
}
TEST(semantic1, expr13) {
    EXPECT_NO_THROW(runSemantic("expr13.in"));
}
TEST(semantic1, return10) {
    EXPECT_ANY_THROW(runSemantic("return10.in"));
}
TEST(semantic1, return9) {
    EXPECT_ANY_THROW(runSemantic("return9.in"));
}
TEST(semantic1, expr11) {
    EXPECT_ANY_THROW(runSemantic("expr11.in"));
}
TEST(semantic1, misc44) {
    EXPECT_NO_THROW(runSemantic("misc44.in"));
}
TEST(semantic1, misc5) {
    EXPECT_NO_THROW(runSemantic("misc5.in"));
}
TEST(semantic1, misc2) {
    EXPECT_NO_THROW(runSemantic("misc2.in"));
}
TEST(semantic1, expr23) {
    EXPECT_ANY_THROW(runSemantic("expr23.in"));
}
TEST(semantic1, loop9) {
    EXPECT_ANY_THROW(runSemantic("loop9.in"));
}
TEST(semantic1, basic24) {
    EXPECT_NO_THROW(runSemantic("basic24.in"));
}
TEST(semantic1, if1) {
    EXPECT_NO_THROW(runSemantic("if1.in"));
}
TEST(semantic1, basic26) {
    EXPECT_NO_THROW(runSemantic("basic26.in"));
}
TEST(semantic1, expr6) {
    EXPECT_ANY_THROW(runSemantic("expr6.in"));
}
TEST(semantic1, autoref7) {
    EXPECT_ANY_THROW(runSemantic("autoref7.in"));
}
TEST(semantic1, type14) {
    EXPECT_ANY_THROW(runSemantic("type14.in"));
}
TEST(semantic1, loop4) {
    EXPECT_NO_THROW(runSemantic("loop4.in"));
}
TEST(semantic1, return7) {
    EXPECT_ANY_THROW(runSemantic("return7.in"));
}
TEST(semantic1, array5) {
    EXPECT_ANY_THROW(runSemantic("array5.in"));
}
TEST(semantic1, misc48) {
    EXPECT_NO_THROW(runSemantic("misc48.in"));
}
TEST(semantic1, expr3) {
    EXPECT_ANY_THROW(runSemantic("expr3.in"));
}
TEST(semantic1, expr8) {
    EXPECT_ANY_THROW(runSemantic("expr8.in"));
}
TEST(semantic1, basic33) {
    EXPECT_ANY_THROW(runSemantic("basic33.in"));
}
TEST(semantic1, basic19) {
    EXPECT_NO_THROW(runSemantic("basic19.in"));
}
TEST(semantic1, basic14) {
    EXPECT_ANY_THROW(runSemantic("basic14.in"));
}
TEST(semantic1, expr17) {
    EXPECT_NO_THROW(runSemantic("expr17.in"));
}
TEST(semantic1, misc17) {
    EXPECT_NO_THROW(runSemantic("misc17.in"));
}
TEST(semantic1, misc59) {
    EXPECT_ANY_THROW(runSemantic("misc59.in"));
}
TEST(semantic1, misc32) {
    EXPECT_NO_THROW(runSemantic("misc32.in"));
}
TEST(semantic1, type15) {
    EXPECT_ANY_THROW(runSemantic("type15.in"));
}
TEST(semantic1, basic38) {
    EXPECT_ANY_THROW(runSemantic("basic38.in"));
}
TEST(semantic1, misc20) {
    EXPECT_NO_THROW(runSemantic("misc20.in"));
}
TEST(semantic1, basic22) {
    EXPECT_NO_THROW(runSemantic("basic22.in"));
}
TEST(semantic1, expr19) {
    EXPECT_NO_THROW(runSemantic("expr19.in"));
}
TEST(semantic1, basic30) {
    EXPECT_ANY_THROW(runSemantic("basic30.in"));
}
TEST(semantic1, loop6) {
    EXPECT_ANY_THROW(runSemantic("loop6.in"));
}
TEST(semantic1, misc62) {
    EXPECT_ANY_THROW(runSemantic("misc62.in"));
}
TEST(semantic1, expr39) {
    EXPECT_ANY_THROW(runSemantic("expr39.in"));
}
TEST(semantic1, loop2) {
    EXPECT_NO_THROW(runSemantic("loop2.in"));
}
TEST(semantic1, loop1) {
    EXPECT_NO_THROW(runSemantic("loop1.in"));
}
TEST(semantic1, misc45) {
    EXPECT_NO_THROW(runSemantic("misc45.in"));
}
TEST(semantic1, autoref4) {
    EXPECT_NO_THROW(runSemantic("autoref4.in"));
}
TEST(semantic1, misc56) {
    EXPECT_ANY_THROW(runSemantic("misc56.in"));
}
TEST(semantic1, if13) {
    EXPECT_ANY_THROW(runSemantic("if13.in"));
}
TEST(semantic1, type3) {
    EXPECT_ANY_THROW(runSemantic("type3.in"));
}
TEST(semantic1, misc54) {
    EXPECT_NO_THROW(runSemantic("misc54.in"));
}
TEST(semantic1, expr2) {
    EXPECT_ANY_THROW(runSemantic("expr2.in"));
}
TEST(semantic1, misc42) {
    EXPECT_NO_THROW(runSemantic("misc42.in"));
}
TEST(semantic1, type5) {
    EXPECT_ANY_THROW(runSemantic("type5.in"));
}
TEST(semantic1, type16) {
    EXPECT_ANY_THROW(runSemantic("type16.in"));
}
TEST(semantic1, type13) {
    EXPECT_ANY_THROW(runSemantic("type13.in"));
}
TEST(semantic1, if2) {
    EXPECT_NO_THROW(runSemantic("if2.in"));
}
TEST(semantic1, basic34) {
    EXPECT_ANY_THROW(runSemantic("basic34.in"));
}
TEST(semantic1, expr14) {
    EXPECT_ANY_THROW(runSemantic("expr14.in"));
}
TEST(semantic1, expr5) {
    EXPECT_ANY_THROW(runSemantic("expr5.in"));
}
TEST(semantic1, misc28) {
    EXPECT_NO_THROW(runSemantic("misc28.in"));
}
TEST(semantic1, basic2) {
    EXPECT_ANY_THROW(runSemantic("basic2.in"));
}
TEST(semantic1, misc21) {
    EXPECT_NO_THROW(runSemantic("misc21.in"));
}
TEST(semantic1, misc46) {
    EXPECT_NO_THROW(runSemantic("misc46.in"));
}
TEST(semantic1, basic9) {
    EXPECT_ANY_THROW(runSemantic("basic9.in"));
}
TEST(semantic1, basic7) {
    EXPECT_ANY_THROW(runSemantic("basic7.in"));
}
TEST(semantic1, misc35) {
    EXPECT_NO_THROW(runSemantic("misc35.in"));
}
TEST(semantic1, basic36) {
    EXPECT_NO_THROW(runSemantic("basic36.in"));
}
TEST(semantic1, autoref8) {
    EXPECT_ANY_THROW(runSemantic("autoref8.in"));
}
TEST(semantic2, comprehensive33) {
    EXPECT_NO_THROW(runSemantic("comprehensive33.in"));
}
TEST(semantic2, comprehensive36) {
    EXPECT_NO_THROW(runSemantic("comprehensive36.in"));
}
TEST(semantic2, comprehensive49) {
    EXPECT_NO_THROW(runSemantic("comprehensive49.in"));
}
TEST(semantic2, comprehensive34) {
    EXPECT_NO_THROW(runSemantic("comprehensive34.in"));
}
TEST(semantic2, comprehensive27) {
    EXPECT_NO_THROW(runSemantic("comprehensive27.in"));
}
TEST(semantic2, comprehensive21) {
    EXPECT_NO_THROW(runSemantic("comprehensive21.in"));
}
TEST(semantic2, comprehensive42) {
    EXPECT_NO_THROW(runSemantic("comprehensive42.in"));
}
TEST(semantic2, comprehensive25) {
    EXPECT_NO_THROW(runSemantic("comprehensive25.in"));
}
TEST(semantic2, comprehensive7) {
    EXPECT_NO_THROW(runSemantic("comprehensive7.in"));
}
TEST(semantic2, comprehensive22) {
    EXPECT_NO_THROW(runSemantic("comprehensive22.in"));
}
TEST(semantic2, comprehensive23) {
    EXPECT_NO_THROW(runSemantic("comprehensive23.in"));
}
TEST(semantic2, comprehensive41) {
    EXPECT_NO_THROW(runSemantic("comprehensive41.in"));
}
TEST(semantic2, comprehensive26) {
    EXPECT_NO_THROW(runSemantic("comprehensive26.in"));
}
TEST(semantic2, comprehensive32) {
    EXPECT_NO_THROW(runSemantic("comprehensive32.in"));
}
TEST(semantic2, comprehensive5) {
    EXPECT_NO_THROW(runSemantic("comprehensive5.in"));
}
TEST(semantic2, comprehensive12) {
    EXPECT_NO_THROW(runSemantic("comprehensive12.in"));
}
TEST(semantic2, comprehensive48) {
    EXPECT_NO_THROW(runSemantic("comprehensive48.in"));
}
TEST(semantic2, comprehensive19) {
    EXPECT_NO_THROW(runSemantic("comprehensive19.in"));
}
TEST(semantic2, comprehensive13) {
    EXPECT_NO_THROW(runSemantic("comprehensive13.in"));
}
TEST(semantic2, comprehensive31) {
    EXPECT_NO_THROW(runSemantic("comprehensive31.in"));
}
TEST(semantic2, comprehensive38) {
    EXPECT_NO_THROW(runSemantic("comprehensive38.in"));
}
TEST(semantic2, comprehensive16) {
    EXPECT_NO_THROW(runSemantic("comprehensive16.in"));
}
TEST(semantic2, comprehensive29) {
    EXPECT_NO_THROW(runSemantic("comprehensive29.in"));
}
TEST(semantic2, comprehensive17) {
    EXPECT_NO_THROW(runSemantic("comprehensive17.in"));
}
TEST(semantic2, comprehensive46) {
    EXPECT_NO_THROW(runSemantic("comprehensive46.in"));
}
TEST(semantic2, comprehensive8) {
    EXPECT_NO_THROW(runSemantic("comprehensive8.in"));
}
TEST(semantic2, comprehensive2) {
    EXPECT_NO_THROW(runSemantic("comprehensive2.in"));
}
TEST(semantic2, comprehensive4) {
    EXPECT_NO_THROW(runSemantic("comprehensive4.in"));
}
TEST(semantic2, comprehensive20) {
    EXPECT_NO_THROW(runSemantic("comprehensive20.in"));
}
TEST(semantic2, comprehensive11) {
    EXPECT_NO_THROW(runSemantic("comprehensive11.in"));
}
TEST(semantic2, comprehensive45) {
    EXPECT_NO_THROW(runSemantic("comprehensive45.in"));
}
TEST(semantic2, comprehensive18) {
    EXPECT_NO_THROW(runSemantic("comprehensive18.in"));
}
TEST(semantic2, comprehensive35) {
    EXPECT_NO_THROW(runSemantic("comprehensive35.in"));
}
TEST(semantic2, comprehensive40) {
    EXPECT_NO_THROW(runSemantic("comprehensive40.in"));
}
TEST(semantic2, comprehensive37) {
    EXPECT_NO_THROW(runSemantic("comprehensive37.in"));
}
TEST(semantic2, comprehensive6) {
    EXPECT_NO_THROW(runSemantic("comprehensive6.in"));
}
TEST(semantic2, comprehensive44) {
    EXPECT_NO_THROW(runSemantic("comprehensive44.in"));
}
TEST(semantic2, comprehensive14) {
    EXPECT_NO_THROW(runSemantic("comprehensive14.in"));
}
TEST(semantic2, comprehensive30) {
    EXPECT_NO_THROW(runSemantic("comprehensive30.in"));
}
TEST(semantic2, comprehensive28) {
    EXPECT_NO_THROW(runSemantic("comprehensive28.in"));
}
TEST(semantic2, comprehensive3) {
    EXPECT_NO_THROW(runSemantic("comprehensive3.in"));
}
TEST(semantic2, comprehensive9) {
    EXPECT_NO_THROW(runSemantic("comprehensive9.in"));
}
TEST(semantic2, comprehensive47) {
    EXPECT_NO_THROW(runSemantic("comprehensive47.in"));
}
TEST(semantic2, comprehensive1) {
    EXPECT_NO_THROW(runSemantic("comprehensive1.in"));
}
TEST(semantic2, comprehensive15) {
    EXPECT_NO_THROW(runSemantic("comprehensive15.in"));
}
TEST(semantic2, comprehensive39) {
    EXPECT_NO_THROW(runSemantic("comprehensive39.in"));
}
TEST(semantic2, comprehensive50) {
    EXPECT_NO_THROW(runSemantic("comprehensive50.in"));
}
TEST(semantic2, comprehensive24) {
    EXPECT_NO_THROW(runSemantic("comprehensive24.in"));
}
TEST(semantic2, comprehensive43) {
    EXPECT_NO_THROW(runSemantic("comprehensive43.in"));
}
TEST(semantic2, comprehensive10) {
    EXPECT_NO_THROW(runSemantic("comprehensive10.in"));
}

*/
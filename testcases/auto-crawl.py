import os, sys, json, glob, shutil
import subprocess
from pathlib import Path

# ---------- 参数 ----------
GIT_URL     = str(sys.argv[1]) if len(sys.argv) > 1 else "https://github.com/RayZh-hs/RCompiler-Testcases"
LOCAL_DIR   = sys.argv[2] if len(sys.argv) > 2 else Path(GIT_URL).stem
REPO_ROOT   = Path(LOCAL_DIR).resolve()
OUT_BASE    = REPO_ROOT.parent
OUT_TC      = OUT_BASE / "testcases"
OUT_CPP     = OUT_BASE / "test.cpp"

print(">>> 开始处理测试用例...")

# ---------- 1. Git 操作 ----------
if REPO_ROOT.exists():
    print(">>> 拉取最新代码…")
    subprocess.run(["git", "pull"], cwd=REPO_ROOT, check=True)
else:
    print(">>> 克隆仓库…")
    subprocess.run(["git", "clone", GIT_URL, str(REPO_ROOT)], check=True)

# 切换到仓库目录
original_cwd = os.getcwd()
os.chdir(REPO_ROOT)

# ---------- 2. 复制 *.rx → testcases/*.in ----------
OUT_TC.mkdir(parents=True, exist_ok=True)
rx_files = list(Path(".").rglob("*.rx"))
seen = set()
for rx in rx_files:
    rel = rx.stem
    if rel in seen:
        continue
    seen.add(rel)
    dst = OUT_TC / f"{rel}.in"
    shutil.copy2(rx, dst)
print(f">>> 已复制 {len(seen)} 个 .rx → {OUT_TC}")

# ---------- 3. 生成 all_tests.cpp ----------
jsons = list(Path(".").rglob("testcase_info.json"))
blocks = {}
for jf in jsons:
    try:
        data = json.loads(jf.read_text(encoding="utf8"))

        rel_path = jf.parent.relative_to(".")
        suite = rel_path.parts[0].replace("-", "")
        case  = rel_path.name

        # if case[0:5] != "array" : continue

        code  = data.get("compileexitcode", 0)
        inp   = f"{case}.in"
        expect = "EXPECT_NO_THROW" if code == 0 else "EXPECT_ANY_THROW"

        blocks[jf] = f"TEST({suite}, {case}) {{\n" \
                     f"    {expect}(runParser(\"{inp}\"));\n" \
                     f"}}"
    except Exception as e:
        print(f"警告: 处理 {jf} 时出错: {e}")
        continue

OUT_CPP.write_text(R"""#include "../src/parser/parser.h"
#include "../src/AST/node/basic.h"
#include "../src/semantic/semantic_analyzer.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../testcases/testcases/"+path;
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

void runSematic(std::string path)
{
    const auto code=openFile(path);
    SemanticAnalyzer semantic_analyzer(Parser(code).work());
    semantic_analyzer.analyze();
    if (semantic_analyzer.has_errors()) {
      	std::cout << "tell me tell me" << std::endl;
        throw std::runtime_error("RE");
    }
}

"""
                   +("\n".join(blocks.values())), encoding="utf8")
print(f">>> 已生成 {OUT_CPP}  （共 {len(blocks)} 条 TEST）")

# 恢复原始工作目录
os.chdir(original_cwd)

# ---------- 4. 删除临时仓库 ----------
import stat
import time

def force_rmtree(path):
    """兼容性更好的强制删除函数"""
    def onerror(func, path, exc_info):
        # 兼容旧版本 Python 的 onerror 回调
        if not os.access(path, os.W_OK):
            os.chmod(path, stat.S_IWUSR)
            func(path)
        else:
            raise

    # 尝试多次删除
    for attempt in range(3):
        try:
            # 使用 onerror 而不是 onexc 以兼容旧版本
            shutil.rmtree(path, onerror=onerror)
            print(f">>> 已删除临时仓库：{path}")
            return
        except (PermissionError, OSError) as e:
            if attempt == 2:  # 最后一次尝试
                print(f"警告: 无法完全删除 {path}, 错误: {e}")
                # 尝试删除内容但保留目录
                try:
                    for root, dirs, files in os.walk(path, topdown=False):
                        for name in files:
                            file_path = os.path.join(root, name)
                            try:
                                os.chmod(file_path, stat.S_IWUSR)
                                os.unlink(file_path)
                            except:
                                pass
                        for name in dirs:
                            dir_path = os.path.join(root, name)
                            try:
                                os.chmod(dir_path, stat.S_IWUSR)
                                os.rmdir(dir_path)
                            except:
                                pass
                    print(f">>> 已清理仓库内容，但目录 {path} 可能仍存在")
                except:
                    print(f">>> 无法清理 {path}，请手动删除")
            else:
                time.sleep(0.5)

# 删除临时仓库
if REPO_ROOT.exists():
    force_rmtree(REPO_ROOT)
else:
    print(">>> 仓库目录不存在，无需删除")

print(">>> 处理完成！")
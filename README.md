# CsGo
2021编译原理课程作业

### 1.1 语言特点

C语言基本语法加上Go的部分特点

### 1.2 开发环境

- 操作系统：Linux
- 编译环境：
  - Flex 2.6.4
  - Bison 3.5.1 (GNU Bison)
  - LLVM 10.0.0

### 1.3 文件说明

- src：源代码文件夹

  - mini.l：Flex源代码，主要实现词法分析，生成Token
  - mini.y：Yacc源代码，主要实现语法分析，生成抽象语法树
  - ast.h：抽象语法树头文件，定义所有AST节点类
  - ast.cpp：抽象语法树实现文件，主要包含`codeGen`和`getJoson`方法的实现
  - CodeGenerator.h：中间代码生成器头文件，定义生成器环境
  - CodeGenerator.cpp：中间代码生成器实现文件
  - ObjGenerator.h：生成目标代码所用头文件
  - ppMacro.h：宏定义相关头文件
  - ppMacro.cpp：宏定义相关实现文件
  - Makefile：定义编译链接规则
  - gentest.sh：生成目标代码并执行测试的脚本
  - tree.json：基于AST生成的JSON文件
  - tree.html：可视化AST的网页文件

- doc：报告文档文件夹

  - report.pdf：报告文档

  - Slides.pdf：展示文档

- test：测试用例文件夹

  - qsort
    - qsort.gc：快速排序的实现代码
    - linux-amd64：测试文件

  - matrix
    - matrix.gc：矩阵乘法的实现代码
    - linux-amd64：测试文件

  - course_assist
    - c_assist.gc：选课助手实现代码
    - linux-amd64：测试文件

### 1.4 组员分工

| 组员   | 具体分工                             |
| :----- | :----------------------------------- |
| 刘馨宇 | 词法分析，语法分析                   |
| 毕邹彬 | AST定义，AST可视化，宏定义           |
| 吴韬   | 语义分析，中间代码生成，目标代码生成 |

### 1.5 运行方式

可使用测试脚本`gentest.sh`运行提供的测试程序

```shell
cd ./src
make
./mini filename.gc
clang filename.o -o filename.out
```


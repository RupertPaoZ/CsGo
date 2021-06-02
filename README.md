# CsGo
2021编译原理课程作业



## DeadLine

| Date | Target                         |
| ---- | ------------------------------ |
| 5.9  | 完成词法分析和语法分析         |
| 5.21 | 完成词法分析和语法分析部分代码 |



5/21 更新：

- lex_test: 对flex进行测试，make编译后，输入`./mini_test test.gc`可以输出词法翻译结果

- yacc_test: 对bison进行测试，make编译后，输入同上，可以测试语法分析的解析结果。

  ​       如果出错，50%的概率是bison写的不对，50%的概率是test.gc语法不对。

5/22 更新：
- 修复语法问题。添加了`function_stmt`一行：
  ~~~
  stmt:
    expr_stmt                           { printf("stmt -> expr_stmt\n"); }
    | compound_stmt                     { printf("stmt -> compound_stmt\n"); }
    | selection_stmt                    { printf("stmt -> selection_stmt\n"); }
    | iteration_stmt                    { printf("stmt -> iteration-stmt\n"); }
    | return_stmt                       { printf("stmt -> return-stmt\n"); }
    | function_stmt                     { printf("stmt -> function-stmt\n"); }
  
  function_stmt:
    call
  ~~~
  修改后，函数调用，if、while语句均可正常执行。
- 接下来的可能改进：
  - [ ] for语句
  - [x] 数组
  - [ ] 声明处赋值

5/25 :
完成ast和可视化部分，对于之前提到过的测试文件进行测试均能通过，同时增加了嵌套if-else和while的测试。



6.2：

不知道修改了多少地方！

运行：

```
make
./mini test2.gc
```

然后命令行会输出一堆东西！但是没有写ObjGenerator！

从这里面复制到test.ll里面！前四行不要动！

* 直接运行：

```
lli test.ll
```

* 生成可执行文件

```
./BinGen.sh test.ll
```

* 宏

加入无参数和有参数宏，实例如下：
```
#define INC(x) x=x+1
#define MAX 100
```
编译时会生成预处理文件.gcpp

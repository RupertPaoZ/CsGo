#### 宏

##### 实现

为了方便程序的书写，我们引入了宏的使用。我们实现了基本的简单宏和带参数的宏，例如：

```
#define MAX 100
#define INC(x) x=x+1
```

宏替换发生在整个编译程序开始之前，属于预处理的一部分。为了实现带参数的宏，我们定义了类`macroTable`每一个`macroTable`都储存了一个宏定义的名称、参数、和目标字符串。

```c++
class macroTable
{
    friend std::string preProcess(std::string filename);
private:
    std::string Name;                 // the name of this macro
    bool withArgs;                    // whether it is a macro with arg(s)
    std::vector<std::string> srcArgs; // args of the macro
    std::vector<std::string> defs;    // splited destination code which is used for replacing
    /* 
     * judge whether there is a macro in this sentence 
     * if exists, return true; 
     * else false.
     */
    bool match(std::string token);
    /*
     * replace the macro in input
     * input :
     *      std::string input : source code
     * return :
     *      std::string : processed code 
     */
    std::string replace(std::string input);
public:
    /* 
     * constructor
     * input :
     *      std::string firstPart : the name of the macro
     *      std::string secondPart : the destination code 
     */
    macroTable(std::string firstPart, std::string secondPart);
    ~macroTable() {}
};
```

`withArgs`标识了这条宏定义是否含有参数；`srcArgs`存储了定义中的参数的名称，用于宏展开时变量的替换；`defs`中存储了分隔开的目标字符串（将所有变量名（`[a-zA-Z_][0-9a-zA-Z_]*`）视为一个整体，任何一个非变量名中会出现符号视为一个整体，进行字符串分割），这样在宏展开时若出现了和参数中相同的字符串的，直接替换即可。

`match`函数对一段字符串进行查找，查看是否包括这张表所对应的宏，包含则返回`true`；否则返回`false`。

```c++
bool match(std::string token)
{
    if (token.find(this->Name) != token.npos)
        return true;
    else
        return false;
}
```

`replace`函数对输入字符串进行宏展开，若无参宏则直接替换，若是带有参数的宏则首先进行字符串的分割，然后检查参数数目是否与定义的参数一致，若不一致则报错提示。然后用提取出的参数替换目标字符串中的形参，最后返回替换完的字符串。

```c++
std::string replace(std::string input)
{
    int pos = input.find(this->Name);
    if (!withArgs)
    {
        // if there is no args, it is just a simple replace
        std::string head = input.substr(0, pos), tail = input.substr(pos + this->Name.length());
        return head + " " + this->defs[0] + " " + tail;
    }
    else
    {
        // if there is arg(s), we need to find out all args
        std::string head = input.substr(0, pos), cache = input.substr(pos);
        int rec = 1, delta = this->Name.length();
        do
        {
            delta++;
            if (cache[delta] == '(')
                rec++;
            else if (cache[delta] == ')')
                rec--;
        } while (rec != 0);
        std::string tail = cache.substr(delta + 1), body = cache.substr(this->Name.length() + 1, delta - this->Name.length());
        cache.clear();
        std::vector<std::string> args;
        rec = 1;
        // find all args
        for (int i = 0; i < body.length(); i++)
        {
            if (body[i] == ',' || body[i] == ' ' || (body[i] == ')' && rec == 1))
            {
                if (cache.length() != 0)
                {
                    args.push_back(cache);
                    cache.clear();
                }
            }
            else
            {
                if (body[i] == '(')
                    rec++;
                else if (body[i] == ')')
                    rec--;
                cache += body[i];
            }
        }
        // use the macro in wrong way
        if (args.size() != this->srcArgs.size())
        {
            perror("Wrong MACRO ARGS!");
        }
        std::vector<std::string> outputs(defs);
        for (int i = 0; i < this->srcArgs.size(); i++)
        {
            for (auto &output : outputs)
            {
                if (this->srcArgs[i] == output)
                    //if this substring is corresponding to an arg, use it to replcae the arg in the intermediate code
                {
                    output = args[i];
                }
            }
        }
        cache.clear();
        for (auto output : outputs)
        {
            cache += output;
        }
        return head + " " + cache + " " + tail;
    }
}
```

`macroTable`类的友函数`preProcess`是对外提供的预处理函数。函数会读取输入文件（`.gc`）的头部，形成一张包含所有`macroTable`的表`std::vector<macroTable> Macros`，然后根据这张表对程序进行宏展开，处理完的代码存放到`.gcpp`文件中。

```c++
std::string preProcess(std::string filename)
{
    std::ifstream fin(filename);
    // intermediate filename
    std::ofstream fout(filename + "pp");
    std::string fileContent;
    std::vector<macroTable> Macros;

    while (getline(fin, fileContent))
    {
        std::stringstream ss;
        std::string tmp;
        ss << fileContent;
        ss >> tmp;
        if (tmp.size() != 0)
        {
            if (tmp == "#define") //find macros
            {
                std::vector<std::string> lineTokens;
                while (ss >> tmp)
                    lineTokens.push_back(tmp);

                int rec = 0;
                std::string firstPart, secondPart;
                if (lineTokens[0].find('(') != lineTokens[0].npos)
                {
                    //if it is with args, it's name must end with a ')'
                    for (int i = 0;; i++)
                    {
                        firstPart += lineTokens[i] + " ";
                        if (lineTokens[i].find(')') != lineTokens[i].npos)
                        {
                            rec = i;
                            break;
                        }
                    }
                }
                else
                    firstPart = lineTokens[0];
                for (int i = rec + 1; i < lineTokens.size(); i++)
                {
                    secondPart += lineTokens[i];
                }
                // store the marco in macrotable
                Macros.push_back(macroTable(firstPart, secondPart));
            }
            else
            {
                // if this line is not a macro define, try to find macros in it and replace it.
                bool found = true;
                while (found)
                {
                    found = false;
                    for (auto macro : Macros)
                    {
                        while (macro.match(fileContent))
                        {
                            fileContent = macro.replace(fileContent);
                            found = true;
                        }
                    }
                }
                fout << fileContent << std::endl;
            }
        }
    }
    fin.close();

    return filename + "pp";
}
```

##### 示例

输入测试，测试带参宏展开与宏嵌套：

```
#define INC(x, y) y=x+1
#define SL(x) x*2

func main(void)(ret int){
    int x;
    int y;
    x,y=1,0;
    INC(SL(x),y);
    return 0;
}
```

处理结果如下：

```
func main(void)(ret int){
    int x;
    int y;
    x,y=1,0;
     y= x*2 +1 ;
    return 0;
}
```


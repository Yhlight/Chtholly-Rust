## Chtholly 珂朵莉

Chtholly是一门基于Rust + LLVM实现的编程语言，Chtholly以简洁，高性能，编译期安全为特征。

Chtholly遵循零成本抽象以及运行时极简原则，尽可能把事情交给编译期进行。

Chtholly文件后缀为`.cns`。

### 注释
```chtholly
// 单行注释

/*
多行注释
*/
````

### 主函数

```chtholly
fn main(args: string[]): Result<int, SystemError>
{

}
```

### 资源管理 (Resource Management)

Chtholly采用现代**所有权**和**借用**模型。

  * **所有权 (Ownership)：** Chtholly 中的每个值都有一个**所有者**。在任何时间点，一个值只能有一个所有者。当所有者超出作用域时，该值将被自动清理（Drop/析构），保证了内存安全。
  * **零成本抽象：** 所有权机制在编译期强制执行，运行时开销极低，无需垃圾回收（GC）。

### 不可变变量 (Immutable Variables)

Chtholly使用`let`声明不可变变量。

Chtholly编译器会自动推导变量的类型。

Chtholly支持类型注解。

  * **不可变性优先：** 默认情况下，所有绑定都是不可变的。`let` 关键字创建的变量，其绑定的**资源内容**在创建后不能通过该变量名被修改。

```chtholly
let a = 10;
let b = 30.4;
let c = 'a';
let d = "HelloWorld";
let e = [1, 2, 3, 4];

let a2: int = 10;
let b2: double = 30.4;
let c2: char = 'a';
let d2: string = "HelloWorld";
let e2: int[] = [1, 2, 3, 4];
```

### 可变变量 (Mutable Variables)

你可以使用`mut`声明可变变量。

  * **可变性：** `mut` 关键字允许所有者通过该变量**操作或修改**它所拥有的资源。可变性是作用于当前绑定的**权限**。

```chtholly
mut a = 10;
```

### 变量间的传递：移动语义 (Move Semantics)

变量间以**移动**进行传递。`let`和`mut`声明的变量相互之间可以自由赋值，这是因为变量相互之间的传递属于**资源转移**。

`let`和`mut`仅决定指向的资源是否可以被操作。

  * **资源移动：** 当所有权发生转移（例如赋值给新变量、作为参数传递给函数）时，资源的所有权从原变量转移给新变量。**原变量在移动后立即失效且不可用**，防止了资源被二次释放。
  * **零成本复制 (`Copy` Trait)：** 对于基本数据类型（如整数、浮点数、字符等），赋值操作不是移动，而是**零成本的按位复制**。在这种情况下，**原变量在赋值后仍然有效**。


```chtholly
let a = 10;
a = 10;  // 错误，不可变变量无法操控这部分资源
mut a2 = a; // 对于 Copy 类型 10，这里是复制，a 仍然可用。
a2 = 20;  // 正确，可变变量可以操控这部分资源
```

### 引用语义：借用 (Borrowing Semantics)

如果你想要同时共享一份资源，你可以使用引用符号`&`创建一个引用。

  * **借用：** 引用 (`&`, `&mut`) 允许在不转移所有权的情况下**临时访问**资源。借用必须遵守**生命周期**规则，生命周期用于**确保所有引用都比它们指向的数据存活的时间短**，从而防止悬垂引用。
  * 使用`&`创建**不可变引用（共享引用）**。
  * 使用`&mut`创建**可变引用（独占引用）**。

**借用规则（核心原则：读写互斥）：**
在任何时间点，对于同一份资源，以下两种借用情况只能存在一种：

  * **任意数量的共享引用 ($\&$)：** 允许多个共享引用同时存在，但它们都不能修改资源。

  * **仅一个可变引用 ($\&mut$)：** 允许通过该引用修改资源，但它必须是独占的。

  * 在某一作用域内，如果存在一个可变引用，则不能存在任何其他引用（无论是共享引用还是另一个可变引用）。这是 Chtholly 保证**数据竞争安全**的关键。

```chtholly
let a = 10;
let b = &a;  // 对a的资源的不可变引用

// 以下示例展示借用规则：
let mut x = 10;
let r1 = &x;    // 共享引用 R1
// let m1 = &mut x; // 错误：在 R1 存在时，不能创建可变引用 M1

// r1 超出作用域或不再使用...

let m2 = &mut x; // 正确：现在可以创建可变引用 M2
// let r2 = &x;     // 错误：在 M2 存在时，不能创建共享引用 R2
```

### 数据类型 (Data Types)

Chtholly有如下内置数据类型：

  * `int`: 默认使用的整数类型。它是i32的别名  
  * 精细划分的整数类型：`i8, i16, i32, i64` (有符号)；`u8, u16, u32, u64` (无符号)。
  * `char`: 字符类型。
  * `double`: 默认使用的浮点数类型，对应 **64 位浮点数 (f64)**。
  * 精细划分的浮点数类型：`f32`，`f64`。
  * `void`: 表示空类型，通常用于函数不返回任何值的情况。
  * `bool`: 布尔类型。
  * `string`: 表示动态字符串，默认使用 **UTF-8 编码**。
  * 数组：`int[]` (动态数组，所有权在堆上)，`int[4]` (静态数组，大小编译期确定，通常在栈上)。
  * 函数：`function`，`(int, int): void`

这部分的类型注解使用定义时的标识符：

  * `class`
  * `struct`

### 类型转换

Chtholly支持类型转换，使用type_cast<T>()。

这是一个静态类型转换函数。

此函数为内置函数，不需要导入。

```chtholly
let a: i8 = type_cast<i8>(10.5);
```

### 溢出处理
所有溢出行为皆进行回绕  

### 运算符

Chtholly支持编程语言中的标准运算符。

```Chtholly
+ - * / %

== != > >= < <= !

& | ^ ~ << >>

= += -= *= /= %=

&& ||

++ --
```

### 函数 (function)

Chtholly使用`fn`关键字定义函数。

```Chtholly
fn hello(): void
{
    println("HelloWorld");
}

hello();

let test: (): void = hello;
```

### lambda

Chtholly支持lambda表达式。

[]可以捕获外部变量，遵循a所有权，&a不可变捕获，&mut可变捕获的语义  

```Chtholly
let test = [](): void {
    println("HelloWorld");
}();
```

### 类 (Class)

你可以使用`class`关键字定义类。

#### 成员变量

Chtholly使用`let`声明不可变成员变量。

Chtholly使用`mut`声明可变成员变量。

```Chtholly
class Person
{
    let name: string = "yhlight";  // 不可变成员变量，可以赋予默认值
    let age: int;

    mut des: string;  // 可变成员变量，也可以赋予默认值
}
```

#### 构造函数

Chtholly使用C++语法的构造函数  

```Chtholly
class Person
{
    let name: string = "yhlight";  // 不可变成员变量，可以赋予默认值
    let age: int;

    mut des: string;  // 可变成员变量，也可以赋予默认值

    Person(age: int, des: string)
    {
        Self.age = age;  // 使用 Self 表示本身
        Self.des = des;
    }
}

let test = Test();  // 空初始化，遵循零初始化
let test2 = Test(18, "yhlight");  // 构造函数初始化
let test3 = Test{"yhlight", 18, "yinghuolight"};  // 聚合初始化(注意，聚合初始化会覆盖默认值)
let test4 = Test{des: "yhlight", age: 18};  // 指定初始化(注意，指定初始化会覆盖默认值)
```

#### 析构函数

在必要的时候，Chtholly可以使用析构函数管理资源释放后的行为。

析构函数将在所有权失效后执行调用。

从底层来看，析构函数本质是一个专门用于资源释放的函数。

```Chtholly
class fileSysteam
{
    ~fileSysteam()
    {

    }
}
```

#### 成员函数与Self

在Chtholly之中，使用Self表示本身。

你可以使用`fn`定义成员函数。

成员函数的第一位函数参数必须是所有权语义，即self(所有权)，&self(不可变)，&mut self(可可变)。

谨慎使用self，self会导致实例访问后所有权失效。

如果没有所有权语义，则表示与对象无关，此时需要使用::来访问。

使用Self表示本身。

```Chtholly
class Person
{
    let name: string = "yhlight";  // 不可变成员变量，可以赋予默认值
    let age: int;

    mut des: srting;  // 可变成员变量，也可以赋予默认值

    Person(age: int, des: string)
    {
        Self.age = age;  // 使用 Self 表示本身
        Self.des = des;
    }

    fn show(self): void
    {
        println(self.name, self.age, self.des);
    }

    fn print(): void
    {
        println("人");
    }

    fn add(self, other: Person): Self
    {
        return Self(self.age + other.age, string::combine(self.des, other.des));  // 返回对象本身
    }
}

let p = Person();
p.show();
Person::print();
```

#### 权限修饰

Chtholly支持两种权限修饰符：public 和 private。

```Chtholly
class Person
{
public:
    let name: string = "yhlight";
    let age: int;
    mut des: string;

private:
    fn show(self): void
    {

    }

    fn print(): void
    {

    }
}
```

#### 继承

只支持组合式继承，请自行内嵌其他类对象。

#### 多态

暂不支持多态。

### 结构体 (Struct)

Chtholly使用`struct`关键字定义结构体。

结构体与类十分相似，但是struct是纯粹的数据集。

即struct不支持权限修饰符，不支持构造函数。

但是struct支持成员函数。

结构体不支持零初始化，这意味着你必须要保证结构体所有成员都得到了有效的初始化。

```Chtholly
struct Person
{
    let name: string;  // 支持默认值
    let age: int;
    mut des: string;

    fn show(self): void
    {
        println(self.name, self.age, self.des);
    }
}

let test = Test{"yhlight", 18, "des"};  // 聚合初始化(注意，聚合初始化会覆盖默认值)
let test2 = Test{name: "yhlight", id: 18};  // 指定初始化(注意，指定初始化会覆盖默认值)
```

### 枚举

Chtholly使用enum创建枚举。

Chtholly的枚举支持状态，即它们能够存储信息。

编译器必须具有自动解构枚举的能力。

例如在if语句中，if(color::RGB(r, g, b)) {} r, g, b就是解构的变量。

```Chtholly
enum color
{
    red,
    green,
    RGB(string, string, string),
}

let a: color = color::red;
```

### 数组

Chtholly使用类型 + []创建数组。

int[]为动态数组，int[4]为静态数组。

a[0] = 10;这一种访问方式属于安全的静态访问。

```Chtholly
let a: int[] = [1, 2, 3];
a[0] = 10;
```

### 选择结构
#### if-else

Chtholly使用C风格的if-else结构。

```Chtholly
if (a > b)
{
    print("a > b");
}
else if (a < b)
{
    print("a < b");
}
else
{
    print("a == b");
}
```

#### switch-case

Chtholly使用C风格的switch-case结构。

不一样的是，Chtholly的switch-case修复了C语言的switch-case的bug。

```Chtholly
switch(任意类型的变量 / 表达式)
{
    case 值1: {
        break;  // break现在不是防止穿透，而是跳出匹配
    }
    case 表达式: {
        break;
    }
    case 表达式2: {
        fallthrough;  // 如果需要穿透，请使用fallthrough
    }
}
```

### 循环结构
#### while循环

Chtholly使用标准C风格while循环。

```Chtholly
while (条件) {
    continue / break;
}
```

#### for循环

Chtholly使用标准C风格for循环。

```Chtholly
for (let i = 0; i < 10; i++) {
    continue / break;
}
```

#### foreach循环

Chtholly使用foreach循环。

对于所有容器对象，如果实现了`iterator`约束，则Chtholly会提供foreach循环。

item : container 为移动，对应iterator模块的iterator_move约束。

如果你需要决定如何安全地创建迭代器，请使用create_iterator_move约束，这将决定迭代器对象如何被创建。

item : container 为不可变引用，对应iterator模块的iterator_let约束。

如果你需要决定如何安全地创建迭代器，请使用create_iterator_let约束，这将决定迭代器对象如何被创建。

item : container 为可变引用，对应iterator模块的iterator_mut约束。

如果你需要决定如何安全地创建迭代器，请使用create_iterator_mut约束，这将决定迭代器对象如何被创建。

```Chtholly
for (item : container)
{

}
```

#### do-while循环

Chtholly使用标准C风格do-while循环。

```Chtholly
do
{

} while (condition);
```

### 泛型
#### 泛型函数
```Chtholly
fn add<T>(a: T, b: T): T
{
    return a + b;
}

add<int>(1, 2);

fn add<T = double>(a: T, b: T): T  // 默认值
{
    return a + b;
}

fn add<string>(a: string, b: string): string  // 特例化
{
    return string::concat(a, b);
}
```

#### 泛型类
```Chtholly
class Point<T>
{
    x: T;
    y: T;

    fn swap(self, other: Point<T>): Self
    {
        
    }

    fn swap(self, other: Point<T = int>): Self
    {

    }

    fn swap(self, other: Point<int>): Self
    {

    }
}

class Point<T = int>
{

}

class Point<int>
{

}

let p = Point<int>(1, 2);
```

#### 类内的泛型函数

在Chtholly之中，类，无论自身是否是泛型，都可以拥有泛型成员函数。

##### 常规类内的泛型函数
```Chtholly
class Printer
{
    fn print<T>(self, value: T)
    {
        print(value);
    }
}

fn main()
{
    let p = Printer();
    p.print<int>(10);  // 调用时指定类型
    p.print("hello");  // 或者让编译器自动推断类型
}
```

##### 泛型类内的泛型函数

泛型类内部也可以创建独立的泛型函数。

```Chtholly
class Point<T>
{
  // 方法的泛型参数 K, F 与类的泛型参数 T 是独立的
    fn test<K, F>()
    {

    }

    fn test2<K = int, F>()
    {

    }

    fn test2<K, F = int>()
    {

    }

    fn test2<K = int, F = int>()
    {

    }

    fn test2<int, int>()
    {

    }

}
```

### optional

Chtholly具有安全的optional类型。

optional需要optional模块的支持。

optional类型有两个主要方法，unwrap和unwarp_or。

```
let a = optional<int>(10);
```

### request

现在，你可以使用request来创建一个类的约束。

```chtholly
request compare<T>  // 支持泛型
{
    fn compare<K>(self, other: K);  // 必须实现这个函数

    fn test2()  // 如果没有self，那么这个函数可以具有代码体
    {

    }
}

class Test require compare
{
    
}
```

#### Self

在Chtholly之中，如果你在约束内部使用Self，那么会表示实现了这一个约束的类本身。

注意，这里有一个特别的注意事项，如果一个所需要的内容在约束时就可以确定，而不需要到实现后再确定。

那么你应该直接使用。

如果一个所需要的内容再约束时仍不能确定，在实现时不能够确定，那么我们建议使用Self::来进行区分，尽管它们的最终作用是一样的。

为什么效果一样？这是因为约束的语义最终确定是在实现类之中，这意味着item和Self::item是相同的，只是Self::item多了一层保护。

使用Self::可以让实现类必须显性提供一个参数值，从而避免不必要的错误。

#### 关联类型

在Chtholly之中，泛型无法同时表示多种类型。

这对于约束来说十分有局限，例如我想要让一个函数同时表达string, &string, &mut string。

泛型无法实现这样的功能，为此你可以使用关联类型，以便类型可以被复用。

关联类型仅限于约束使用。

```Chtholly
request iterator
{
    type item;
    fn next(&mut self): optional<Self::item>;  // 由实现类确定

    fn test2(a: Self::item);
    fn test3(a: &Self::item);
    fn test4(a: &mut Self::item);
}

class container requier iterator
{
    type item = int;  // 把所有的item参数替换为int

    fn next(&mut self): optional<item>
    {

    }

    fn test2(a: item)
    {

    }

    fn test3(a: &item)
    {

    }

    fn test4(a: &mut item)
    {
        
    }
}
```

#### 关联约束

如果一个约束需要其他的约束作为附属，那么你需要使用require进行请求。

这种请求行为是单向的，也被称为单向必要请求。

举个例子，假设create_iterator_move约束请求了iterator_move约束。

那么一旦类请求了create_iterator_move约束，就必须请求iterator_move约束。

create_iterator_move和iterator_move可以相互请求，这不会导致问题的产生，但为了简洁性，我们始终推荐开发者进行线性的规划。

```Chtholly
request iterator_move require create_iterator_move
{
    type item;
    fn next(&mut self): optional<item>;
}
```

#### 类型关联约束

对于type类型，如果type类型的注解是一个约束，那么type类型会期盼一个实现了这一个约束的类型。


```Chtholly
request create_iterator_move
{
    type IntoMoveIter: iterator_move;  // 要求一个实现了iterator_move的类型
    fn into_iter(self): Self::IntoMoveIter;
}
```

#### 内置约束

Chtholly提供了一系列的内置约束，来提供给开发者使用。

你只需要导入不同的模块即可使用。  

##### iterator

需要iterator模块的支持。

```Chtholly
request iterator_move require create_iterator_move
{
    type item;
    fn next(&mut self): optional<Self::item>;
}

request iterator_let require create_iterator_let
{
    type item;
    fn next(&mut self): optional<Self::item>;
}

request iterator_mut require create_iterator_mut
{
    type item;
    fn next(&mut self): optional<Self::item>;
}

request create_iterator_move
{
    type IntoMoveIter: iterator_move;
    fn into_iter(self): Self::IntoMoveIter;
}

request create_iterator_let
{
    type RefIter: iterator_let;
    fn iter(&self): Self::RefIter;
}

request create_iterator_mut
{
    type MutIter: iterator_mut;
    fn iter_mut(&mut self): Self::MutIter;
}
```

### 模块与import

Chtholly 支持模块系统，允许您将代码组织到多个文件中或使用标准库功能，`import`关键字用于加载另一个模块中的代码并将其合并到当前作用域。

#### 语法

`import` 语句接受两种形式：  
1.  **文件路径**: 一个字符串字面量，表示您想要包含的 Chtholly 文件（`.cns`）的路径。
2.  **标准库模块名**: 一个标识符，表示您想要导入的标准库模块。

import应该被用于导入模块，如果你需要导入模块中需要的成员，请使用use。

use语句同样支持别名。

路径默认是相对路径，相对当前文件。

```Chtholly
// 导入文件模块
import "path/to/your_module.cns";

// 导入标准库模块
import iostream;
```

#### 行为

你需要使用模块名::方法名来访问模块中的方法。

```Chtholly
import iostream;
use iostream::println as pl;  // 把这个函数导入到当前作用域

fn main()
{
    iostream::print("Hello, World!");
    println("Hello, World!");
}
```

#### 示例

假设您有一个名为 `math.cns` 的文件：

```Chtholly
// math.cns
fn add(a: int, b: int): int
{
    return a + b;
}
```

您可以在另一个文件 `main.cns` 中通过导入 `math.cns` 来使用 `add` 函数：

```Chtholly
// main.cns
import "math.cns";

fn main()
{
    let result = math::add(5, 10);
    print(result); // 将输出 15
}
```

#### 冲突防止

通常情况下，Chtholly默认使用文件名作为模块名。
在冲突时，你可以为模块指定一个别名。

```Chtholly
import "math.cns" as math;
import "math.cns" as math2;
```

### 创建模块的最佳实践
我们建议模块使用对象式而不是函数式  

### 包
模块被收录在哪一个包？  

```nota
// 例如这个是数学模块
package packageName;

fn add(a: int, b: int): int
{
    return a + b;
}

/////////////////////
import packageName::math;
import packageName2::math;

packageName::math::add(1, 2);  // 冲突时需要写出包名
```

### 错误处理 (Error Handling)

Chtholly采用 Result<T, E> 类型进行基于值的错误处理，以实现编译期强制检查和零成本抽象。

#### Result 类型

Result<T, E> 是一个枚举，用于封装可能成功或失败的操作结果：

```chtholly
enum Result<T, E>
{
    Pass(T),    // 成功，包含一个值 T
    Fail(E)    // 失败，包含一个错误 E
}
```

#### 错误传播：? 操作符

为了简洁地在函数间传递错误，Chtholly 提供了 ? 操作符。该操作符只能用于返回 Result 类型的函数内部：

1.  如果 Result 是 Pass(T)，解包出值 T。
2.  如果 Result 是 Fail(E)，立即将错误 E 从当前函数返回，实现错误传播。

```chtholly
fn process_file(path: string): Result<int, ErrorType>
{
    // 如果 file::open 失败，函数立即返回 Fail(ErrorType)
    let handle = file::open(path)?; 
    
    // ... 使用 handle ...
    return Pass(1); 
}
```

#### 错误处理：模式匹配

对于复杂的错误处理，建议使用 `switch` 结构对 `Result<T, E>` 进行模式匹配：

```chtholly
let operation_result = read_and_process("test.cns");

switch (operation_result)
{
    case Pass(data):
    {
        println("处理成功，数据为:", data);
        break;
    }
    case Fail(error):
    {
        println("处理失败，错误信息:", error.message);
        break;
    }
}
```

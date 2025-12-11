## Chtholly 珂朵莉：系统编程的新平衡

Chtholly 是一门**编译型、通用**的系统编程语言。它旨在为开发者提供一个**兼具 C++ 性能和 Rust 内存安全**的优雅环境。

Chtholly 遵循**零成本抽象**以及**运行时极简**原则，尽可能把事情交给编译期进行，实现：
1.  **默认安全 (Safety by Default)：** 采用所有权与借用模型，在编译期根除内存错误。
2.  **人体工程学 (Ergonomics)：** 采用经典的类/结构体语法和智能的生命周期推导，降低系统编程的学习成本。

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
fn main(args: string[]): Result<i32, SystemError>
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

let a2: i32 = 10;
let b2: f64 = 30.4;
let c2: char = 'a';
let d2: string = "HelloWorld";
let e2: i32[] = [1, 2, 3, 4];
```

### 可变变量 (Mutable Variables)

你可以使用`mut`修饰不可变变量，使其可变。

  * **可变性：** `mut` 关键字允许所有者通过该变量**操作或修改**它所拥有的资源。可变性是作用于当前绑定的**权限**。

```chtholly
let mut a = 10;
```

### 变量间的传递：移动语义 (Move Semantics)

变量间以**移动**进行传递。`let`和`let mut`声明的变量相互之间可以自由赋值，这是因为变量相互之间的传递属于**资源转移**。

`let`和`let mut`仅决定指向的资源是否可以被操作。

  * **资源移动：** 当所有权发生转移（例如赋值给新变量、作为参数传递给函数）时，资源的所有权从原变量转移给新变量。**原变量在移动后立即失效且不可用**，防止了资源被二次释放。
  * **零成本复制 (`Copy` Trait)：** 对于基本数据类型（如整数、浮点数、字符等），赋值操作不是移动，而是**零成本的按位复制**。在这种情况下，**原变量在赋值后仍然有效**。


```chtholly
let a = 10;
a = 10;  // 错误，不可变变量无法操控这部分资源
let mut a2 = a; // 对于 Copy 类型 10，这里是复制，a 仍然可用。
a2 = 20;  // 正确，可变变量可以操控这部分资源
```

### 引用语义：借用 (Borrowing Semantics)

如果你想要同时共享一份资源，你可以使用引用符号`&`创建一个引用。

  * **借用：** 引用 (`&`, `&mut`) 允许在不转移所有权的情况下**临时访问**资源。借用必须遵守**生命周期**规则，生命周期用于**确保所有引用都比它们指向的数据存活的时间短**，从而防止悬垂引用。
  * 使用`&`创建**不可变引用（共享引用）**。
  * 使用`&mut`创建**可变引用（独占引用）**。

**借用规则（核心原则：读写互斥）：**

在任何时间点，对于同一份资源，以下两种借用情况只能存在一种：

  * **任意数量的共享引用 (&)：** 允许多个共享引用同时存在，但它们都不能修改资源。

  * **仅一个可变引用 (&mut)：** 允许通过该引用修改资源，但它必须是独占的。

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

### 生命周期

生命周期是借用机制的核心，它用于确保所有引用都比它们指向的数据存活的时间短，从而防止悬垂引用。

#### 智能生命周期省略

Chtholly引入了智能的生命周期省略规则，旨在在 90% 的情况下消除手动生命周期注解，最大程度地减轻心智负担，同时保留 Rust 级别的编译期安全。

### 数据类型 (Data Types)

Chtholly有如下内置数据类型：

  * `i32`: 默认使用的整数类型。
  * 精细划分的整数类型：`i8, i16, i32, i64` (有符号)；`u8, u16, u32, u64` (无符号)。
  * `char`: 字符类型。
  * `f64`: 默认使用的浮点数类型。
  * 精细划分的浮点数类型：`f32, f64`。  
  * `void`: 表示空类型，通常用于函数不返回任何值的情况。
  * `bool`: 布尔类型。
  * `string`: 表示动态字符串，默认使用 **UTF-8 编码**。
  * 数组：`i32[]` (动态数组，所有权在堆上)，`i32[4]` (静态数组，大小编译期确定，通常在栈上)。
  * 函数：`function`，使用`(i32, i32): void`进行表示。

这部分的类型注解使用定义时的标识符：

  * `class`
  * `struct`

### 类型转换

Chtholly倾向于使用显式的 as 关键字进行原始类型之间的强制转换，以替代冗长的函数调用（如 type_cast<T>()）。

```chtholly
let float_val = 10.5;
let a: i8 = float_val as i8; // 结果为 10
```

### 溢出处理

所有溢出行为皆进行回绕。

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

Chtholly使用`let mut`声明可变成员变量。

```Chtholly
class Person
{
    let name: string = "yhlight";  // 不可变成员变量，可以赋予默认值
    let age: i32;

    let mut des: string;  // 可变成员变量，也可以赋予默认值
}
```

#### 构造函数

Chtholly使用C++语法的构造函数  

```Chtholly
class Person
{
    let name: string = "yhlight";  // 不可变成员变量，可以赋予默认值
    let age: i32;

    let mut des: string;  // 可变成员变量，也可以赋予默认值

    Person(age: i32, des: string)
    {
        self.age = age;
        self.des = des;
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
    let age: i32;

    let mut des: srting;  // 可变成员变量，也可以赋予默认值

    Person(age: i32, des: string)
    {
        self.age = age;
        self.des = des;
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
    let age: i32;
    let mut des: string;

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
    let age: i32;
    let mut des: string;

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
enum color<T>
{
    red,
    green,
    RGB(string, string, string),
}

let a: color = color::red;
```

### 数组

Chtholly使用类型 + []创建数组。

i32[]为动态数组，i32[4]为静态数组。

a[0] = 10;这一种访问方式属于安全的静态访问。

```Chtholly
let a: i32[] = [1, 2, 3];
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

add<i32>(1, 2);

fn add<T = f64>(a: T, b: T): T  // 默认值
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

    fn swap(self, other: Point<T = i32>): Self
    {

    }

    fn swap(self, other: Point<i32>): Self
    {

    }
}

class Point<T = i32>
{

}

class Point<i32>
{

}

let p = Point<i32>(1, 2);
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
    p.print<i32>(10);  // 调用时指定类型
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

    fn test2<K = i32, F>()
    {

    }

    fn test2<K, F = i32>()
    {

    }

    fn test2<K = i32, F = i32>()
    {

    }

    fn test2<i32, i32>()
    {

    }

}
```

### optional

Chtholly具有安全的optional类型。

optional需要optional模块的支持。

optional类型有两个主要方法，unwrap和unwarp_or。

```
let a = optional<i32>(10);
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
fn add(a: i32, b: i32): i32
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

模块与物理结构的目录存在关联。

例如package std;那么此模块将会被存放在std文件夹之中。

package std::math;你可以使用这样在方式表达具有一定结构的模块。

所有的模块都应该被收录在module文件夹之中。

```nota
// 例如这个是数学模块
package packageName;

fn add(a: i32, b: i32): i32
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
    Ok(T),    // 成功，包含一个值 T
    Err(E)    // 失败，包含一个错误 E
}
```

#### 错误传播：? 操作符

为了简洁地在函数间传递错误，Chtholly 提供了 ? 操作符。该操作符只能用于返回 Result 类型的函数内部：

1.  如果 Result 是 Ok(T)，解包出值 T。
2.  如果 Result 是 Err(E)，立即将错误 E 从当前函数返回，实现错误传播。

```chtholly
fn process_file(path: string): Result<i32, ErrorType>
{
    // 如果 file::open 失败，函数立即返回 Err(ErrorType)
    let handle = file::open(path)?; 
    
    // ... 使用 handle ...
    return Ok(1); 
}
```

#### 错误处理：模式匹配

对于复杂的错误处理，建议使用 `switch` 结构对 `Result<T, E>` 进行模式匹配：

```chtholly
let operation_result = read_and_process("test.cns");

switch (operation_result)
{
    case Ok(data):
    {
        println("处理成功，数据为:", data);
        break;
    }
    case Err(error):
    {
        println("处理失败，错误信息:", error.message);
        break;
    }
}
```

## 编译期编程体系

Chtholly 严格遵循**运行时极简**原则，通过一套强大的编译期编程体系（Const System）将复杂计算和代码生成任务前置到编译阶段，从而确保零运行时开销。

所有编译期操作都必须使用 `const` 关键字显式标记，并在编译时（Compile-Time）完成求值。

## 核心原则

| 原则 | 描述 |
| :--- | :--- |
| **编译期求值** | 任何标记为 `const` 的表达式，必须能在编译阶段被完全求值，不允许依赖运行时数据。 |
| **无副作用** | 编译期代码（`const fn`）必须是**纯函数**。不允许进行 I/O、网络通信、内存分配或任何影响程序状态的副作用。 |
| **零运行时开销** | 所有 `const` 表达式在最终生成的机器码中，将被其计算出的**最终常量值**替换。 |

## const常量定义

`const` 关键字用于定义在编译期已知且不可变的常量。

### 语法

```chtholly
const NAME: Type = expression;
```

### 规则与限制

1.  **必须显式标注类型：** 与 `let` 变量不同，`const` 常量必须显式标注其类型，以确保类型安全。
2.  **必须可被求值：** 赋值表达式必须是一个可以在编译阶段计算出确定值的**常量表达式**（Const Expression）。
3.  **不允许引用或借用：** `const` 常量不能包含运行时借用 (`&T`) 或指针。然而，可以包含指向其他静态内存区域（例如字符串字面量）的引用。

### 示例

```chtholly
// 基础类型常量
const MAX_USERS: i32 = 1000;

// 使用编译期函数计算常量
const ARRAY_SIZE: i32 = const_math::calculate_size(10, 5);

// 数组常量
const PRIME_NUMBERS: i32[] = [2, 3, 5, 7, 11];

// 结构体常量（假设 Point 是 Copy 类型）
const ORIGIN: Point = Point { x: 0, y: 0 }; 
```

## const fn编译期函数

`const fn` 关键字用于定义可在编译期调用的函数。

### 语法

```chtholly
const fn function_name(params): return_type
{
    // ... const expression body
}
```

### 调用规则

1.  **调用上下文：** `const fn` 只能在编译期上下文（Const Context）中被调用，例如：
      * 定义另一个 `const` 常量或 `const fn` 的函数体内部。
      * 用作泛型参数（Const Generics）。
      * 用作数组长度。
2.  **运行时兼容：** `const fn` 默认也是一个普通的运行时函数（Runtime Function）。如果它在运行时上下文被调用，它将作为一个普通的函数被编译和执行。

### 示例

```chtholly
// 编译期函数：计算斐波那契数列（假设输入小于某个安全值）
const fn const_fibonacci(n: i32): i32
{
    if n == 0 || n == 1 {
        return n;
    }
    // 递归调用也发生在编译期
    return const_fibonacci(n - 1) + const_fibonacci(n - 2);
}

// 在常量定义中使用
const FIB_10: i32 = const_fibonacci(10); 

// 在运行时调用 (合法)
fn main(): Result<i32, SystemError>
{
    let runtime_result = const_fibonacci(20); // 运行时执行
    // ...
}
```

## Const Struct 常结构体

Chtholly支持常结构体，常结构体内部所有成员皆为const成员。

常结构体是编译型产物，不会在运行时中生成。

所有的数据都会以各种合理的方式传递给运行时。

常结构体的成员允许延迟赋值，但是仅限于编译期的使用。

```chtholly
const struct Point {
    const x: i32;  // 允许延迟赋值
    const y: i32;

    const fn print(&self): void
    {

    }
}
```

## Const Context 内部规则（允许与限制）

为了确保编译期执行的安全性和确定性，`const fn` 内部的代码受到严格限制。

### 允许的操作（Const Expression）

| 操作类型 | 描述 |
| :--- | :--- |
| **数学运算** | 整数、浮点数的加减乘除和位操作。 |
| **控制流** | `if/else`、`switch` 表达式。 |
| **循环** | **`while` 循环** 和 **`for` 循环**（要求循环次数在编译期可推导）。编译器必须能证明循环会终止。 |
| **函数调用** | 只能调用其他 **`const fn`**。 |
| **结构体操作** | 创建和访问 `const` 兼容的结构体。 |
| **数组/元组操作** | 索引访问、创建数组。 |

### 严格禁止的操作（Side Effects）

以下操作会引入运行时依赖或副作用，在 `const fn` 中是**禁止**的，编译期会报错：

1.  **I/O 操作：** 读写文件、网络请求、打印到控制台 (`print()` 函数)。
2.  **内存分配：** 任何涉及堆分配的操作（例如：创建动态 `string` 或 `Array<T>`，除非它们是编译期可构造的静态大小）。
3.  **运行时依赖：** 获取系统时间、生成随机数、获取用户输入。
4.  **修改状态：** 对静态可变变量或任何外部状态进行修改。
5.  **非 `const fn` 调用：** 调用任何未标记为 `const fn` 的函数。

## 编译期数据类型 (Const Data Types)

并非所有 Chtholly 类型都可以在编译期上下文中使用。只有那些**完全在编译期内存模型**中可表示的类型才能作为 `const` 常量或 `const fn` 的参数/返回值。

| 允许的类型 | 描述 |
| :--- | :--- |
| **原始类型** | `i32`, `f64`, `bool`, `char`。 |
| **不可变数组** | `T[N]`，其中 `T` 是编译期类型，`N` 是编译期常量。 |
| **编译期结构体** | 仅包含其他编译期兼容类型的字段的结构体。 |
| **枚举** | 任何枚举类型。 |
| **字符串字面量** | 静态的字符串引用（指向静态内存）。 |

## 编译期错误处理

如果一个 `const fn` 在编译期求值时遇到错误（例如整数除以零，数组越界），编译器必须提供清晰的机制来处理。

### 机制：编译期 `Result<T, E>`

Chtholly 应该扩展其 `Result<T, E>` 错误处理机制 来支持编译期：

1.  **`const fn` 返回 `Result`：** 允许 `const fn` 返回 `Result<T, E>`，以表示可能失败的操作。
2.  **编译期强制：** 如果一个 `const` 常量依赖于一个返回 `Result` 的 `const fn`，则开发者**必须**在编译期处理 `Err` 分支。

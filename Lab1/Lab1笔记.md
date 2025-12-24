# 1. 有符号数-补码

![一篇彻底学懂补码-CSDN博客](https://img-blog.csdnimg.cn/20210312193345394.png)



### 1. 核心模型：模运算环 (Modular Arithmetic Ring)

- **本质区别**：

  - **数学整数**：定义在无限延伸的**数轴**上。
  - **计算机整数**：定义在首尾相接的**有限环**上（模 $2^w$）。

- 运算基础：所有加减法本质均为同余运算。

  

  $$A + B \equiv (A+B) \pmod{2^w}$$

  $$A - B \equiv A + (-B) \pmod{2^w}$$

- <font color="red">**实战技巧 (from `isTmax`, `negate`)**：</font>

  - <font color="red">利用环的特性构造常数：$T_{max} + 1 = T_{min}$。</font>
  - <font color="red">相反数计算：$-x = \sim x + 1$。</font>

### 2. 关键数值锚点 (以 32 位为例)

补码环上的四个关键刻度，体现了“数值连续”与“二进制断裂”的统一。

| **数值含义** | **符号**  | **十六进制 (Hex)** | **二进制 (Binary)** | **环上位置**            |
| ------------ | --------- | ------------------ | ------------------- | ----------------------- |
| **零**       | $0$       | `0x00000000`       | `0...000`           | 12点钟方向 (起点)       |
| **最大负数** | $-1$      | `0xFFFFFFFF`       | `1...111`           | 逆时针第一格 (回归零点) |
| **最大正数** | $T_{max}$ | `0x7FFFFFFF`       | `01...11`           | 顺时针最大极限          |
| **最小负数** | $T_{min}$ | `0x80000000`       | `10...00`           | 6点钟方向 (符号突变点)  |

- **连续性**：数值上，$-1$ 加 $1$ 回归 $0$（进位溢出被截断）。
- **断裂点**：$T_{max}$ 加 $1$ 突变为 $T_{min}$（正溢出，符号位由 0 变 1）。
- <font color="red">**实战技巧 (from `isAsciiDigit`, `conditional`, `howManyBits`)**：</font>
  - <font color="red">**符号位提取**：`x >> 31`。正数得到 $0$ (`0x00...0`)，负数得到 $-1$ (`0xFF...F`)。</font>
  - <font color="red">**掩码构造**：利用算术右移的符号扩展特性。例如 `(!!x) << 31 >> 31` 可生成全0或全1掩码。</font>

### 3. 两种“取反”操作的本质

在补码环中，存在两种不同的对称操作，二者恒差 1。

#### A. 按位取反 (Bitwise NOT, `~x`)

- **数学定义**：$\sim x = (2^w - 1) - x$
- **几何意义**：关于**数值轴**（约为 -0.5）的对称。
- **对应关系**：
  - $0 \leftrightarrow -1$ (`00...0` $\leftrightarrow$ `11...1`)
  - $T_{min} \leftrightarrow T_{max}$

#### B. 算术取负 (Negation, `-x`)

- **数学定义**：$-x \equiv 2^w - x \pmod{2^w}$
- **几何意义**：关于**零点 0** 的对称（加法逆元）。
- **对应关系**：
  - $x + (-x) = 0$

### 4. 黄金公式：联系与推导

由于 $0$ 没有 $-0$（唯一性），导致两种操作存在偏移：

$$-x = \sim x + 1$$

$$\sim x = -x - 1$$

- **推论**：补码系统中，求相反数 $\equiv$ 按位取反再加 1。

### 5. 特殊性质 (Corner Case)

- TMin 的奇异性：

  $$-T_{min} = T_{min}$$

  - **原因**：在模 $2^w$ 环上，$T_{min}$ 位于 6 点钟方向，其关于零点的对称点正是它自己（类似于圆的直径）。
  - **后果**：对最小负数求绝对值或取反，结果仍是负数（溢出）。

- **<font color="red">0 的唯一性 (from `logicalNeg`)</font>**：

  - <font color="red">**特性**：$0$ 是唯一一个及其相反数（$-0=0$）的符号位都为 0 的数。</font>
  - <font color="red">**应用**：利用 `x | (~x + 1)` 的符号位来高效判断 `x` 是否为 0。</font>

# 2. 浮点数-IEEE格式

<img src="https://img-blog.csdnimg.cn/img_convert/8c6c450ec870457fac9ce4cd4d2e1ad8.png" alt="浮点数转字符字符串转换工具包-CSDN博客" style="zoom:50%;" />

+ **符号位 s** (1 bit)
+ **指数位 E** (8 bits)：存储的是**移码**。
  - <font color="red">**真实指数** $Exp = E - Bias$</font>
  - <font color="red">**偏置值** $Bias = 2^{8-1} - 1 = 127$</font>
+ **尾数位 M** (23 bits)：存储的是小数部分。

------

### 1. 规格化的值 (Normalized)

- **条件**：阶码 $E$ 不全为 0，也不全为 1 ($1 \sim 254$)。

- 公式：

  

  $$V = (-1)^s \times (1.M) \times 2^{E-127}$$

- **特点**：尾数隐含一个开头的 $1$。

- **范围**：$E$ 对应真实指数 $-126 \sim 127$。

- <font color="red">**实战技巧 (from `floatFloat2Int`, `floatScale2`)**：</font>

  - <font color="red">计算时需手动加上隐含的 $1$：`(1 << 23) | frac`。</font>
  - <font color="red">乘以 2 等价于指数加 1 (`exp + 1`)。</font>

### 2. 非规格化的值 (Denormalized)

- **条件**：阶码 $E$ 全为 0。

- 公式：

  $$V = (-1)^s \times (0.M) \times 2^{1-127}$$

- **特点**：

  - 尾数隐含一个开头的 $0$。
  - <font color="red">**真实指数固定为** $1 - Bias = -126$ (用于实现与规格化数的平滑过渡)。</font>

- **作用**：提供 $0$ 的表示，以及接近 $0$ 的极小数值。

- <font color="red">**实战技巧 (from `floatScale2`, `floatPower2`)**：</font>

  - <font color="red">乘以 2 等价于尾数左移一位 (`frac << 1`)。</font>
  - <font color="red">**最小正数极限**：$V_{min} = 2^{-126} \times 2^{-23} = 2^{-149}$。小于此值即为下溢 (Underflow)。</font>

### 3. 特殊值 (Special Values)

- **条件**：阶码 $E$ 全为 1 ($255$)。
- **分类**：
  - **无穷大 (INF)**：尾数 $M$ 全为 0。
    - <font color="red">实战：`0x7F800000` (+INF), `0xFF800000` (-INF)。</font>
  - **非数值 (NaN)**：尾数 $M$ 不全为 0。
- <font color="red">**实战技巧 (from `floatScale2`)**：</font>
  - <font color="red">操作前需优先检查是否为特殊值，特殊值通常直接返回原值。</font>
  - <font color="red">运算溢出时应返回无穷大。</font>

------

### 4. <font color="red">浮点数运算的本质 (总结)</font>

- <font color="red">浮点数运算不是简单的位操作，而是对**符号、指数、尾数**三个字段的分别处理和重新组合。</font>
- <font color="red">核心难点在于处理**规格化与非规格化的边界过渡**，以及**溢出/下溢**的特殊情况。</font>
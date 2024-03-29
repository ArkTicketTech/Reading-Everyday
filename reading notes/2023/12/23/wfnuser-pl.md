## 类型良好和类型推断(Well typing and type inference)
在给定的类型系统中，如果存在一种类型 $A$，使得 $\Gamma \vdash M : A$ 是有效的断言，即项 $M$ 可以被赋予某种类型，那么对于环境 $\Gamma$ 来说项 $M$ 是良类型的。

对于一个项的推导（以及类型的推导），被称为类型推断问题。在由规则 $(Env\ \phi)$、$(Val\ n)$ 和 $(Val\ +)$ 组成的简单类型系统中，在空环境中可以推导出项 $1+2$ 的类型是 $Nat$。

现在假设我们添加了一个带有前提 $\Gamma \vdash \lozenge$ 和结论 $\Gamma \vdash true : Bool$ 的类型规则。在这个新的类型系统中，我们无法为项 1+true 推断任何类型，因为没有规则可以将自然数与布尔值相加。由于对于 1+true 没有任何推导，我们说 1+true 无法被赋予类型，或者说它是不良类型，或者说它存在类型错误(typing error)。

我们可以进一步添加一个带有前提 $\Gamma \vdash M : Nat$ 和 $\Gamma \vdash N : Bool$ 以及结论 $\Gamma \vdash M+N : Nat$ 的类型规则（例如，将 $true$ 解释为 $1$）。在这种类型系统中，可以为项 $1+true$ 推断一种类型，现在它是良类型的。

因此，对于给定项的类型推断问题对于所讨论的类型系统非常敏感。针对不同类型系统的类型推断算法复杂度迥异，有的系统甚至无法设计出可行的算法。

对于显式类型的过程性语言（例如 Pascal）的类型推断问题相对容易解决；我们将在第 8 节中讨论它。对于隐式类型的语言，如 ML，类型推断问题要复杂得多，我们在本书中不讨论它。基本算法是众所周知的（文献中有几种描述），并且被广泛使用。但是，实际中使用的算法版本通常较复杂，仍在研究中。

在存在多态性(polymorphism)的情况下，类型推断问题变得特别困难（在第 5 节讨论多态性）。对于 Ada、CLU 和 Standard ML 等显式类型多态功能的类型推断问题在实践中是可处理的。然而，这些问题通常是通过算法来解决的，而不是首先描述相关的类型系统。最通用的多态性的类型系统由 λ-演算表示，它的类型推断算法相对容易，我们将在第 8 节中介绍它。然而，解决方案的简洁性依赖于不实用的冗长类型注释。为了使这种通用多态性变得实用，必须省略一些类型信息。这种类型推断问题仍然是活跃研究的领域。
- # Lecture 2: Elimination
	- 如果 $A$ 矩阵是一个好矩阵 那么消元法是奏效的
	- 那么问题在于什么是好矩阵
	-
	- 消元法
		-
$$
		  \begin{pmatrix}
		  1 & 2 & 1 \\
		  3 & 8 & 1 \\
		  0 & 4 & 1 \\
		  \end{pmatrix}
		  $$		- first pivot
			-
$$
			  \begin{pmatrix}
			  1 & 2 & 1 \\
			  3 & 2 & -2 \\
			  0 & 4 & 1 \\
			  \end{pmatrix}
			  $$		- second pivot
			-
$$
			  \begin{pmatrix}
			  1 & 2 & 1 \\
			  0 & 2 & -2 \\
			  0 & 0 & 5 \\
			  \end{pmatrix}
			  $$		- $A$ -> $U$
	-
	- 消元法什么时候失败？
		-
$$
		  \begin{pmatrix}
		  1 & 2 & 1 \\
		  3 & 8 & 1 \\
		  0 & 4 & -4 \\
		  \end{pmatrix}
		  $$		- 消元过程中某行以下全是0 则无法消元成功
	- 增广矩阵 augmented matrix
		-
$$
		  \begin{pmatrix}
		  1 & 2 & 1 & 2 \\
		  3 & 8 & 1 & 12\\
		  0 & 4 & 1 & 2\\
		  \end{pmatrix}
		  $$		-
$$
		  \begin{pmatrix}
		  1 & 2 & 1 & 2 \\
		  0 & 2 & -2 & 6\\
		  0 & 0 & 5 & -10\\
		  \end{pmatrix}
		  $$		- $U x = c$
		- 回代
			$z = -2 \\ y = 1 \\z = 2$
	- E elementary 初等矩阵 elimination 消元矩阵
		-
$$
		  \begin{pmatrix}
		  1 & 0 & 0 \\
		  0 & 1 & 0 \\
		  0 & -2 & 1 \\
		  \end{pmatrix}
		  \cdot
		  \begin{pmatrix}
		  1 & 0 & 0 \\
		  -3 & 1 & 0 \\
		  0 & 0 & 1 \\
		  \end{pmatrix}
		  \cdot
		  \begin{pmatrix}
		  1 & 2 & 1 \\
		  3 & 8 & 1 \\
		  0 & 4 & 1 \\
		  \end{pmatrix}
		  =
		  \begin{pmatrix}
		  1 & 2 & 1 \\
		  0 & 2 & -2 \\
		  0 & 0 & 5 \\
		  \end{pmatrix}
		  $$ 
		  
		  $$
		  E_{32} (E_{21} A) = U
		  $$ 

		  $$
		  (E_{32} E_{21}) A = U
		  $$		
		  - 结合律成立 - associative law
	- permutation matrix
		- exchange rows 1 and 2
			-
$$
			  \begin{pmatrix}
			  0 & 1  \\
			  1 & 0  \\
			  \end{pmatrix}
			  \cdot
			  \begin{pmatrix}
			  a & b  \\
			  c & d \\
			  \end{pmatrix}
			  $$		
			  - exchange cols 1 and 2
			-
$$
			  \begin{pmatrix}
			  a & b  \\
			  c & d  \\
			  \end{pmatrix}
			  \cdot
			  \begin{pmatrix}
			  0 & 1  \\
			  1 & 0 \\
			  \end{pmatrix}
			  $$	
			  - inverse matrix
		-
$$
		  \begin{pmatrix}
		  1 & 0 & 0 \\
		  3 & 1 & 0 \\
		  0 & 0 & 1 \\
		  \end{pmatrix}
		  \cdot
		  \begin{pmatrix}
		  1 & 0 & 0 \\
		  -3 & 1 & 0 \\
		  0 & 0 & 1 \\
		  \end{pmatrix}
		  =
		  \begin{pmatrix}
		  1 & 0 & 0 \\
		  0 & 1 & 0 \\
		  0 & 0 & 1 \\
		  \end{pmatrix}
		  $$		
		  -
$$E^{-1} E = I$$	

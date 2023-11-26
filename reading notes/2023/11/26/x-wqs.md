pip install pyinstaller
mac系统里打出来mac系统里的包，win打出来win系统的包。根据操作系统不同要有所区别。

pip freeze > requirements.txt
pip install -r requirements.txt
pyinstaller -F xxxxx

多文件：pyinstaller -D app.py
.spec 打包生成的配置文件
build 是编译过程的代码
dist是打完包的结果。

单文件：pyinstaller -F app.py     
pyinstaller -F app.py -n name

v1 = os.path.abspath(__file__)  ## 当前py文件目录
Base_dir = os.path.dirname(os.path.abspath(__file__))  ## 当前py文件上一层目录

with open(os.path.join(Base_dir, "account.txt"), mode='r', encoding='utf-8') as f:


当单文件打包后，涉及到读取文件的路径会报错。单文件打包会生成一个临时文件， 可以用sys.argv[0]获取可执行文件所在目录 

os.path.realpath(sys.argv[0])
Base_dir = os.path.dirname(os.path.realpath(sys.argv[0]))

打包的时候可以找到关联的包，一并导入。如果是动态导入模块的代码，无法找到关联的包。
card = importlib.import_module("utils.card")
可根据.spec文件导包
